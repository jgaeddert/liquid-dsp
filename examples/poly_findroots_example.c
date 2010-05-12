// 
// poly_findroots_example.c
//
// test polynomial root-finding algorithm (Bairstow's method)
//

#include <stdio.h>
#include <math.h>

#include "liquid.internal.h"

int main() {
    unsigned int n=6;   // polynomial length (order+1)
    unsigned int i;

    // generate polynomial
    float p[6] = {6,11,-33,-33,11,6};
    float complex roots[n-1];

    // print polynomial
    printf("polynomial:\n");
    for (i=0; i<n; i++)
        printf("  p[%3u] = %12.4f\n", i, p[i]);

    // compute roots of polynomial
    polyf_findroots(p,n,roots);
    printf("roots:\n");
    for (i=0; i<n-1; i++) {
        printf("  r[%3u] = %12.8f + j*%12.8f\n", i,
                                                 crealf(roots[i]),
                                                 cimagf(roots[i]));
    }

    printf("done.\n");
    return 0;
}
