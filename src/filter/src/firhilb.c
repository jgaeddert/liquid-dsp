//
// FIR Hilbert transform
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "firdes.h"

#include "../../math/src/lmath.h"
#include "../../dotprod/src/dotprod_internal.h"
#include "../../buffer/src/window.h"

// defined:
//  FIRHILB()       name-mangling macro
//  T               coefficients type
//  WINDOW()        window macro
//  DOTPROD()       dotprod macro
//  PRINTVAL()      print macro

struct FIRHILB(_s) {
    T * h;
    unsigned int h_len;
    unsigned int m;
    unsigned int delay;

    WINDOW() wi;
    WINDOW() wq;
};

FIRHILB() FIRHILB(_create)(unsigned int _h_len)
{
    FIRHILB() f = (FIRHILB()) malloc(sizeof(struct FIRHILB(_s)));
    f->h_len = _h_len;

    // change filter length as necessary
    // h_len = 2*(2*m) + 1
    f->m = (_h_len-1)/4;
    if (f->m < 2)
        f->m = 2;

    f->h_len = 4*(f->m) + 1;

    f->h = (T *) malloc((f->h_len)*sizeof(T));
    unsigned int i;
    float t, h1, h2;
    float beta = 6.0f;
    for (i=0; i<f->h_len; i++) {
        t = (float)i - (float)(f->h_len-1)/2.0f;
        h1 = sincf(t/2.0f);
        h2 = kaiser(i,f->h_len,beta);
        f->h[i] = h1*h2;
    }

    // design filter
    T h_tmp[f->h_len];

    // resample, alternate sign
    unsigned int j=0;
    for (i=1; i<f->h_len; i+=2)
        f->h[j++] = (i%2) ? h_tmp[i] : -h_tmp[i];

    f->wi = WINDOW(_create)(f->h_len);
    f->wq = WINDOW(_create)(f->h_len);
    WINDOW(_clear)(f->wi);
    WINDOW(_clear)(f->wq);

    return f;
}

void FIRHILB(_destroy)(FIRHILB() _f)
{
    WINDOW(_destroy)(_f->wi);
    WINDOW(_destroy)(_f->wq);
    free(_f->h);
    free(_f);
}

void FIRHILB(_print)(FIRHILB() _f)
{
    printf("fir hilbert transform: [%u]\n", _f->h_len);
    unsigned int i, n = _f->h_len;
    for (i=0; i<n; i++) {
        printf("  h(%3u) = ", i+1);
        PRINTVAL(_f->h[n-i-1]);
        printf("\n");
    }
}

void FIRHILB(_execute)(FIRHILB() _f, T * _x, T complex *_y)
{
    *_y = _x[0];
}

