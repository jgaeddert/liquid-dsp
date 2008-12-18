//
// FIR Hilbert transform
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "firdes.h"

#include "../../math/src/lmath.h"
#include "../../dotprod/src/dotprod_internal.h"
#include "../../buffer/src/buffer.h"

// defined:
//  RESAMP2()       name-mangling macro
//  T               coefficients type
//  WINDOW()        window macro
//  DOTPROD()       dotprod macro
//  PRINTVAL()      print macro

struct RESAMP2(_s) {
    T * h;              // filter prototype
    unsigned int m;     // 
    unsigned int h_len; // h_len = 4*m+1

    // lower branch (filter)
    T * h1;
    WINDOW() w1;
    unsigned int h1_len;

    // upper branch (delay line)
    T * w0;
    unsigned int w0_index;
};

RESAMP2() RESAMP2(_create)(unsigned int _h_len)
{
    RESAMP2() f = (RESAMP2()) malloc(sizeof(struct RESAMP2(_s)));
    f->h_len = _h_len;

    // change filter length as necessary
    // h_len = 2*(2*m) + 1
    f->m = (_h_len-1)/4;
    if (f->m < 2)
        f->m = 2;

    f->h_len = 4*(f->m) + 1;
    f->h = (T *) malloc((f->h_len)*sizeof(T));

    f->h1_len = 2*(f->m);
    f->h1 = (T *) malloc((f->h1_len)*sizeof(T));

    // design filter prototype
    unsigned int i;
    float t, h1, h2;
    float beta = 6.0f;
    for (i=0; i<f->h_len; i++) {
        t = (float)i - (float)(f->h_len-1)/2.0f;
        h1 = sincf(t/2.0f);
        h2 = kaiser(i,f->h_len,beta);
        f->h[i] = h1*h2;
    }

    // resample, alternate sign, reverse direction
    unsigned int j=0;
    for (i=1; i<f->h_len; i+=2)
        f->h1[j++] = f->h[f->h_len - i - 1];

    f->w1 = WINDOW(_create)(2*(f->m));
    WINDOW(_clear)(f->w1);

    f->w0 = (T*)malloc((f->m)*sizeof(T));
    for (i=0; i<f->m; i++)
        f->w0[i] = 0;
    f->w0_index = 0;

    return f;
}

void RESAMP2(_destroy)(RESAMP2() _f)
{
    WINDOW(_destroy)(_f->w1);
    free(_f->w0);
    free(_f->h);
    free(_f->h1);
    free(_f);
}

void RESAMP2(_print)(RESAMP2() _f)
{
    printf("fir half-band resampler: [%u]\n", _f->h_len);
    unsigned int i;
    for (i=0; i<_f->h_len; i++) {
        printf("  h(%4u) = ", i+1);
        PRINTVAL(_f->h[i]);
        printf(";\n");
    }
    printf("---\n");
    for (i=0; i<_f->h1_len; i++) {
        printf("  h1(%4u) = ", i+1);
        PRINTVAL(_f->h1[i]);
        printf(";\n");
    }
}

void RESAMP2(_clear)(RESAMP2() _f)
{
    WINDOW(_clear)(_f->w1);
    unsigned int i;
    for (i=0; i<_f->m; i++)
        _f->w0[i] = 0;
    _f->w0_index = 0;
}

void RESAMP2(_decim_execute)(RESAMP2() _f, T * _x, T *_y)
{
    T * r;
    T y0, y1;

    // compute filter branch
    WINDOW(_push)(_f->w1, _x[0]);
    WINDOW(_read)(_f->w1, &r);
    // TODO yq = DOTPROD(_execute)(_f->dpq, r);
    y1 = DOTPROD(_run)(_f->h1, r, _f->h1_len);

    // compute delay branch
    y0 = _f->w0[_f->w0_index];
    _f->w0[_f->w0_index] = _x[1];
    _f->w0_index = (_f->w0_index+1) % (_f->m);

    // set return value
    *_y = y0 + y1;
}

void RESAMP2(_interp_execute)(RESAMP2() _f, T _x, T *_y)
{
    T * r;  // read pointer

    // TODO macro for crealf, cimagf?
    
    // compute first branch (delay)
    _y[0] = _f->w0[_f->w0_index];
    _f->w0[_f->w0_index] = _x;
    _f->w0_index = (_f->w0_index+1) % (_f->m);

    // compute second branch (filter)
    WINDOW(_push)(_f->w1, _x);
    WINDOW(_read)(_f->w1, &r);
    //yq = DOTPROD(_execute)(_f->dpq, r);
    _y[1] = DOTPROD(_run)(_f->h1, r, _f->h1_len);

}

