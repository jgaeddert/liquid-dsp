// 
// poly_findroots_example.c
//
// test polynomial root-finding algorithm(s)
//

#include <stdio.h>
#include <math.h>

#include "liquid.h"

void besselpoly(unsigned int _n, int * _p);

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

