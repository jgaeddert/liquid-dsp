//
// Finite impulse response filter design
//

#include <math.h>
#include <stdio.h>
#include "window.h"
#include "../../math/src/math.h"

//
float kaiser(unsigned int _n, unsigned int _N, float _beta)
{
    float t = (float)_n - (float)(_N-1)/2;
    float r = 2.0f*t/(float)(_N);
    float a = besseli_0(_beta*sqrtf(1-r*r));
    float b = besseli_0(_beta);
    return a / b;
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

        //printf("t = %f, h1 = %f, h2 = %f\n", t, h1, h2);

        // composite
        _h[i] = h1*h2;
    }
}

