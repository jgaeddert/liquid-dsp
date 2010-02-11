//
//
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.h"

#define OUTPUT_FILENAME "iirdes_cheby1_example.m"

int main() {
    // options
    unsigned int n=4;   // filter order
    float fc = 0.25f;   // cutoff
    float ripple = 1.0f;// passband ripple [dB]

    // epsilon
    float epsilon = sqrtf( powf(10.0f, ripple / 10.0f) - 1.0f );

    unsigned int i;
    float b[n+1];       // numerator
    float a[n+1];       // denominator

#if 1
    // complex analog roots
    float complex za[0];
    float complex pa[n];
    float complex ka;
    cheby1_azpkf(n,fc,epsilon,za,pa,&ka);
    for (i=0; i<n; i++)
        printf("  pa[%3u] = %12.8f + j*%12.8f\n", i, crealf(pa[i]), cimagf(pa[i]));

    // complex digital poles/zeros/gain
    float complex zd[n];
    float complex pd[n];
    float complex kd;
    float m = 1.0f / tanf(M_PI * fc);
    printf("ka : %12.8f + j*%12.8f\n", crealf(ka), cimagf(ka));
    zpk_a2df(za,    0,
             pa,    n,
             ka,    m,
             zd, pd, &kd);

    // convert complex digital poles/zeros/gain into transfer function
    dzpk2tff(zd,pd,n,kd,b,a);
#else
    cheby1f(n,fc,epsilon,b,a);
#endif

    // print coefficients
    for (i=0; i<=n; i++) printf("a(%3u) = %12.8f;\n", i+1, a[i]);
    for (i=0; i<=n; i++) printf("b(%3u) = %12.8f;\n", i+1, b[i]);

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

    printf("done.\n");
    return 0;
}

