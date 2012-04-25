//
// fft_spgram.c
//
// Spectral periodogram example.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.h"

#define OUTPUT_FILENAME_TIME "figures.gen/fft_spgram_time.dat"
#define OUTPUT_FILENAME_FREQ "figures.gen/fft_spgram_freq.dat"

int main() {
    // spectral periodogram options
    unsigned int nfft=256;              // spectral periodogram FFT size
    unsigned int num_samples = 2001;    // number of samples

    // allocate memory for data arrays
    float complex x[num_samples];       // input signal
    float complex X[nfft];              // output spectrum
    float psd[nfft];                    // power spectral density

    unsigned int ramp = num_samples/20 < 10 ? 10 : num_samples/20;

    // create spectral periodogram
    unsigned int window_size = nfft/2;  // spgram window size
    unsigned int delay       = nfft/8;  // samples between transforms
    spgram q = spgram_create(nfft, window_size);

    unsigned int i;

    // generate signal
    nco_crcf nco = nco_crcf_create(LIQUID_VCO);
    for (i=0; i<num_samples; i++) {
        nco_crcf_set_frequency(nco, 0.1f*(1.2f+sinf(0.007f*i)) );
        nco_crcf_cexpf(nco, &x[i]);
        nco_crcf_step(nco);
    }
    nco_crcf_destroy(nco);

    // add soft ramping functions
    for (i=0; i<ramp; i++) {
        x[i]                    *= 0.5f - 0.5f*cosf(M_PI*(float)i          / (float)ramp);
        x[num_samples-ramp+i-1] *= 0.5f - 0.5f*cosf(M_PI*(float)(ramp-i-1) / (float)ramp);
    }

    // 
    // export output file(s)
    //
    FILE * fid;
    
    // 
    // export time-doman data
    //
    fid = fopen(OUTPUT_FILENAME_TIME,"w");
    fprintf(fid,"# %s : auto-generated file\n", OUTPUT_FILENAME_TIME);
    for (i=0; i<num_samples; i++)
        fprintf(fid,"%12u %12.8f %12.8f\n", i, crealf(x[i]), cimagf(x[i]));
    fclose(fid);
    printf("results written to %s.\n", OUTPUT_FILENAME_TIME);
    
    // 
    // export freq-doman data
    //
    fid = fopen(OUTPUT_FILENAME_FREQ,"w");
    fprintf(fid,"# %s : auto-generated file\n", OUTPUT_FILENAME_FREQ);

    unsigned int t=0;
    for (i=0; i<num_samples; i++) {
        // push sample into periodogram
        spgram_push(q, &x[i], 1);

        if ( ((i+1)%delay)==0 ) {
            // compute spectral periodogram output
            spgram_execute(q, X);

            unsigned int k;

            // compute PSD and FFT shift
            for (k=0; k<nfft; k++)
                psd[k] = 20*log10f( cabsf(X[(k+nfft/2)%nfft]) );
#if 1
            for (k=0; k<nfft; k++)
                fprintf(fid,"%12u %12.8f %12.8f\n", t, (float)k/(float)nfft - 0.5f, psd[k]);
#else
            for (k=0; k<nfft; k++)
                fprintf(fid,"%12.8f ", psd[k]);
#endif
            fprintf(fid,"\n");
            t++;
        }
    }

    // destroy spectral periodogram object
    spgram_destroy(q);

    fclose(fid);
    printf("results written to %s.\n", OUTPUT_FILENAME_FREQ);

    printf("done.\n");
    return 0;
}

