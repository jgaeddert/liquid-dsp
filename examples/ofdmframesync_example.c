//
//
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <getopt.h>
#include <time.h>

#include "liquid.h"

#define OUTPUT_FILENAME "ofdmframesync_example.m"

void usage()
{
    printf("Usage: ofdmframesync_example [OPTION]\n");
    printf("  h     : print help\n");
    printf("  d     : enable debugging\n");
    printf("  M     : number of subcarriers (must be even), default: 64\n");
    printf("  C     : cyclic prefix length, default: 16\n");
    printf("  T     : taper length, default: 4\n");
    printf("  s     : signal-to-noise ratio [dB], default: 30\n");
    printf("  F     : carrier frequency offset, default: 0.002\n");
    printf("  c     : number of channel filter taps, default: 1\n");
}

static int callback(float complex * _X,
                    unsigned char * _p,
                    unsigned int _M,
                    void * _userdata);

int main(int argc, char*argv[])
{
    srand(time(NULL));

    // options
    unsigned int M           = 64;            // number of subcarriers
    unsigned int cp_len      = 16;            // cyclic prefix length
    unsigned int taper_len   = 4;             // taper length
    unsigned int num_symbols = 8;             // number of data symbols
    unsigned int hc_len      = 1;             // channel filter length
    float hc_std             = 0.10f;         // channel filter standard deviation
    modulation_scheme ms = LIQUID_MODEM_QPSK; // modulation scheme
    float noise_floor        = -30.0f;        // noise floor [dB]
    float SNRdB              = 20.0f;         // signal-to-noise ratio [dB]
    float phi                = 0.0f;          // carrier phase offset
    float dphi               = 0.002f;        // carrier frequency offset
    int debug_enabled        = 0;

    // get options
    int dopt;
    while((dopt = getopt(argc,argv,"hdM:C:T:s:F:c:")) != EOF){
        switch (dopt) {
        case 'h': usage();                      return 0;
        case 'd': debug_enabled = 1;            break;
        case 'M': M         = atoi(optarg);     break;
        case 'C': cp_len    = atoi(optarg);     break;
        case 'T': taper_len = atoi(optarg);     break;
        case 's': SNRdB     = atof(optarg);     break;
        case 'F': dphi      = atof(optarg);     break;
        case 'c': hc_len    = atoi(optarg);     break;
        default:
            exit(1);
        }
    }

    // validate input
    if (hc_len < 1) {
        fprintf(stderr,"error: %s, channel filter must have at least 1 tap\n", argv[0]);
        exit(1);
    }
    
    unsigned int i;

    // derived values
    unsigned int frame_len = M + cp_len;
    unsigned int num_samples = (3+num_symbols)*frame_len;  // data symbols
    float nstd = powf(10.0f, noise_floor/20.0f);
    float gamma = powf(10.0f, (SNRdB + noise_floor)/20.0f);
    printf("gamma : %f\n", gamma);

    // initialize subcarrier allocation
    unsigned char p[M];
    ofdmframe_init_default_sctype(M, p);

    // received symbol buffer
    windowcf wsyms = windowcf_create(10*M);

    // create frame generator
    ofdmframegen fg = ofdmframegen_create(M, cp_len, taper_len, p);
    ofdmframegen_print(fg);

    // create frame synchronizer
    ofdmframesync fs = ofdmframesync_create(M, cp_len, taper_len, p, callback, (void*)wsyms);
    ofdmframesync_print(fs);
    if (debug_enabled)
        ofdmframesync_debug_enable(fs);

    modem mod = modem_create(ms);

    float complex X[M];             // channelized symbols
    float complex y[num_samples];   // output time series

    unsigned int n=0;

    // write first S0 symbol
    ofdmframegen_write_S0a(fg, &y[n]);
    n += frame_len;

    // write second S0 symbol
    ofdmframegen_write_S0b(fg, &y[n]);
    n += frame_len;

    // write S1 symbol
    ofdmframegen_write_S1( fg, &y[n]);
    n += frame_len;

    // modulate data symbols
    unsigned int s;
    for (i=0; i<num_symbols; i++) {

        unsigned int j;
        for (j=0; j<M; j++) {
            s = modem_gen_rand_sym(mod);
            modem_modulate(mod,s,&X[j]);
        }

        ofdmframegen_writesymbol(fg, X, &y[n]);
        n += frame_len;
    }

    // create channel filter (random delay taps)
    float complex hc[hc_len];
    for (i=0; i<hc_len; i++) {
        hc[i] = (i==0) ? 1.0f : hc_std*randnf()*cexpf(_Complex_I*2*M_PI*randf());
        hc[i] *= gamma;
    }
    firfilt_cccf fc = firfilt_cccf_create(hc,hc_len);
    // fill with noise
    for (i=0; i<hc_len; i++)
        firfilt_cccf_push(fc, nstd*(randnf() + _Complex_I*randnf())*M_SQRT1_2);

    // add noise, carrier offset
    for (i=0; i<num_samples; i++) {
        // push through channel filter
        firfilt_cccf_push(fc, y[i]);
        firfilt_cccf_execute(fc, &y[i]);

        // add carrier offset
        y[i] *= cexpf(_Complex_I*(phi + dphi*i));

        // add noise
        y[i] += nstd*(randnf() + _Complex_I*randnf())*M_SQRT1_2;
    }
    firfilt_cccf_destroy(fc);

    // push noise into synchronizer
    unsigned int d=8;
    for (i=0; i<d; i++) {
        float complex z = nstd*(randnf() + _Complex_I*randnf())*M_SQRT1_2;

        ofdmframesync_execute(fs,&z,1);
    }

    // execute synchronizer
    ofdmframesync_execute(fs,y,num_samples);

    // export debugging file
    if (debug_enabled)
        ofdmframesync_debug_print(fs, "ofdmframesync_debug.m");

    // destroy objects
    ofdmframegen_destroy(fg);
    ofdmframesync_destroy(fs);
    modem_destroy(mod);


    // 
    // export output file
    //

    // count subcarrier types
    unsigned int M_data  = 0;
    unsigned int M_pilot = 0;
    unsigned int M_null  = 0;
    ofdmframe_validate_sctype(p, M, &M_null, &M_pilot, &M_data);

    FILE * fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\nclose all;\n\n");
    fprintf(fid,"M           = %u;\n", M);
    fprintf(fid,"M_data      = %u;\n", M_data);
    fprintf(fid,"M_pilot     = %u;\n", M_pilot);
    fprintf(fid,"M_null      = %u;\n", M_null);
    fprintf(fid,"cp_len      = %u;\n", cp_len);
    fprintf(fid,"num_samples = %u;\n", num_samples);
    fprintf(fid,"noise_floor = %f;\n", noise_floor);
    fprintf(fid,"SNRdB = %f;\n", SNRdB);

    // 
    fprintf(fid,"y = zeros(1,num_samples);\n");
    for (i=0; i<num_samples; i++)
        fprintf(fid,"y(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(y[i]), cimagf(y[i]));

    // save channel response
    fprintf(fid,"hc_len = %u;\n", hc_len);
    fprintf(fid,"hc = zeros(1,hc_len);\n");
    for (i=0; i<hc_len; i++)
        fprintf(fid,"hc(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(hc[i]), cimagf(hc[i]));

    fprintf(fid,"\n\n");
    fprintf(fid,"%% compute spectral periodigram\n");
    fprintf(fid,"nfft=256;\n");
    fprintf(fid,"g = sqrt((M_data+M_pilot)/M)/(mean(hamming(nfft))*sqrt(nfft));\n");
    fprintf(fid,"f=[0:(nfft-1)]/nfft - 0.5;\n");
    fprintf(fid,"Hc = 20*log10(abs(fftshift(fft(hc,nfft))));\n");
    fprintf(fid,"Y = zeros(1,nfft);\n");
    fprintf(fid,"v0=1; v1=v0+round(nfft); k=0;\n");
    fprintf(fid,"while v1 <= num_samples,\n");
    fprintf(fid,"    Y += abs(fft(y(v0:[v1-1]).*hamming(v1-v0).',nfft))*g;\n");
    fprintf(fid,"    v0 = v0 + round(nfft/4);\n");
    fprintf(fid,"    v1 = v1 + round(nfft/4);\n");
    fprintf(fid,"    k = k+1;\n");
    fprintf(fid,"end;\n");
    fprintf(fid,"Y  = 20*log10(abs(fftshift(Y/k)));\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(f,Y,f,Hc);\n");
    fprintf(fid,"psd_min = noise_floor - 10;\n");
    fprintf(fid,"psd_max = noise_floor + 10 + max(SNRdB, 0);\n");
    fprintf(fid,"axis([-0.5 0.5 psd_min psd_max]);\n");
    fprintf(fid,"xlabel('Normalized Frequency');\n");
    fprintf(fid,"ylabel('Power Spectral Density [dB]');\n");
    fprintf(fid,"title('Multipath channel response');\n");

    // print results
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"t=0:(num_samples-1);\n");
    fprintf(fid,"plot(t,real(y),t,imag(y));\n");
    fprintf(fid,"xlabel('time');\n");
    fprintf(fid,"ylabel('received signal');\n");

    // save received symbols
    float complex * rc;
    windowcf_read(wsyms, &rc);
    fprintf(fid,"syms = [];\n");
    for (i=0; i<10*M; i++)
        fprintf(fid,"syms(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(real(syms),imag(syms),'o');\n");
    fprintf(fid,"xlabel('real');\n");
    fprintf(fid,"ylabel('imag');\n");
    fprintf(fid,"axis([-1 1 -1 1]*1.6);\n");
    fprintf(fid,"axis square;\n");

    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);

    windowcf_destroy(wsyms);

    printf("done.\n");
    return 0;
}

static int callback(float complex * _X,
                    unsigned char * _p,
                    unsigned int _M,
                    void * _userdata)
{
    printf("**** callback invoked\n");

    windowcf wsyms = (windowcf)_userdata;
    unsigned int i;
    for (i=0; i<_M; i++) {
        if (_p[i] == OFDMFRAME_SCTYPE_DATA)
            windowcf_push(wsyms, _X[i]);
    }

    return 0;
}

