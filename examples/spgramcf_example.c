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
    unsigned int num_samples =    2e6;  // number of samples
    float        beta        =  10.0f;  // Kaiser-Bessel window parameter
    float        noise_floor = -60.0f;  // noise floor [dB]

    unsigned int i;

    // derived values
    float nstd = powf(10.0f, noise_floor/20.0f);

    // create spectral periodogram
    spgramcf q = spgramcf_create_default(nfft);

    // generate signal (filter with frequency offset)
    unsigned int  h_len = 91;       // filter length
    float         fc    = 0.07f;    // filter cut-off frequency
    float         f0    = 0.20f;    // filter center frequency
    float         As    = 60.0f;    // filter stop-band attenuation
    firfilt_cccf filter = firfilt_cccf_create_kaiser(h_len, fc, As, 0.0f);
    firfilt_cccf_set_scale(filter, 2*fc);

    for (i=0; i<num_samples; i++) {
        // filter random sample
        float complex y = 0;
        firfilt_cccf_push(filter, (randnf() + _Complex_I*randnf())*M_SQRT1_2);
        firfilt_cccf_execute(filter, &y);

        // add noise
        y += nstd * ( randnf() + _Complex_I*randnf() ) * M_SQRT1_2;

        // push resulting sample through periodogram
        spgramcf_push(q, y);
    }

    // explort to gnuplot
    spgramcf_export_gnuplot(q,"spgramcf_example.gnu");

    // compute power spectral density output
    float psd[nfft];
    spgramcf_get_psd(q, psd);

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
    fprintf(fid,"psd  = zeros(1,nfft);\n");
    fprintf(fid,"noise_floor = %12.6f;\n", noise_floor);
    
    for (i=0; i<nfft; i++)
        fprintf(fid,"psd(%6u) = %12.4e;\n", i+1, psd[i]);

    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(f, psd, '-', 'LineWidth',1.5);\n");
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

