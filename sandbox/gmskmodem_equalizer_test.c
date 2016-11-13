// 
// gmskmodem_equalizer_test.c
//
// Tests least mean-squares (LMS) equalizer (EQ) on a received GMSK
// signal. The equalizer is updated using decision-directed demodulator
// output samples.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <complex.h>
#include <getopt.h>
#include <time.h>
#include "liquid.h"

#define OUTPUT_FILENAME "gmskmodem_equalizer_test.m"

// print usage/help message
void usage()
{
    printf("Usage: gmskmodem_equalizer_test [OPTION]\n");
    printf("  h     : print help\n");
    printf("  n     : number of symbols, default: 500\n");
    printf("  k     : samples/symbol, default: 2\n");
    printf("  m     : filter semi-length (symbols), default: 4\n");
    printf("  b     : filter excess bandwidth factor, default: 0.3\n");
    printf("  p     : equalizer semi-length (symbols), default: 3\n");
    printf("  u     : equalizer learning rate, default; 0.05\n");
}

int main(int argc, char*argv[])
{
    srand(time(NULL));

    // options
    unsigned int num_symbols = 1200;    // number of symbols to observe
    unsigned int k           = 2;       // matched filter samples/symbol
    unsigned int m           = 3;       // matched filter delay (symbols)
    float        beta        = 0.30f;   // GMSK bandwidth-time factor
    unsigned int p           = 3;       // equalizer length (symbols, hp_len = 2*k*p+1)
    float        mu          = 0.10f;   // learning rate
    unsigned int nfft        = 1200;    // spectrum estimate FFT size

    int dopt;
    while ((dopt = getopt(argc,argv,"hn:k:m:b:p:u:")) != EOF) {
        switch (dopt) {
        case 'h': usage();                    return 0;
        case 'n': num_symbols = atoi(optarg); break;
        case 'k': k           = atoi(optarg); break;
        case 'm': m           = atoi(optarg); break;
        case 'b': beta        = atof(optarg); break;
        case 'p': p           = atoi(optarg); break;
        case 'u': mu          = atof(optarg); break;
        default:
            exit(1);
        }
    }

    // validate input
    if (num_symbols == 0) {
        fprintf(stderr,"error: %s, number of symbols must be greater than zero\n", argv[0]);
        exit(1);
    } else if (k < 2) {
        fprintf(stderr,"error: %s, samples/symbol must be at least 2\n", argv[0]);
        exit(1);
    } else if (m == 0) {
        fprintf(stderr,"error: %s, filter semi-length must be at least 1 symbol\n", argv[0]);
        exit(1);
    } else if (beta < 0.0f || beta > 1.0f) {
        fprintf(stderr,"error: %s, filter excess bandwidth must be in [0,1]\n", argv[0]);
        exit(1);
    } else if (p == 0) {
        fprintf(stderr,"error: %s, equalizer semi-length must be at least 1 symbol\n", argv[0]);
        exit(1);
    } else if (mu < 0.0f || mu > 1.0f) {
        fprintf(stderr,"error: %s, equalizer learning rate must be in [0,1]\n", argv[0]);
        exit(1);
    }

    // derived values
    unsigned int hm_len = 2*k*m+1;   // matched filter length
    unsigned int hp_len = 2*k*p+1;   // equalizer filter length
    unsigned int num_samples = k*num_symbols;

    // bookkeeping variables
    float complex x[num_samples];       // interpolated time series
    float complex y[num_samples];       // equalized output

    float hm[hm_len];                   // matched filter response
    float complex hp[hp_len];           // equalizer filter coefficients

    unsigned int i;

    // generate matched filter response
    liquid_firdes_prototype(LIQUID_FIRFILT_GMSKTX, k, m, beta, 0.0f, hm);

    // create the modem objects
    modem demod = modem_create(LIQUID_MODEM_QPSK);

    gmskmod gmod = gmskmod_create(k,m,beta);
    for (i=0; i<num_symbols; i++)
        gmskmod_modulate(gmod, rand()%2, &x[i*k]);
    
    // push through equalizer
    // create equalizer, intialized with square-root Nyquist filter
    eqlms_cccf eq = eqlms_cccf_create_rnyquist(LIQUID_FIRFILT_RRC, k, p, beta, 0.0f);
    eqlms_cccf_set_bw(eq, mu);

    // get initialized weights
    eqlms_cccf_get_weights(eq, hp);

    // filtered error vector magnitude (emperical RMS error)
    float evm_hat = 0.03f;

    float complex d_hat = 0.0f;
    for (i=0; i<num_samples; i++) {
        // print filtered evm (emperical rms error)
        if ( ((i+1)%50)==0 )
            printf("%4u : rms error = %12.8f dB\n", i+1, 10*log10(evm_hat));

        eqlms_cccf_push(eq, x[i]);
        eqlms_cccf_execute(eq, &d_hat);

        // store output
        y[i] = d_hat;

        // decimate by k
        if ( (i%k) != 0 ) continue;

        // estimate transmitted signal
        unsigned int sym_out;   // output symbol
        float complex d_prime;  // estimated input sample
        modem_demodulate(demod, d_hat, &sym_out);
        modem_get_demodulator_sample(demod, &d_prime);

        // update equalizer
        eqlms_cccf_step(eq, d_prime, d_hat);

        // update filtered evm estimate
        float evm = crealf( (d_prime-d_hat)*conjf(d_prime-d_hat) );
        evm_hat = 0.98f*evm_hat + 0.02f*evm;
    }

    // get equalizer weights
    eqlms_cccf_get_weights(eq, hp);

    //
    // run many trials get get average spectrum
    //
    spgramcf periodogram_tx = spgramcf_create_default(nfft);
    spgramcf periodogram_rx = spgramcf_create_default(nfft);

    firfilt_cccf mf = firfilt_cccf_create(hp, hp_len);

    float complex buf_tx[k];
    float complex buf_rx[k];
    for (i=0; i<500e3; i++) {
        // generate random symbol
        gmskmod_modulate(gmod, rand()%2, buf_tx);

        // run matched filter on result
        firfilt_cccf_execute_block(mf, buf_tx, k, buf_rx);

        // accumulate spectrum average
        spgramcf_write(periodogram_tx, buf_tx, k);
        spgramcf_write(periodogram_rx, buf_rx, k);
    }
    firfilt_cccf_destroy(mf);

    // write accumulated output PSD
    float X[nfft];
    float Y[nfft];
    spgramcf_get_psd(periodogram_tx, X);
    spgramcf_get_psd(periodogram_rx, Y);

    // destroy periodogram objects
    spgramcf_destroy(periodogram_tx);
    spgramcf_destroy(periodogram_rx);


    // destroy objects
    eqlms_cccf_destroy(eq);
    modem_destroy(demod);
    gmskmod_destroy(gmod);

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

    // save equalizer response
    fprintf(fid,"hp_len = %u;\n", hp_len);
    fprintf(fid,"hp = zeros(1,hp_len);\n");
    for (i=0; i<hp_len; i++)
        fprintf(fid,"hp(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(hp[i]), cimagf(hp[i]));

    // save input/output spectrum
    fprintf(fid,"nfft = %u;\n", nfft);
    fprintf(fid,"X = zeros(1,nfft);\n");
    fprintf(fid,"Y = zeros(1,nfft);\n");
    for (i=0; i<nfft; i++) {
        fprintf(fid,"X(%4u) = %12.4e;\n", i+1, X[i]);
        fprintf(fid,"Y(%4u) = %12.4e;\n", i+1, Y[i]);
    }

    // save sample sets
    fprintf(fid,"x = zeros(1,num_samples);\n");
    fprintf(fid,"y = zeros(1,num_samples);\n");
    for (i=0; i<num_samples; i++) {
        fprintf(fid,"x(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(x[i]), cimagf(x[i]));
        fprintf(fid,"y(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(y[i]), cimagf(y[i]));
    }

    // plot time response
    fprintf(fid,"t = 0:(num_samples-1);\n");
    fprintf(fid,"tsym = 1:k:num_samples;\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(t,real(y),...\n");
    fprintf(fid,"     t(tsym),real(y(tsym)),'x');\n");

    // plot constellation
    fprintf(fid,"tsym0 = tsym(1:(length(tsym)/2));\n");
    fprintf(fid,"tsym1 = tsym((length(tsym)/2):end);\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(real(y(tsym1)),imag(y(tsym1)),'x','Color',[1 1 1]*0.0);\n");
    fprintf(fid,"xlabel('In-Phase');\n");
    fprintf(fid,"ylabel('Quadrature');\n");
    fprintf(fid,"axis([-1 1 -1 1]*1.5);\n");
    fprintf(fid,"axis square;\n");
    fprintf(fid,"grid on;\n");

    // plot responses
    //fprintf(fid,"nfft = 1024;\n");
    fprintf(fid,"f = [0:(nfft-1)]/nfft - 0.5;\n");
    fprintf(fid,"X  = X - 20*log10(k);\n");
    fprintf(fid,"Y  = Y - 20*log10(k);\n");
    fprintf(fid,"Hp = 20*log10(abs(fftshift(fft(hp,nfft))));\n");

    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(f,X, f,Hp, f,Y, '-k','LineWidth',2, [-0.5/k 0.5/k],[-6.026 -6.026],'or');\n");
    fprintf(fid,"xlabel('Normalized Frequency');\n");
    fprintf(fid,"ylabel('Power Spectral Density');\n");
    fprintf(fid,"legend('transmit','equalizer','composite','half-power points','location','south');\n");
    //fprintf(fid,"axis([-0.5 0.5 -12 8]);\n");
    fprintf(fid,"axis([-0.5 0.5 -50 10]);\n");
    fprintf(fid,"grid on;\n");
    
    fclose(fid);
    printf("results written to '%s'\n", OUTPUT_FILENAME);

    return 0;
}
