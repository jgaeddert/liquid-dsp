// This example demonstrates the most basic functionality of the
// numerically-controlled oscillator (NCO) object.
#include <stdio.h>
#include <math.h>
#include "liquid.h"

#define OUTPUT_FILENAME "nco_crcq16_example.m"

int main()
{
    // options
    int          type        = LIQUID_NCO;      // nco type
    float        fc          = 0.1f*M_SQRT1_2;  // nco tone frequency
    unsigned int num_samples = 240000;          // number of samples to run
    unsigned int nfft        =   4000;          // spectral periodogram FFT size

    // create the NCO object
    nco_crcq16 q = nco_crcq16_create(type);
    nco_crcq16_set_frequency(q, 2*M_PI*fc);
    nco_crcq16_print(q);

    // create spectral periodogram
    spgramcf periodogram = spgramcf_create_default(nfft);
    spgramcf_print(periodogram);

    unsigned int i;
    for (i=0; i<num_samples; i++) {
        cq16_t y;
        nco_crcq16_cexpf(q, &y);
        nco_crcq16_step(q);
        printf("%12.8f %12.8f\n", q16_fixed_to_float(y.real), q16_fixed_to_float(y.imag));
        
        // push resulting sample through periodogram
        spgramcf_push(periodogram, cq16_fixed_to_float(y));
    }

    // compute power spectral density output
    float psd[nfft];
    spgramcf_get_psd(periodogram, psd);

    // destroy objects
    spgramcf_destroy(periodogram);
    nco_crcq16_destroy(q);

    // export output file
    FILE * fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n\n");
    fprintf(fid,"nfft = %u;\n", nfft);
    fprintf(fid,"f    = [0:(nfft-1)]/nfft - 0.5;\n");
    fprintf(fid,"psd  = zeros(1,nfft);\n");
    for (i=0; i<nfft; i++)
        fprintf(fid,"psd(%6u) = %12.4e;\n", i+1, psd[i]);
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(f, psd, '-', 'LineWidth',1.5);\n");
    fprintf(fid,"xlabel('Normalized Frequency [f/F_s]');\n");
    fprintf(fid,"ylabel('Power Spectral Density [dB]');\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"axis([-0.5 0.5 -60 40]);\n");

    fclose(fid);
    printf("results written to %s.\n", OUTPUT_FILENAME);
    return 0;
}
