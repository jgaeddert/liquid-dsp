//
// msourcecf_example.c
//
// This example demonstrates generating multiple signal sources simultaneously
// for testing using the msource (multi-source) family of objects.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.h"

#define OUTPUT_FILENAME "msourcecf_example.m"

int main()
{
    // msource parameters
    int          ms          = LIQUID_MODEM_QPSK;
    unsigned int k           =     4;
    unsigned int m           =     9;
    float        beta        = 0.30f;

    // spectral periodogram options
    unsigned int nfft        =   2400;  // spectral periodogram FFT size
    unsigned int num_samples =  48000;  // number of samples

    unsigned int i;

    // create spectral periodogram
    spgramcf periodogram = spgramcf_create_default(nfft);

    unsigned int buf_len = 1024;
    float complex buf[buf_len];

    // create stream generator
    msourcecf gen = msourcecf_create();
    
    int id;

    // add noise source (wide-band)
    id =
    msourcecf_add_noise(gen, 1.00f);
    msourcecf_set_gain(gen, id, -60.0f);

    // add noise source (narrow-band)
    id =
    msourcecf_add_noise(gen, 0.10f);
    msourcecf_set_frequency(gen, id, 0.4*2*M_PI);
    msourcecf_set_gain(gen, id, -20.0f);

    // add tone
    id =
    msourcecf_add_tone(gen);
    msourcecf_set_frequency(gen, id, -0.4*2*M_PI);
    msourcecf_set_gain(gen, id, -40.0f);

    // add modulated data
    id =
    msourcecf_add_modem(gen,ms,k,m,beta);
    //msourcecf_set_frequency(gen, id, -0.2*2*M_PI);
    //msourcecf_set_gain(gen, id, 0.1f);
    
    // print source generator object
    msourcecf_print(gen);

    unsigned int total_samples = 0;
    while (total_samples < num_samples) {
        // write samples to buffer
        msourcecf_write_samples(gen, buf, buf_len);

        // push resulting sample through periodogram
        spgramcf_write(periodogram, buf, buf_len);
        
        // accumulated samples
        total_samples += buf_len;
    }
    printf("total samples: %u\n", total_samples);

    // compute power spectral density output
    float psd[nfft];
    spgramcf_get_psd(periodogram, psd);

    // destroy objects
    msourcecf_destroy(gen);
    spgramcf_destroy(periodogram);

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
    
    for (i=0; i<nfft; i++)
        fprintf(fid,"H(%6u) = %12.4e;\n", i+1, psd[i]);

    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(f, H, '-', 'LineWidth',1.5);\n");
    fprintf(fid,"xlabel('Normalized Frequency [f/F_s]');\n");
    fprintf(fid,"ylabel('Power Spectral Density [dB]');\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"axis([-0.5 0.5 -80 20]);\n");

    fclose(fid);
    printf("results written to %s.\n", OUTPUT_FILENAME);

    printf("done.\n");
    return 0;
}

