//
// Finite impulse response filter design
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "firdes.h"

//
void fir_design_halfband_windowed_sinc(float * _h, unsigned int _n)
{
    // assume _n is odd
    if ((_n % 2)==0) {
        printf("warning: fir_design_halfband_windowed_sinc(), _n should be odd\n");
        _n--;
        _h[_n] = 0.0f;
    }

    unsigned int i;
    for (i=0; i<_n; i++) {
        int z = (int)i - (int)((_n-1)/2);
        float t = M_PI * 0.5f * (float)z;

        // sinc(x) = sin(x)/x
        float sinc;
        if (z==0)
            sinc = 1.0f;
        else
            sinc = sinf(t)/t;

        // Hamming windowing function
        float w = 0.54f - 0.46f*cosf(2*M_PI*i/(_n-1));

        _h[i] = sinc*w;
    }
}

