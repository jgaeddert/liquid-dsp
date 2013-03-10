// file: freqmodem_test.c
//
// Tests simple modulation/demodulation without noise or phase
// offset
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <getopt.h>
#include "liquid.h"

#define OUTPUT_FILENAME "freqmodem_example.m"

// print usage/help message
void usage()
{
    printf("freqmodem_example [options]\n");
    printf("  h     : print usage\n");
    printf("  n     : number of samples, default: 1024\n");
    printf("  S     : SNR [dB], default: 30\n");
    printf("  m     : FM modulation index, default: 0.5\n");
    printf("  t     : FM demod. type (delayconj/pll), default: delayconj\n");
}

int main(int argc, char*argv[])
{
    // options
    float kf = 0.2f;                    // modulation factor
    liquid_freqdem_type type = LIQUID_FREQDEM_DELAYCONJ;
    unsigned int num_samples = 1024;    // number of samples
    float SNRdB = 30.0f;                // signal-to-noise ratio [dB]

    int dopt;
    while ((dopt = getopt(argc,argv,"hn:S:m:t:")) != EOF) {
        switch (dopt) {
        case 'h':   usage();                    return 0;
        case 'n':   num_samples = atoi(optarg); break;
        case 'S':   SNRdB       = atof(optarg); break;
        case 'm':   kf          = atof(optarg); break;
        case 't':
            if (strcmp(optarg,"delayconj")==0) {
                type = LIQUID_FREQDEM_DELAYCONJ;
            } else if (strcmp(optarg,"pll")==0) {
                type = LIQUID_FREQDEM_PLL;
            } else {
                fprintf(stderr,"error: %s, invalid FM type: %s\n", argv[0], optarg);
                exit(1);
            }
            break;
        default:
            exit(1);
        }
    }

    // create mod/demod objects
    freqmod mod = freqmod_create(kf);       // modulator
    freqdem dem = freqdem_create(kf,type);  // demodulator
    freqmod_print(mod);

    unsigned int i;
    float x[num_samples];
    float complex y[num_samples];
    float z[num_samples];

#if 0
    // generate un-modulated signal (band-limited pulse)
    liquid_firdes_kaiser(num_samples, 0.02f, -40.0f, 0.0f, x);
#else
    for (i=0; i<num_samples; i++)
        x[i] = cosf(2*M_PI*0.01f*i);
#endif

    // modulate signal
    printf("modulating signal...\n");
    for (i=0; i<num_samples; i++)
        freqmod_modulate(mod, x[i], &y[i]);

    // add channel impairments
    printf("adding channel...\n");
    float nstd = powf(10.0f,-SNRdB/20.0f);
    for (i=0; i<num_samples; i++)
        y[i] += nstd*( randnf() + _Complex_I*randnf() ) * M_SQRT1_2;

    // demodulate signal
    printf("demodulating signal...\n");
    for (i=0; i<num_samples; i++)
        freqdem_demodulate(dem, y[i], &z[i]);

    // write results to output file
    FILE * fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all\n");
    fprintf(fid,"close all\n");
    fprintf(fid,"n=%u;\n",num_samples);
    for (i=0; i<num_samples; i++) {
        fprintf(fid,"x(%3u) = %12.4e;\n", i+1, x[i]);
        fprintf(fid,"y(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(y[i]), cimagf(y[i]));
        fprintf(fid,"z(%3u) = %12.4e;\n", i+1, z[i]);
    }
    // plot time-domain result
    fprintf(fid,"t=0:(n-1);\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(2,1,1);\n");
    fprintf(fid,"  plot(t,x,t,z);\n");
    fprintf(fid,"  axis([0 n -1.2 1.2]);\n");
    fprintf(fid,"  xlabel('Normalized Time [t/T_s]');\n");
    fprintf(fid,"  ylabel('m(t)');\n");
    fprintf(fid,"  grid on;\n");
    // spectrum
    fprintf(fid,"nfft=1024;\n");
    fprintf(fid,"f=[0:(nfft-1)]/nfft - 0.5;\n");
    fprintf(fid,"Y = 20*log10(abs(fftshift(fft(y.*hamming(n)'/n,nfft))));\n");
    fprintf(fid,"subplot(2,1,2);\n");
    fprintf(fid,"  plot(f,Y,'LineWidth',2);\n");
    fprintf(fid,"  axis([-0.5 0.5 -80 20]);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  xlabel('Normalized Frequency [f/F_s]');\n");
    fprintf(fid,"  ylabel('PSD [dB]');\n");
    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);

    freqmod_destroy(mod);
    freqdem_destroy(dem);
    return 0;
}
