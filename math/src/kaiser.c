//
// Finite impulse response filter design
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "kaiser.h"

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
float kaiser(unsigned int _n, unsigned int _N, float _beta)
{
    float t = (float)_n - (float)(_N-1)/2;
    float r = 2.0f*t/(float)(_N);
    float a = besseli_0(_beta*sqrtf(1-r*r));
    float b = besseli_0(_beta);
    return a / b;
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

void fir_kaiser_window(unsigned int _n, float _fc, float _atten_dB, float *_h) {
    // chooise kaiser beta parameter (approximate)
    // TODO find better method for initial estimation
    float beta = (_atten_dB - 16.0f) / 8.0f;

    if (beta < 2.0f) beta = 2.0f;
    else if (beta > 12.0f)  beta = 12.0f;

    float t, h1, h2;
    unsigned int i;
    for (i=0; i<_n; i++) {
        t = (float)i - (float)(_n-1)/2;
        
        // sinc prototype
        h1 = sincf(_fc*t);

        // kaiser window
        h2 = kaiser(i,_n,beta);

        printf("t = %f, h1 = %f, h2 = %f\n", t, h1, h2);

        // composite
        _h[i] = h1*h2;
    }
}

