//
// Chebeshev type-I filter design
//

#include <stdio.h>
#include <complex.h>
#include <math.h>

#include "firdes.h"

void cheby1f(unsigned int _n, float _ep, float * _b, float * _a)
{
    // poles
    float complex s[_n];

    float e = _ep;
    float e2 = e*e;
    float nf = (float) _n;

    float b = 0.5*powf( sqrtf(1+1/e2) + 1/e,  1/nf) +
              0.5*powf( sqrtf(1-1/e2) + 1/e, -1/nf);

    float a = 0.5*powf( sqrtf(1+1/e2) + 1/e,  1/nf) -
              0.5*powf( sqrtf(1-1/e2) + 1/e, -1/nf);

    printf("ep : %12.8f\n", e);
    printf("b  : %12.8f\n", b);
    printf("a  : %12.8f\n", a);

}
