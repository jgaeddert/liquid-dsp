// file: freqmodemq16_test.c
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

#define OUTPUT_FILENAME "freqmodemq16_example.m"

// print usage/help message
void usage()
{
    printf("%s [options]\n", __FILE__);
    printf("  h     : print usage\n");
    printf("  n     : number of samples, default: 1024\n");
    printf("  S     : SNR [dB], default: 30\n");
    printf("  k     : FM modulation factor, default: 0.1\n");
}

int main(int argc, char*argv[])
{
    // options
    float        kf          = 0.1f;    // modulation factor
    unsigned int num_samples = 1024;    // number of samples
    float        SNRdB       = 30.0f;   // signal-to-noise ratio [dB]

    int dopt;
    while ((dopt = getopt(argc,argv,"hn:S:k:")) != EOF) {
        switch (dopt) {
        case 'h':   usage();                    return 0;
        case 'n':   num_samples = atoi(optarg); break;
        case 'S':   SNRdB       = atof(optarg); break;
        case 'k':   kf          = atof(optarg); break;
        default:
            exit(1);
        }
    }

    // create mod/demod objects
    freqmodq16 mod = freqmodq16_create(kf);   // modulator
    freqdemq16 dem = freqdemq16_create(kf);   // demodulator
    freqmodq16_print(mod);

    unsigned int i;
    q16_t  m[num_samples];      // message signal
    cq16_t r[num_samples];      // received signal (complex baseband)
    q16_t  y[num_samples];      // demodulator output

    // generate message signal (sum of sines)
    for (i=0; i<num_samples; i++) {
        m[i] = q16_float_to_fixed(
                0.3f*cosf(2*M_PI*0.013f*i + 0.0f) +
                0.2f*cosf(2*M_PI*0.021f*i + 0.4f) +
                0.4f*cosf(2*M_PI*0.037f*i + 1.7f)
               );
    }

    // modulate signal
    freqmodq16_modulate_block(mod, m, num_samples, r);

    // add channel impairments
    float nstd = powf(10.0f,-SNRdB/20.0f);
    for (i=0; i<num_samples; i++) {
        r[i].real += q16_float_to_fixed( nstd * randnf() * M_SQRT1_2 );
        r[i].imag += q16_float_to_fixed( nstd * randnf() * M_SQRT1_2 );
    }

    // demodulate signal
    freqdemq16_demodulate_block(dem, r, num_samples, y);

    // destroy modem objects
    freqmodq16_destroy(mod);
    freqdemq16_destroy(dem);

    // compute RMS error (ignore first sample)
    float rmse = 0.0;
    for (i=1; i<num_samples; i++) {
        float err = q16_fixed_to_float(y[i] - m[i]);
        rmse += err*err;
    }
    rmse = sqrtf( rmse / (float)(num_samples-1) );
    printf("rmse = %12.4e\n", rmse);

    // 
    // write results to output file
    //
    FILE * fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all\n");
    fprintf(fid,"close all\n");
    fprintf(fid,"n=%u;\n",num_samples);
    for (i=0; i<num_samples; i++) {
        fprintf(fid,"m(%3u) = %12.4e;\n", i+1, q16_fixed_to_float(m[i]));
        fprintf(fid,"r(%3u) = %12.4e + j*%12.4e;\n",
                i+1,
                q16_fixed_to_float(r[i].real),
                q16_fixed_to_float(r[i].imag));
        fprintf(fid,"y(%3u) = %12.4e;\n", i+1, q16_fixed_to_float(y[i]));
    }
    // plot time-domain result
    fprintf(fid,"t=0:(n-1);\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(3,1,1);\n");
    fprintf(fid,"  plot(t,m,'LineWidth',1,'Color',[0 0.2 0.5],...);\n");
    fprintf(fid,"       t,y,'LineWidth',1,'Color',[0 0.5 0.2]);\n");
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
    fprintf(fid,"  plot(f,M,'LineWidth',1.2,'Color',[0 0.2 0.5],...\n");
    fprintf(fid,"       f,Y,'LineWidth',1.2,'Color',[0 0.5 0.2]);\n");
    fprintf(fid,"  axis([-0.5 0.5 -80 20]);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  xlabel('Normalized Frequency [f/F_s]');\n");
    fprintf(fid,"  ylabel('Audio PSD [dB]');\n");
    // plot spectral response (RF)
    fprintf(fid,"subplot(3,1,3);\n");
    fprintf(fid,"  plot(f,R,'LineWidth',1.2,'Color',[0.5 0 0]);\n");
    fprintf(fid,"  axis([-0.5 0.5 -80 20]);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  xlabel('Normalized Frequency [f/F_s]');\n");
    fprintf(fid,"  ylabel('RF PSD [dB]');\n");
    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);

    return 0;
}
