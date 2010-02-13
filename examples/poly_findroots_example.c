// 
// poly_findroots_example.c
//
// test polynomial root-finding algorithm(s)
//

#include <stdio.h>
#include <math.h>

#include "liquid.h"

#define OUTPUT_FILENAME "poly_findroots_example.m"

void besselpoly(unsigned int _n, int * _p);

//  n   m0(n)                   m2(n)                   tr(n)
//  4   -2.32218504000000e+00   1.57017774731388e-01    1.75438094000000
//  5   -2.99066420749534e+00   1.25586431233268e-01    1.76789754200000
//  6   -3.66047349303832e+00   1.04583627408534e-01    1.77689253100000
//  7   -4.33196373609137e+00   8.96656396226647e-02    1.78345867057143e+00
//  8   -4.99584039483034470    0.07824392346702261     1.78322436142857e+00
//
// m[0] ~ -0.668861023825672*n + 0.352940768662957
// m[2] ~ 1 / (1.6013579390149844*n -0.0429146801453954)
//
// Roots r[i] of the (n-1)^th-order Bessel polynomial lie on
// quadratic polynomial (approximately)
//      Im{r[i]} ~ t * 1.778
// where t = i - L - w + 0.5, w = (n-1) % 2, L = (n-w-1)/2
//      Re{r[i]} ~ m0(n) + m1(n)*Im{r[i]}^2
//
//

int main() {
    unsigned int n=5;

    float complex p[n];
    float complex roots[n-1];

    // compute bessel polynomial
    unsigned int i;
    int bp[n];
    besselpoly(n-1,bp);
    for (i=0; i<n; i++)
        p[i] = bp[i];

    cfpoly_findroots(p,n,roots);

    // expand polynomial
    float complex roots_hat[n-1];
    // convert form...
    for (i=0; i<n-1; i++)
        roots_hat[i] = -roots[i];

    float complex p_hat[n];
    cfpoly_expandroots(roots_hat,n-1,p_hat);

    printf("polynomial :\n");
    for (i=0; i<n; i++)
        printf("  p[%3u] = %12.8f + j*%12.8f\n", i, crealf(p[i]), cimagf(p[i]));

    printf("roots :\n");
    for (i=0; i<n-1; i++)
        printf("  r[%3u] = %12.8f + j*%12.8f\n", i, crealf(roots[i]), cimagf(roots[i]));

    printf("poly (expanded roots):\n");
    for (i=0; i<n; i++)
        printf("  p[%3u] = %12.8f + j*%12.8f\n", i, crealf(p_hat[i]), cimagf(p_hat[i]));

    FILE * fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n\n");
    fprintf(fid,"n = %u;\n", n);
    for (i=0; i<n; i++)
        fprintf(fid,"p(%3u) = %12.8f + j*%12.8f;\n", i+1, crealf(p[i]), cimagf(p[i]));
    for (i=0; i<n-1; i++)
        fprintf(fid,"r(%3u) = %12.8f + j*%12.8f;\n", i+1, crealf(roots[i]), cimagf(roots[i]));
    fprintf(fid,"figure;\n");
    fprintf(fid,"t = -0.5:0.01:0.1;\n");
    fprintf(fid,"plot(t,polyval(p,t));\n");
    fprintf(fid,"grid on;\n");

    // fit roots to 2nd-order polynomial
    fprintf(fid,"m = polyfit(imag(r),real(r),2);\n");
    fprintf(fid,"y = [-1:0.01:1]*1.1*max(imag(r));\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(polyval(m,y),y,'-',r,'x');\n");
    fprintf(fid,"grid on;\n");

    fprintf(fid,"n\n");
    fprintf(fid,"m'\n");

    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);
    
    return 0;
}

void besselpoly(unsigned int _n, int * _p)
{
    unsigned int k;
    for (k=0; k<=_n; k++) {
        float t0 = liquid_lngammaf((float)(2*_n-k)+1);
        float t1 = liquid_lngammaf((float)(_n-k)+1);
        float t2 = liquid_lngammaf((float)(k) + 1);
        float t3 = 1<<(_n-k);

        _p[k] = (int) ( expf(t0 - t1 -t2)/t3 );
#if 0
        printf("  p[%3u,%3u] = %d\n", k, _n, _p[k]);
        printf("    t0 : %12.8f\n", t0);
        printf("    t1 : %12.8f\n", t1);
        printf("    t2 : %12.8f\n", t2);
        printf("    t3 : %12.8f\n", t3);
#endif
    }
}

