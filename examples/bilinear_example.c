//
// bilinear_example.c
//
// This file demonstrates the interface for the zero-pole-gain
// bilinear z-transform to design a Butterworth filter.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>

#include "liquid.h"

#define OUTPUT_FILENAME "bilinear_example.m"

int main() {
    // options
    unsigned int np=5;      // num poles (filter order)
    unsigned int nz=0;      // num zeros (nz <= np)

    unsigned int i;
    float complex p[np];    // poles array
    float complex z[nz];    // zeros array

    // compute butterworth roots (poles)
    butter_rootsf(np,p);

    // add zeros (if available)
    for (i=0; i<nz; i++) z[i] = 1.0f;

    float complex k = 1.0f;     // scaling factor
    float m = 1.0f;             // bilateral warping factor (filter bandwidth)
    float complex b[np+1];      // output numerator
    float complex a[np+1];      // output denominator

    // compute bilinear z-transform on continuous time
    // transfer function
    bilinear_zpk(z, nz, // zeros
                 p, np, // poles
                 k, m,  // scaling/warping factors
                 b, a); // output

    // print results
    printf("poles (%u):\n",np);
    for (i=0; i<np; i++)
        printf("  p[%3u] = %12.8f + j*%12.8f\n", i, crealf(p[i]), cimagf(p[i]));
    printf("\n");

    printf("zeros (%u):\n",nz);
    for (i=0; i<nz; i++)
        printf("  z[%3u] = %12.8f + j*%12.8f\n", i, crealf(z[i]), cimagf(z[i]));
    printf("\n");

    printf("numerator:\n");
    for (i=0; i<=np; i++)
        printf("  b[%3u] = %12.8f +j*%12.8f\n", i, crealf(b[i]), cimagf(b[i]));
    printf("\n");

    printf("denominator:\n");
    for (i=0; i<=np; i++)
        printf("  a[%3u] = %12.8f +j*%12.8f\n", i, crealf(a[i]), cimagf(a[i]));

    // open output file
    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"\nclear all;\nclose all;\n\n");
    fprintf(fid,"n=%u;\n", np);
    fprintf(fid,"a = zeros(1,n+1);\n");
    fprintf(fid,"b = zeros(1,n+1);\n");
    for (i=0; i<=np; i++) {
        fprintf(fid,"a(%3u) = %12.4e;\n", i+1, crealf(a[i]));
        fprintf(fid,"b(%3u) = %12.4e;\n", i+1, crealf(b[i]));
    }
    fprintf(fid,"\n");
    fprintf(fid,"freqz(b,a);\n");
    fclose(fid);
    printf("results written to %s.\n", OUTPUT_FILENAME);

    printf("done.\n");
    return 0;
}

