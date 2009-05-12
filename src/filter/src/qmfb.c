//
// quadrature mirror filterbank (QMFB)
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.internal.h"

// defined:
//  QMFB()          name-mangling macro
//  TO              output data type
//  TC              coefficient data type
//  TI              input data type
//  WINDOW()        window macro
//  DOTPROD()       dotprod macro
//  PRINTVAL()      print macro

struct QMFB(_s) {
    TC * h;             // filter prototype
    unsigned int m;     // primitive filter length
    unsigned int h_len; // actual filter length: h_len = 4*m+1

    // lower branch (filter)
    TC * h1;
    WINDOW() w1;
    unsigned int h1_len;

    // upper branch (delay line)
    TI * w0;
    unsigned int w0_index;
};

QMFB() QMFB(_create)(unsigned int _m)
{
    QMFB() f = (QMFB()) malloc(sizeof(struct QMFB(_s)));

    // compute filter length
    // h_len = 2*(2*m) + 1
    f->m = _m;
    if (f->m < 2)
        f->m = 2;

    f->h_len = 4*(f->m) + 1;
    f->h = (TC *) malloc((f->h_len)*sizeof(TC));

    f->h1_len = 2*(f->m);
    f->h1 = (TC *) malloc((f->h1_len)*sizeof(TC));

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

    f->w0 = (TI*)malloc((f->m)*sizeof(TI));
    for (i=0; i<f->m; i++)
        f->w0[i] = 0;
    f->w0_index = 0;

    return f;
}

QMFB() QMFB(_recreate)(QMFB() _f, unsigned int _h_len)
{
    // TODO implement this method
    printf("error: qmfb_xxxt_recreate(), method not supported yet\n");
    exit(1);
    return NULL;
}

void QMFB(_destroy)(QMFB() _f)
{
    WINDOW(_destroy)(_f->w1);
    free(_f->w0);
    free(_f->h);
    free(_f->h1);
    free(_f);
}

void QMFB(_print)(QMFB() _f)
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

void QMFB(_clear)(QMFB() _f)
{
    WINDOW(_clear)(_f->w1);
    unsigned int i;
    for (i=0; i<_f->m; i++)
        _f->w0[i] = 0;
    _f->w0_index = 0;
}

void QMFB(_decim_execute)(QMFB() _f, TI * _x, TO *_y)
{
    TI * r;
    TO y0, y1;

    // compute filter branch
    WINDOW(_push)(_f->w1, _x[0]);
    WINDOW(_read)(_f->w1, &r);
    // TODO yq = DOTPROD(_execute)(_f->dpq, r);
    DOTPROD(_run4)(_f->h1, r, _f->h1_len, &y1);

    // compute delay branch
    y0 = _f->w0[_f->w0_index];
    _f->w0[_f->w0_index] = _x[1];
    _f->w0_index = (_f->w0_index+1) % (_f->m);

    // set return value
    *_y = y0 + y1;
}

void QMFB(_interp_execute)(QMFB() _f, TI _x, TO *_y)
{
    TI * r;  // read pointer

    // TODO macro for crealf, cimagf?
    
    // compute first branch (delay)
    _y[0] = _f->w0[_f->w0_index];
    _f->w0[_f->w0_index] = _x;
    _f->w0_index = (_f->w0_index+1) % (_f->m);

    // compute second branch (filter)
    WINDOW(_push)(_f->w1, _x);
    WINDOW(_read)(_f->w1, &r);
    //yq = DOTPROD(_execute)(_f->dpq, r);
    DOTPROD(_run4)(_f->h1, r, _f->h1_len, &_y[1]);

}

void QMFB(_analysis_execute)(QMFB() _q,
                             float complex * _x,
                             float complex * _y0,
                             float complex * _y1)
{
}

void QMFB(_synthesis_execute)(QMFB() _q,
                             float complex _y0,
                             float complex _y1,
                             float complex * _x)
{
}

