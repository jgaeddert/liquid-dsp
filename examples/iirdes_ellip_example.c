//
//
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.h"

#define OUTPUT_FILENAME "iirdes_ellip_example.m"

int main() {
    // options
    unsigned int n=6;   // filter order
    float fc = 0.25f;   // cutoff
    float slsl = 60.0f; // stopband attenuation [dB]
    float ripple = 0.5f;// passband ripple [dB]

    float Gp = powf(10.0f, -ripple  / 20.0f);
    float Gs = powf(10.0f, -slsl    / 20.0f);
    printf("  Gp = %12.8f\n", Gp);
    printf("  Gs = %12.8f\n", Gs);

    // epsilon values (ignored for now)
    float ep = sqrtf(1.0f/(Gp*Gp) - 1.0f);
    float es = sqrtf(1.0f/(Gs*Gs) - 1.0f);

    unsigned int i;
    float b[n+1];       // numerator
    float a[n+1];       // denominator

    unsigned int r = n%2;
    unsigned int L = (n-r)/2;

    // complex analog roots
    float complex za[2*L];
    float complex pa[n];
    float complex ka;
    ellip_azpkf(n,fc,ep,es,za,pa,&ka);
    for (i=0; i<n; i++)
        printf("  pa[%3u] = %12.8f + j*%12.8f\n", i, crealf(pa[i]), cimagf(pa[i]));
    printf("\n");
    for (i=0; i<2*L; i++)
        printf("  za[%3u] = %12.8f + j*%12.8f\n", i, crealf(za[i]), cimagf(za[i]));

    // complex digital poles/zeros/gain
    float complex zd[n];
    float complex pd[n];
    float complex kd;
    float m = 1.0f / tanf(M_PI * fc);
    printf("ka : %12.8f + j*%12.8f\n", crealf(ka), cimagf(ka));
    iirdes_zpka2df(za,    2*L,
                   pa,    n,
                   ka,    m,
                   zd, pd, &kd);

    // convert complex digital poles/zeros/gain into transfer function
    iirdes_dzpk2tff(zd,pd,n,kd,b,a);

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

