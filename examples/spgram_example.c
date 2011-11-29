//
// spgram_example.c
//
// Spectral periodogram example.
//

#include <unistd.h> // usleep
#include <stdio.h>
#include <math.h>

#include "liquid.h"

#define OUTPUT_FILENAME "spgram_example.m"

int main() {
    // spectral periodogram options
    unsigned int n=140;             // spectral periodogram FFT size

    // OFDM options
    unsigned int nfft=64;           // OFDM FFT size
    unsigned int num_symbols = 1;   // number of OFDM symbols

    // initialize objects
    float complex X[nfft];  // OFDM symbol (freq)
    float complex x[nfft];  // OFDM symbol (time)
    float psd[n];           // power spectral density

    // create spectral periodogram
    spgram q = spgram_create(nfft);

    // guard bands
    unsigned int g0 = (nfft/2) - (nfft/10);
    unsigned int g1 = (nfft/2) + (nfft/10);

    // NULL bands
    unsigned int n0 = (nfft/4) - (nfft/16);
    unsigned int n1 = (nfft/4) + (nfft/16);

    unsigned int i;
    for (i=0; i<num_symbols; i++) {
        // generate data
        unsigned int j;
        for (j=0; j<nfft; j++) {
            if (j==0 || (j<g0 && j>g1)) {
                // NULL subcarrier
                X[j] = 0.0f;
            } else {
                // DATA subcarrier (use noise)
                X[j] = (randnf() + _Complex_I*randnf()) * 0.707f;
            }
        }

        // run transform and push into spectral periodogram object
        fft_run(nfft, X, x, FFT_REVERSE, 0);
        spgram_push(q, x, nfft);

#if 0
        for (j=0; j<nfft; j++)
            printf("x[%3u] = %12.8f + j*%12.8f\n", j, crealf(x[j]), cimagf(x[j]));
#endif
    }

    // 'execute' spectral periodogram
    spgram_execute(q, psd);

    // destroy spectral periodogram object
    spgram_destroy(q);

    // 
    // export output file
    //
    FILE * fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n\n");
    fprintf(fid,"n = %u;\n", n);
    fprintf(fid,"psd = zeros(1,n);\n");

    for (i=0; i<n; i++) {
        fprintf(fid,"psd(%4u) = %12.4e;\n", i+1, psd[i]);
    }

    // print
    fprintf(fid,"f = [0:(n-1)]/n - 0.5;\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(f, psd);\n");
    fprintf(fid,"xlabel('frequency');\n");
    fprintf(fid,"ylabel('PSD [dB]');\n");
    fprintf(fid,"axis([-0.5 0.5 -120 40]);\n");
    fprintf(fid,"axis square;\n");

    fclose(fid);
    printf("results written to %s.\n", OUTPUT_FILENAME);

    printf("done.\n");
    return 0;
}

