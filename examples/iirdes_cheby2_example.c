//
//
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.h"

#define OUTPUT_FILENAME "iirdes_cheby2_example.m"

int main() {
    // options
    unsigned int n=5;   // filter order
    float fc = 0.25f;   // cutoff
    float slsl = 40.0f; // stopband attenuation [dB]

    // epsilon
    float epsilon = powf(10.0f, -slsl/20.0f);

    unsigned int i;
    float b[n+1];       // numerator
    float a[n+1];       // denominator

#if 1
    unsigned int r = n%2;
    unsigned int L = (n-r)/2;

    // complex analog roots
    float complex za[2*L];
    float complex pa[n];
    float complex ka;
    cheby2_azpkf(n,fc,epsilon,za,pa,&ka);
    for (i=0; i<n; i++)
        printf("  pa[%3u] = %12.8f + j*%12.8f\n", i, crealf(pa[i]), cimagf(pa[i]));
    printf("\n");
    for (i=0; i<2*L; i++)
        printf("  za[%3u] = %12.8f + j*%12.8f\n", i, crealf(za[i]), cimagf(za[i]));

    // 
    float complex bc[n+1];
    float complex ac[n+1];
    bilinear_zpk(za,2*L,
                 pa,n,
                 ka,
                 1.0f / tanf(M_PI * fc),
                 bc, ac);

    // real coefficients
    for (i=0; i<=n; i++) {
        b[i] = crealf(bc[i]);
        a[i] = crealf(ac[i]);
    }
#else
    cheby2f(n,fc,epsilon,b,a);
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

