// 
// eqlms_cccf_blind_example.c
//
// Tests least mean-squares (LMS) equalizer (EQ) on a
// signal at the symbol level.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <complex.h>
#include <getopt.h>
#include <time.h>
#include "liquid.h"

#define OUTPUT_FILENAME "eqlms_cccf_blind_example.m"

// print usage/help message
void usage()
{
    printf("Usage: eqlms_cccf_blind_example [OPTION]\n");
    printf("  u/h   : print usage\n");
    printf("  n     : number of symbols, default: 500\n");
    printf("  s     : SNR [dB], default: 30\n");
    printf("  c     : number of channel filter taps (minimum: 1), default: 5\n");
    printf("  m     : modulation scheme (qpsk default)\n");
    liquid_print_modulation_schemes();
}

int main(int argc, char*argv[])
{
    srand(time(NULL));

    // options
    unsigned int num_symbols=500;   // number of symbols to observe
    unsigned int hc_len=5;          // channel filter length
    unsigned int p=3;               // equalizer length (symbols, hp_len = 2*k*p+1)
    float mu = 0.08f;               // learning rate
    float SNRdB = 30.0f;            // signal-to-noise ratio [dB]

    // modulation type
    unsigned int bps=2;
    modulation_scheme ms = LIQUID_MODEM_QPSK;

    unsigned int k=2;               // matched filter samples/symbol
    unsigned int m=5;               // matched filter delay (symbols)
    float beta=0.3f;                // matched filter excess bandwidth factor

    int dopt;
    while ((dopt = getopt(argc,argv,"uhn:s:c:m:")) != EOF) {
        switch (dopt) {
        case 'u':
        case 'h':   usage();                    return 0;
        case 'n':   num_symbols = atoi(optarg); break;
        case 's':   SNRdB = atof(optarg);       break;
        case 'c':   hc_len = atoi(optarg);      break;
        case 'm':
            liquid_getopt_str2modbps(optarg, &ms, &bps);
            if (ms == LIQUID_MODEM_UNKNOWN) {
                fprintf(stderr,"error: %s, unknown/unsupported modulation scheme '%s'\n", argv[0], optarg);
                return 1;
            }
            break;
        default:
            fprintf(stderr,"error: %s, unknown option\n", argv[0]);
            return 1;
        }
    }

    // validate input
    if (hc_len == 0) {
        fprintf(stderr,"error: %s, channel must have at least 1 tap\n", argv[0]);
        exit(1);
    }

    // derived values
    unsigned int hm_len = 2*k*m+1;   // matched filter length
    unsigned int hp_len = 2*k*p+1;   // equalizer filter length
    unsigned int num_samples = k*num_symbols;

    // bookkeeping variables
    float complex sym_tx[num_symbols];  // transmitted data sequence
    float complex x[num_samples];       // interpolated time series
    float complex y[num_samples];       // channel output
    float complex z[num_samples];       // equalized output

    float hm[hm_len];                   // matched filter response
    float complex hc[hc_len];           // channel filter coefficients
    float complex hp[hp_len];           // equalizer filter coefficients

    unsigned int i;

    // generate matched filter response
    liquid_firdes_rnyquist(LIQUID_RNYQUIST_RRC, k, m, beta, 0.0f, hm);
    interp_crcf interp = interp_crcf_create(k, hm, hm_len);

    // create the modem objects
    modem mod   = modem_create(ms, bps);
    modem demod = modem_create(ms, bps);
    unsigned int M = 1 << modem_get_bps(mod);

    // generate channel impulse response, filter
    hc[0] = 1.0f;
    for (i=1; i<hc_len; i++)
        hc[i] = 0.07f*(randnf() + randnf()*_Complex_I);
    firfilt_cccf fchannel = firfilt_cccf_create(hc, hc_len);

    // generate random symbols
    for (i=0; i<num_symbols; i++)
        modem_modulate(mod, rand()%M, &sym_tx[i]);

    // interpolate
    for (i=0; i<num_symbols; i++)
        interp_crcf_execute(interp, sym_tx[i], &x[i*k]);
    
    // push through channel
    float nstd = powf(10.0f, -SNRdB/20.0f);
    for (i=0; i<num_samples; i++) {
        firfilt_cccf_push(fchannel, x[i]);
        firfilt_cccf_execute(fchannel, &y[i]);

        // add noise
        y[i] += nstd*(randnf() + randnf()*_Complex_I)*M_SQRT1_2;
    }

    // push through equalizer
    float hpf[hp_len];
    liquid_firdes_rnyquist(LIQUID_RNYQUIST_RRC, k, p, beta, 0.0f, hpf);
    for (i=0; i<hp_len; i++)
        hp[i] = hpf[i] / (float)k;

    eqlms_cccf eq = eqlms_cccf_create(hp, hp_len);
    eqlms_cccf_set_bw(eq, mu);

    float complex d_hat = 0.0f;
    for (i=0; i<num_samples; i++) {
        eqlms_cccf_push(eq, y[i]);
        eqlms_cccf_execute(eq, &d_hat);

        // store output
        z[i] = d_hat;

        // check to see if buffer is full
        if ( i < hp_len) continue;

        // decimate by k
        if ( (i%k) != 0 ) continue;

        // estimate transmitted signal
        unsigned int sym_out;   // output symbol
        float complex d_prime;  // estimated input sample
        modem_demodulate(demod, d_hat, &sym_out);
        modem_get_demodulator_sample(demod, &d_prime);

        // update equalizer
        eqlms_cccf_step(eq, d_prime, d_hat);
    }

    // get equalizer weights
    eqlms_cccf_get_weights(eq, hp);

    // destroy objects
    eqlms_cccf_destroy(eq);
    interp_crcf_destroy(interp);
    firfilt_cccf_destroy(fchannel);
    modem_destroy(mod);
    modem_destroy(demod);

    // 
    // export output
    //
    FILE * fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all\n");
    fprintf(fid,"close all\n");

    fprintf(fid,"k = %u;\n", k);
    fprintf(fid,"m = %u;\n", m);
    fprintf(fid,"num_symbols = %u;\n", num_symbols);
    fprintf(fid,"num_samples = num_symbols*k;\n");

    // save transmit matched-filter response
    fprintf(fid,"hm_len = 2*k*m+1;\n");
    fprintf(fid,"hm = zeros(1,hm_len);\n");
    for (i=0; i<hm_len; i++)
        fprintf(fid,"hm(%4u) = %12.4e;\n", i+1, hm[i]);

    // save channel impulse response
    fprintf(fid,"hc_len = %u;\n", hc_len);
    fprintf(fid,"hc = zeros(1,hc_len);\n");
    for (i=0; i<hc_len; i++)
        fprintf(fid,"hc(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(hc[i]), cimagf(hc[i]));

    // save equalizer response
    fprintf(fid,"hp_len = %u;\n", hp_len);
    fprintf(fid,"hp = zeros(1,hp_len);\n");
    for (i=0; i<hp_len; i++)
        fprintf(fid,"hp(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(hp[i]), cimagf(hp[i]));

    // save sample sets
    fprintf(fid,"x = zeros(1,num_samples);\n");
    fprintf(fid,"y = zeros(1,num_samples);\n");
    fprintf(fid,"z = zeros(1,num_samples);\n");
    for (i=0; i<num_samples; i++) {
        fprintf(fid,"x(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(x[i]), cimagf(x[i]));
        fprintf(fid,"y(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(y[i]), cimagf(y[i]));
        fprintf(fid,"z(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(z[i]), cimagf(z[i]));
    }

    // plot time response
    fprintf(fid,"t = 0:(num_samples-1);\n");
    fprintf(fid,"tsym = 1:k:num_samples;\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(t,real(z),...\n");
    fprintf(fid,"     t(tsym),real(z(tsym)),'x');\n");

    // plot constellation
    fprintf(fid,"tsym0 = tsym(1:(length(tsym)/2));\n");
    fprintf(fid,"tsym1 = tsym((length(tsym)/2):end);\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(real(z(tsym0)),imag(z(tsym0)),'x','Color',[1 1 1]*0.7,...\n");
    fprintf(fid,"     real(z(tsym1)),imag(z(tsym1)),'x','Color',[1 1 1]*0.0);\n");
    fprintf(fid,"xlabel('In-Phase');\n");
    fprintf(fid,"ylabel('Quadrature');\n");
    fprintf(fid,"axis([-1 1 -1 1]*1.5);\n");
    fprintf(fid,"axis square;\n");
    fprintf(fid,"grid on;\n");

    // plot responses
    fprintf(fid,"nfft = 1024;\n");
    fprintf(fid,"f = [0:(nfft-1)]/nfft - 0.5;\n");
    fprintf(fid,"Hm = 20*log10(abs(fftshift(fft(hm/k,nfft))));\n");
    fprintf(fid,"Hc = 20*log10(abs(fftshift(fft(hc,nfft))));\n");
    fprintf(fid,"Hp = 20*log10(abs(fftshift(fft(hp,nfft))));\n");
    fprintf(fid,"G  = Hm + Hc + Hp;\n");

#if 0
    // compute aliased spectrum
    fprintf(fid,"g  = real(conv(conv(hm,hc),hp));\n");
    fprintf(fid,"G2 = 20*log10(abs(fftshift(fft(g(1:2:end),nfft))));\n");
    fprintf(fid,"plot(f/2,G2);\n");
#endif

    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(f,Hm, f,Hc, f,Hp, f,G,'-k','LineWidth',2, [-0.5/k 0.5/k],[-6.026 -6.026],'or');\n");
    fprintf(fid,"xlabel('Normalized Frequency');\n");
    fprintf(fid,"ylabel('Power Spectral Density');\n");
    fprintf(fid,"legend('transmit','channel','equalizer','composite','half-power points',1);\n");
    fprintf(fid,"axis([-0.5 0.5 -12 8]);\n");
    fprintf(fid,"grid on;\n");
    
    fclose(fid);
    printf("results written to '%s'\n", OUTPUT_FILENAME);

    return 0;
}
