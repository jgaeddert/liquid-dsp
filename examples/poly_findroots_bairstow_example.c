// 
// poly_findroots_bairstow_example.c
//
// test polynomial root-finding algorithm (Bairstow's method)
//

#include <stdio.h>
#include <math.h>

#include "liquid.h"

void fpoly_findroots_bairstow_recursion(float * _p,
                                        unsigned int _k,
                                        float * _p1,
                                        float * _u,
                                        float * _v);

int main() {
    unsigned int n=6;
    float p[6] = {6,11,-33,-33,11,6};

    unsigned int i;
    float p1[n];    // reduced polynomial
    float u, v;     // polynomial factor coefficients

    printf("initial polynomial:\n");
    for (i=0; i<n; i++)
        printf("  p[%3u]  = %12.8f\n", i, p[i]);

    // compute factor using Bairstow's recursion
    fpoly_findroots_bairstow_recursion(p,6,p1,&u,&v);

    printf("polynomial factor: x^2 + u*x + v\n");
    printf("  u : %12.8f\n", u);
    printf("  v : %12.8f\n", v);

    float t = u*u - 4*v;
    float r0, r1;
    printf("roots:\n");
    if (t >= 0) {
        r0 = 0.5f*(-u + sqrtf(t));
        r1 = 0.5f*(-u - sqrtf(t));
        printf("  r0 : %12.8f\n", r0);
        printf("  r1 : %12.8f\n", r1);
    } else {
        printf("  r0 : %12.8f + j*%12.8f\n", -0.5f*u,  0.5f*sqrt(-t));
        printf("  r1 : %12.8f + j*%12.8f\n", -0.5f*u, -0.5f*sqrt(-t));
    }

    printf("reduced polynomial:\n");
    for (i=0; i<n-2; i++)
        printf("  p1[%3u] = %12.8f\n", i, p1[i]);

    printf("done.\n");
    return 0;
}
