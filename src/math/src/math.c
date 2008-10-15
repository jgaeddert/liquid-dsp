//
// Finite impulse response filter design
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "math.h"

float lngammaf(float _z) {
    float g = 0.5*( logf(2*M_PI)-log(_z) );
    g += _z*( logf(_z+(1/(12.0f*_z-0.1f/_z)))-1);
    return g;
}

float gammaf(float _z) {
    return expf( lngammaf(_z) );
}

float factorialf(unsigned int _n) {
    return fabsf(gammaf((float)(_n+1)));
}

// Modified bessel function of the first kind
#define NUM_BESSELI0_ITERATIONS 16
float besseli_0(float _z)
{
    unsigned int k;
    float t, y=0.0f;
    for (k=0; k<NUM_BESSELI0_ITERATIONS; k++) {
        t = powf(_z/2, (float)k) / gammaf((float)k+1);
        y += t*t;
    }
    return y;
}

//
float sincf(float _x) {
    // _x ~ 0 approximation
    //if (fabsf(_x) < 0.01f)
    //    return expf(-lngammaf(1+_x) - lngammaf(1-_x));

    // _x ~ 0 approximation
    // from : http://mathworld.wolfram.com/SincFunction.html
    // sinc(z) = \prod_{k=1}^{\infty} { cos(\pi z / 2^k) }
    if (fabsf(_x) < 0.01f)
        return cosf(M_PI*_x/2.0f)*cosf(M_PI*_x/4.0f)*cosf(M_PI*_x/8.0f);

    return sinf(M_PI*_x)/(M_PI*_x);
}

