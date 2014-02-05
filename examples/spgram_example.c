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
    unsigned int nfft=512;              // spectral periodogram FFT size
    unsigned int num_samples = 4000;    // number of samples
    float beta = 10.0f;                 // Kaiser-Bessel window parameter
    float noise_floor = -60.0f;         // noise floor [dB]

    unsigned int i;

    // derived values
    float nstd = powf(10.0f, noise_floor/20.0f);

    // create spectral periodogram
    unsigned int window_size = nfft/2;  // spgram window size
    spgram q = spgram_create_kaiser(nfft, window_size, beta);

    // generate signal (interpolated symbols with noise)
    unsigned int k = 4;     // interpolation rate
    unsigned int m = 7;     // filter delay (symbols)
    firinterp_crcf interp = firinterp_crcf_create_rnyquist(LIQUID_RNYQUIST_RKAISER, k, m, 0.3f, 0.0f);

    unsigned int n=0;
    float complex x[k]; // interpolator output

    while (n<num_samples) {
        // generate random symbol
        float complex s = ( rand() % 2 ? 0.707f : -0.707f ) +
                          ( rand() % 2 ? 0.707f : -0.707f ) * _Complex_I;

        // interpolate
        firinterp_crcf_execute(interp, s, x);

        // add noise
        for (i=0; i<k; i++)
            x[i] += nstd * ( randnf() + _Complex_I*randnf() ) * M_SQRT1_2;

        n += k;
        
        // push resulting samples through spgram
        spgram_accumulate_psd(q, x, k);
    }

    // compute power spectral density output
    float psd[nfft];
    spgram_write_accumulation(q, psd);

    // destroy objects
    firinterp_crcf_destroy(interp);
    spgram_destroy(q);

    // 
    // export output file
    //
    FILE * fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n\n");
    fprintf(fid,"nfft = %u;\n", nfft);
    fprintf(fid,"f    = [0:(nfft-1)]/nfft - 0.5;\n");
    fprintf(fid,"H    = zeros(1,nfft);\n");
    fprintf(fid,"noise_floor = %12.6f;\n", noise_floor);
    
    for (i=0; i<nfft; i++)
        fprintf(fid,"H(%6u) = %12.4e;\n", i+1, psd[i]);

    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(f, H, '-', 'LineWidth',1.5);\n");
    fprintf(fid,"xlabel('Normalized Frequency [f/F_s]');\n");
    fprintf(fid,"ylabel('Power Spectral Density [dB]');\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"ymin = 10*floor([noise_floor-20]/10);\n");
    fprintf(fid,"ymax = 10*floor([noise_floor+80]/10);\n");
    fprintf(fid,"axis([-0.5 0.5 ymin ymax]);\n");

    fclose(fid);
    printf("results written to %s.\n", OUTPUT_FILENAME);

    printf("done.\n");
    return 0;
}

