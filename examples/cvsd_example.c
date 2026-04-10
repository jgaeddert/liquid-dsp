const char __docstr__[] =
"Continuously-variable slope delta example, sinusoidal input."
" This example demonstrates the CVSD audio encoder interface, and"
" its response to a sinusoidal input.  The output distortion"
" ratio is computed, and the time-domain results are written to"
" a file.";

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char*argv[])
{
    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*, filename, "cvsd_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(unsigned, n,     512,  'n', "number of samples", NULL);
    liquid_argparse_add(float,    fc,    0.02, 'f', "input signal frequency", NULL);
    liquid_argparse_add(unsigned, nbits, 3,    'b', "number of adjacent bits to observe", NULL);
    liquid_argparse_add(float,    zeta,  1.5f, 'z', "slope adjustment multiplier", NULL);
    liquid_argparse_add(float,    alpha, 0.95, 'a', "pre-/post-filter coefficient", NULL);
    liquid_argparse_parse(argc,argv);

    unsigned int i;

    // data arrays
    LIQUID_VLA(float, x, n);             // input time series
    LIQUID_VLA(unsigned char, b, n);     // encoded bit pattern
    LIQUID_VLA(float, y, n);             // reconstructed time series

    // create cvsd codecs
    cvsd cvsd_encoder = cvsd_create(nbits, zeta, alpha);
    cvsd cvsd_decoder = cvsd_create(nbits, zeta, alpha);
    cvsd_print(cvsd_encoder);

    // generate input time series
    for (i=0; i<n; i++)
        x[i] = sinf(2.0f*M_PI*fc*i) * liquid_hamming(i,n);

    // encode time series
    for (i=0; i<n; i++)
        b[i] = cvsd_encode(cvsd_encoder, x[i]);

    // compute reconstructed time series, RMS error
    float rmse=0.0f;
    for (i=0; i<n; i++) {
        y[i] = cvsd_decode(cvsd_decoder, b[i]);

        printf("%1u ", b[i]);
        if ( ((i+1)%32) == 0 )
            printf("\n");

        float e = x[i]-y[i];
        rmse += e*e;
    }

    rmse = sqrtf(rmse/n);

    printf("\n");
    printf("signal/distortion: %8.2f dB\n", -20*log10f(rmse));

    // destroy cvsd objects
    cvsd_destroy(cvsd_encoder);
    cvsd_destroy(cvsd_decoder);

    // 
    // export results to file
    //
    FILE * fid = fopen(filename,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n", filename);
    fprintf(fid,"clear all\n");
    fprintf(fid,"close all\n");

    fprintf(fid,"n=%u;\n", n);
    fprintf(fid,"x=zeros(1,n);\n");
    fprintf(fid,"y=zeros(1,n);\n");

    for (i=0; i<n; i++) {
        fprintf(fid,"x(%3u) = %12.4e;\n", i+1, x[i]);
        fprintf(fid,"y(%3u) = %12.4e;\n", i+1, y[i]);
    }

    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(1:n,x,1:n,y);\n");
    fprintf(fid,"xlabel('time [sample index]');\n");
    fprintf(fid,"ylabel('signal');\n");
    fprintf(fid,"legend('audio input','cvsd output');\n");

    // close debug file
    fclose(fid);
    printf("results written to %s\n", filename);
    printf("done.\n");

    return 0;
}

