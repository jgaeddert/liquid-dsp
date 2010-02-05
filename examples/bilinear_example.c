//
//
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>

#include "liquid.h"

#define OUTPUT_FILENAME "bilinear_example.m"

int main() {
    // options
    unsigned int nz=0;  // num zeros
    unsigned int np=5;  // num poles

    float complex z[nz];
    float complex p[np];

    // compute butterworth roots (poles)
    butter_rootsf(np,p);

    float complex k = 1.0f; // scaling factor
    float m = 1.0f;         // bilateral warping factor
    float complex b[np+1];  // output numerator TODO figure out length
    float complex a[np+1];  // output denominator
    bilinear_zpk(z, nz,
                 p, np,
                 k, m,
                 b, a);

    // print coefficients
    unsigned int i;
    for (i=0; i<=np; i++)
        printf("a(%3u) = %12.8f +j*%12.8f\n", i+1, crealf(a[i]), cimagf(a[i]));

    printf("\n");

    for (i=0; i<=np; i++)
        printf("b(%3u) = %12.8f +j*%12.8f\n", i+1, crealf(b[i]), cimagf(b[i]));


#if 0
    // open output file
    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"\nclear all;\nclose all;\n\n");
    fprintf(fid,"n=%u;\n", n);
    fprintf(fid,"a = zeros(1,n+1);\n");
    fprintf(fid,"b = zeros(1,n+1);\n");
    for (i=0; i<=n; i++) {
        fprintf(fid,"a(%3u) = %12.4e;\n", i+1, a[i]);
        fprintf(fid,"b(%3u) = %12.4e;\n", i+1, b[i]);
    }
    fprintf(fid,"\n");
    fprintf(fid,"freqz(b,a);\n");
    fclose(fid);
    printf("results written to %s.\n", OUTPUT_FILENAME);
#endif

    printf("done.\n");
    return 0;
}

