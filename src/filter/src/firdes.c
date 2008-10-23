//
// Finite impulse response filter design
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//#include <stdio.h>

#include "firdes.h"
#include "window.h"
#include "../../math/src/math.h" // sincf()

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

// Design FIR using kaiser window
//  _n      : filter length
//  _fc     : cutoff frequency
//  _slsl   : sidelobe suppression level (dB attenuation)
//  _h      : output coefficient buffer
void fir_kaiser_window(unsigned int _n, float _fc, float _slsl, float *_h) {
    // chooise kaiser beta parameter (approximate)
    float beta = 
        -1.5637e-05f * _slsl * _slsl +
         1.1656e-01f * _slsl +
        -1.3230e+00f;

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


// Design FIR doppler filter
//  _n      : filter length
//  _fd     : normalized doppler frequency (0 < _fd < 0.5)
//  _K      : Rice fading factor (K >= 0)
//  _h      : output coefficient buffer
void fir_design_doppler(unsigned int _n, float _fd, float _K, float *_h)
{
    // use ifft/window method?
}

