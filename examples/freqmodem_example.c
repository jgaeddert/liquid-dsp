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
    printf("  k     : FM modulation factor, default: 0.1\n");
    printf("  t     : FM demod. type (delayconj/pll), default: delayconj\n");
}

int main(int argc, char*argv[])
{
    // options
    float kf = 0.1f;                    // modulation factor
    liquid_freqdem_type type = LIQUID_FREQDEM_DELAYCONJ;
    unsigned int num_samples = 1024;    // number of samples
    float SNRdB = 30.0f;                // signal-to-noise ratio [dB]

    int dopt;
    while ((dopt = getopt(argc,argv,"hn:S:k:t:")) != EOF) {
        switch (dopt) {
        case 'h':   usage();                    return 0;
        case 'n':   num_samples = atoi(optarg); break;
        case 'S':   SNRdB       = atof(optarg); break;
        case 'k':   kf          = atof(optarg); break;
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
    float         m[num_samples];   // message signal
    float complex r[num_samples];   // received signal (complex baseband)
    float         y[num_samples];   // demodulator output

    // generate message signal (sum of sines)
    for (i=0; i<num_samples; i++) {
        m[i] = 0.3f*cosf(2*M_PI*0.013f*i + 0.0f) +
               0.2f*cosf(2*M_PI*0.021f*i + 0.4f) +
               0.4f*cosf(2*M_PI*0.037f*i + 1.7f);
    }

    // modulate signal
    for (i=0; i<num_samples; i++)
        freqmod_modulate(mod, m[i], &r[i]);

    // add channel impairments
    float nstd = powf(10.0f,-SNRdB/20.0f);
    for (i=0; i<num_samples; i++)
        r[i] += nstd*( randnf() + _Complex_I*randnf() ) * M_SQRT1_2;

    // demodulate signal
    for (i=0; i<num_samples; i++)
        freqdem_demodulate(dem, r[i], &y[i]);

    // write results to output file
    FILE * fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all\n");
    fprintf(fid,"close all\n");
    fprintf(fid,"n=%u;\n",num_samples);
    for (i=0; i<num_samples; i++) {
        fprintf(fid,"m(%3u) = %12.4e;\n", i+1, m[i]);
        fprintf(fid,"r(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(r[i]), cimagf(r[i]));
        fprintf(fid,"y(%3u) = %12.4e;\n", i+1, y[i]);
    }
    // plot time-domain result
    fprintf(fid,"t=0:(n-1);\n");
    fprintf(fid,"ydelay = 17; %% pre-assessed output delay\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(3,1,1);\n");
    fprintf(fid,"  plot(t,m,'LineWidth',1.2,t-ydelay,y,'LineWidth',1.2);\n");
    fprintf(fid,"  axis([0 n -1.2 1.2]);\n");
    fprintf(fid,"  xlabel('Normalized Time [t/T_s]');\n");
    fprintf(fid,"  ylabel('m(t), y(t)');\n");
    fprintf(fid,"  grid on;\n");
    // compute spectral responses
    fprintf(fid,"nfft=2^(1+nextpow2(n));\n");
    fprintf(fid,"f=[0:(nfft-1)]/nfft - 0.5;\n");
    fprintf(fid,"w = hamming(n)';\n");
    fprintf(fid,"g = 1 / (mean(w) * n);\n");
    fprintf(fid,"M = 20*log10(abs(fftshift(fft(m.*w*g,nfft))));\n");
    fprintf(fid,"R = 20*log10(abs(fftshift(fft(r.*w*g,nfft))));\n");
    fprintf(fid,"Y = 20*log10(abs(fftshift(fft(y.*w*g,nfft))));\n");
    // plot spectral response (audio)
    fprintf(fid,"subplot(3,1,2);\n");
    fprintf(fid,"  plot(f,M,'LineWidth',1.2,f,Y,'LineWidth',1.2);\n");
    fprintf(fid,"  axis([-0.5 0.5 -80 20]);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  xlabel('Normalized Frequency [f/F_s]');\n");
    fprintf(fid,"  ylabel('Audio PSD [dB]');\n");
    // plot spectral response (RF)
    fprintf(fid,"subplot(3,1,3);\n");
    fprintf(fid,"  plot(f,R,'LineWidth',1.2,'Color',[0.5 0.25 0]);\n");
    fprintf(fid,"  axis([-0.5 0.5 -80 20]);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  xlabel('Normalized Frequency [f/F_s]');\n");
    fprintf(fid,"  ylabel('RF PSD [dB]');\n");
    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);

    freqmod_destroy(mod);
    freqdem_destroy(dem);
    return 0;
}
