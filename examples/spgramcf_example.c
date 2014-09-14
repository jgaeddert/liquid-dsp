//
// spgramcf_example.c
//
// Spectral periodogram example with complex inputs.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.h"

#define OUTPUT_FILENAME "spgramcf_example.m"

int main() {
    // spectral periodogram options
    unsigned int nfft        =   1024;  // spectral periodogram FFT size
    unsigned int num_samples =   4000;  // number of samples
    float        beta        =  10.0f;  // Kaiser-Bessel window parameter
    float        noise_floor = -60.0f;  // noise floor [dB]
    float        alpha       =   0.1f;  // PSD estimate bandwidth


    unsigned int i;

    // derived values
    float nstd = powf(10.0f, noise_floor/20.0f);

    // create spectral periodogram
    unsigned int window_size = nfft/2;  // spgramcf window size
    spgramcf q = spgramcf_create_kaiser(nfft, window_size, beta);

    // generate signal (filter with frequency offset)
    unsigned int  h_len = 91;       // filter length
    float         fc    = 0.07f;    // filter cut-off frequency
    float         f0    = 0.20f;    // filter center frequency
    float         As    = 60.0f;    // filter stop-band attenuation
    float         h[h_len];         // filter coefficients (prototype)
    float complex hc[h_len];        // filter coefficients (complex, offset)
    liquid_firdes_kaiser(h_len, fc, As, 0, h);
    // convert to complex coefficients with frequency offset
    for (i=0; i<h_len; i++)
        hc[i] = h[i] * cexpf(_Complex_I*2*M_PI*f0*i);
    firfilt_cccf filter = firfilt_cccf_create(hc, h_len);
    firfilt_cccf_set_scale(filter, fc);

    for (i=0; i<num_samples; i++) {
        // generate random sample
        float complex x = randnf() + _Complex_I*randnf();

        // filter
        float complex y = 0;
        firfilt_cccf_push(filter, x);
        firfilt_cccf_execute(filter, &y);

        // add noise
        y += nstd * ( randnf() + _Complex_I*randnf() ) * M_SQRT1_2;

        // push resulting sample through periodogram
        spgramcf_accumulate_psd(q, &y, alpha, 1);
    }

    // compute power spectral density output
    float psd[nfft];
    spgramcf_write_accumulation(q, psd);

    // destroy objects
    firfilt_cccf_destroy(filter);
    spgramcf_destroy(q);

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

