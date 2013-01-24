//
// spgram_example.c
//
// Spectral periodogram example.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.h"

#define OUTPUT_FILENAME "spgram_example.m"

int main() {
    // spectral periodogram options
    unsigned int nfft=256;              // spectral periodogram FFT size
    unsigned int num_samples = 2001;    // number of samples
    float beta = 10.0f;                 // Kaiser-Bessel window parameter

    // allocate memory for data arrays
    float complex x[num_samples];       // input signal
    float complex X[nfft];              // output spectrum
    float psd[nfft];                    // power spectral density

    // create spectral periodogram
    unsigned int window_size = nfft/2;  // spgram window size
    unsigned int delay       = nfft/8;  // samples between transforms
    spgram q = spgram_create_kaiser(nfft, window_size, beta);

    unsigned int i;

    // generate signal
    nco_crcf nco = nco_crcf_create(LIQUID_VCO);
    for (i=0; i<num_samples; i++) {
        nco_crcf_set_frequency(nco, 0.1f*(1.2f+sinf(0.007f*i)) );
        nco_crcf_cexpf(nco, &x[i]);
        nco_crcf_step(nco);
    }
    nco_crcf_destroy(nco);

    // 
    // export output file
    //
    FILE * fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n\n");
    fprintf(fid,"nfft = %u;\n", nfft);
    fprintf(fid,"H = zeros(1,nfft);\n");

    unsigned int timer = nfft;
    unsigned int num_transforms = 0;
    for (i=0; i<num_samples; i++) {
        // push single sample into periodogram
        spgram_push(q, &x[i], 1);

        // update counters
        timer--;

        if ( timer == 0 ) {
            // reset timer
            num_transforms++;
            timer = delay;

            // compute spectral periodogram output
            spgram_execute(q, X);

            unsigned int k;

            // compute PSD and FFT shift
            for (k=0; k<nfft; k++)
                psd[k] = 20*log10f( cabsf(X[(k+nfft/2)%nfft]) );

            // save results to file
            for (k=0; k<nfft; k++)
                fprintf(fid,"H(%3u,%3u) = %12.8f;\n", num_transforms, k+1, psd[k]);
        }
    }

    // destroy spectral periodogram object
    spgram_destroy(q);

    printf("computed %u transforms\n", num_transforms);

    // print
    fprintf(fid,"colors = [1 1 1; 0.25 0.78 0.50; 0 0.25 0.50; 1 1 1];\n");
    //fprintf(fid,"C = generate_colormap(colors);\n");
    fprintf(fid,"H = H - min(min(H));\n");
    fprintf(fid,"H = H / max(max(H));\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"image(40*H);\n");
    //fprintf(fid,"colormap(C);\n");
    fprintf(fid,"xlabel('freq');\n");
    fprintf(fid,"ylabel('time');\n");

    fclose(fid);
    printf("results written to %s.\n", OUTPUT_FILENAME);

    printf("done.\n");
    return 0;
}

