//
// firdespm_callback_example.c
//
// This example demonstrates finite impulse response filter design
// using the Parks-McClellan algorithm with callback function for
// arbitrary response and weighting function.
//
// SEE ALSO: firdes_kaiser_example.c

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "liquid.h"

#define OUTPUT_FILENAME "firdespm_callback_example.m"

// user-defined callback function defining response and weights
int callback(double   _frequency,
             void   * _userdata,
             double * _desired,
             double * _weight)
{
    // inverse sinc
    if (_frequency < 0.30f) {
        double t  = 2.0*M_PI*_frequency + 1e-5;
        *_desired = t / sin(t);
        *_weight  = 4.0f;
    } else {
        *_desired = 0.0f;
        *_weight  = expf(4.0f*_frequency);
    }
    return 0;
}

int main(int argc, char*argv[])
{
    // filter design parameters
    unsigned int h_len = 51;
    liquid_firdespm_btype btype = LIQUID_FIRDESPM_BANDPASS;
    unsigned int num_bands = 2;
    float        bands[4]  = {0.00f, 0.3f,
                              0.35f, 0.5f};

    // design filter
    float h[h_len];
    firdespm q = firdespm_create_callback(h_len,num_bands,bands,btype,callback,NULL);
    firdespm_execute(q,h);
    firdespm_destroy(q);

    // print coefficients
    unsigned int i;
    for (i=0; i<h_len; i++)
        printf("h(%4u) = %16.12f;\n", i+1, h[i]);

    // open output file
    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n\n");
    fprintf(fid,"h_len=%u;\n", h_len);

    for (i=0; i<h_len; i++)
        fprintf(fid,"h(%4u) = %20.8e;\n", i+1, h[i]);

    fprintf(fid,"nfft=1024;\n");
    fprintf(fid,"H=20*log10(abs(fftshift(fft(h,nfft))));\n");
    fprintf(fid,"f=[0:(nfft-1)]/nfft-0.5;\n");
    fprintf(fid,"figure; plot(f,H,'Color',[0 0.5 0.25],'LineWidth',2);\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"xlabel('normalized frequency');\n");
    fprintf(fid,"ylabel('PSD [dB]');\n");
    fprintf(fid,"title('Filter design (firdespm), inverse sinc');\n");
    fprintf(fid,"axis([-0.5 0.5 -60 10]);\n");

    fclose(fid);
    printf("results written to %s.\n", OUTPUT_FILENAME);

    printf("done.\n");
    return 0;
}

