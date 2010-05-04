//
// firdespm_example.c
//
// This example demonstrates finite impulse response filter design
// using the Parks-McClellan algorithm.
// SEE ALSO: firdes_kaiser_example.c
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.h"

#define OUTPUT_FILENAME "firdespm_example.m"

int main() {
    // options
    unsigned int n=25;  // filter length
    float fp = 0.08f;   // pass-band cutoff frequency
    float fs = 0.16f;   // stop-band cutoff frequency
    liquid_firdespm_btype btype = LIQUID_FIRDESPM_BANDPASS;

    // derived values
    unsigned int num_bands = 2;
    float bands[4]   = {0.0f, fp, fs, 0.5f};
    float des[2]     = {1.0f, 0.0f};
    float weights[2] = {1.0f, 1.0f};

    unsigned int i;
    float h[n];
#if 0
    firdespm q = firdespm_create(n,bands,des,weights,num_bands,btype);
    firdespm_print(q);
    firdespm_execute(q,h);
    firdespm_destroy(q);
#else
    firdespm_run(n,bands,des,weights,num_bands,btype,h);
#endif

    for (i=0; i<n; i++)
        printf("%20.12f\n", h[i]);

    // open output file
    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n\n");
    fprintf(fid,"h_len=%u;\n", n);

    for (i=0; i<n; i++)
        fprintf(fid,"h(%4u) = %20.8e;\n", i+1, h[i]);

    fprintf(fid,"nfft = 1024;\n");
    fprintf(fid,"f = [0:(nfft-1)]/nfft - 0.5;\n");
    fprintf(fid,"H = 20*log10(abs(fftshift(fft(h,nfft))));\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(f,H);\n");
    fprintf(fid,"xlabel('Normalized Frequency');\n");
    fprintf(fid,"ylabel('PSD [dB]');\n");
    fprintf(fid,"grid on;\n");

    fclose(fid);
    printf("results written to %s.\n", OUTPUT_FILENAME);

    printf("done.\n");
    return 0;
}

