// file: freqmodem_test.c
//
// Tests simple modulation/demodulation without noise or phase
// offset
//

#include <stdio.h>
#include <math.h>
#include "liquid.h"

#define OUTPUT_FILENAME "freqmodem_example.m"

int main() {
    // options
    float mod_index = 0.1f;         // modulation index (bandwidth)
    float fc = 0.1371f*2.0f*M_PI;   // FM carrier
    liquid_fmtype type = LIQUID_MODEM_FM_DELAY_CONJ;
    unsigned int num_samples = 256; // number of samples
    float SNRdB = 30.0f;            // signal-to-noise ratio [dB]

    // create mod/demod objects
    freqmodem mod   = freqmodem_create(mod_index,fc,type);
    freqmodem demod = freqmodem_create(mod_index,fc,type);
    freqmodem_print(mod);

    unsigned int i;
    float x[num_samples];
    float complex y[num_samples];
    float z[num_samples];

    // generate un-modulated signal (band-limited pulse)
    liquid_firdes_kaiser(num_samples, 0.05f, -40.0f, 0.0f, x);

    // modulate signal
    for (i=0; i<num_samples; i++)
        freqmodem_modulate(mod, x[i], &y[i]);

    // add noise
    float nstd = powf(10.0f,-SNRdB*0.1f);
    for (i=0; i<num_samples; i++)
        cawgn(&y[i], nstd);

    // demodulate signal
    for (i=0; i<num_samples; i++)
        freqmodem_demodulate(demod, y[i], &z[i]);

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
    // plot results
    fprintf(fid,"t=0:(n-1);\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(t,x,t,z);\n");
    fprintf(fid,"axis([0 n -1.2 1.2]);\n");
    // spectrum
    fprintf(fid,"nfft=1024;\n");
    fprintf(fid,"f=[0:(nfft-1)]/nfft - 0.5;\n");
    fprintf(fid,"Y = 20*log10(abs(fftshift(fft(y/n,nfft))));\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(f,Y);\n");
    fprintf(fid,"axis([-0.5 0.5 -60 20]);\n");
    fprintf(fid,"grid on;\n");
    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);

    freqmodem_destroy(mod);
    freqmodem_destroy(demod);
    return 0;
}
