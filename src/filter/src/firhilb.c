//
// FIR Hilbert transform
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

// defined:
//  FIRHILB()       name-mangling macro
//  T               coefficients type
//  WINDOW()        window macro
//  DOTPROD()       dotprod macro
//  PRINTVAL()      print macro

struct FIRHILB(_s) {
    T * h;
    unsigned int h_len;

    T * hq;
    unsigned int hq_len;

    unsigned int m;

    T * wi;
    unsigned int wi_index;
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

    f->hq_len = 2*(f->m);
    f->hq = (T *) malloc((f->hq_len)*sizeof(T));

    unsigned int i;
    float t, h1, h2, s;
    float beta = 6.0f;
    for (i=0; i<f->h_len; i++) {
        t = (float)i - (float)(f->h_len-1)/2.0f;
        h1 = sincf(t/2.0f);
        h2 = kaiser(i,f->h_len,beta);
        s  = sinf(M_PI*t/2);
        f->h[i] = s*h1*h2;
    }

    // resample, alternate sign, reverse direction
    unsigned int j=0;
    for (i=1; i<f->h_len; i+=2)
        f->hq[j++] = f->h[f->h_len - i - 1];

    f->wq = WINDOW(_create)(2*(f->m));
    WINDOW(_clear)(f->wq);

    f->wi = (float*)malloc((f->m)*sizeof(float));
    for (i=0; i<f->m; i++)
        f->wi[i] = 0;
    f->wi_index = 0;

    return f;
}

void FIRHILB(_destroy)(FIRHILB() _f)
{
    WINDOW(_destroy)(_f->wq);
    free(_f->wi);
    free(_f->h);
    free(_f->hq);
    free(_f);
}

void FIRHILB(_print)(FIRHILB() _f)
{
    printf("fir hilbert transform: [%u]\n", _f->h_len);
    unsigned int i;
    for (i=0; i<_f->h_len; i++) {
        printf("  h(%4u) = %8.4f;\n", i+1, _f->h[i]);
    }
    printf("---\n");
    for (i=0; i<_f->hq_len; i++) {
        printf("  hq(%4u) = %8.4f;\n", i+1, _f->hq[i]);
    }
}

void FIRHILB(_clear)(FIRHILB() _f)
{
    WINDOW(_clear)(_f->wq);
    unsigned int i;
    for (i=0; i<_f->m; i++)
        _f->wi[i] = 0;
    _f->wi_index = 0;
}

void FIRHILB(_decim_execute)(FIRHILB() _f, T * _x, T complex *_y)
{
    T * r;
    T yi, yq;

    // compute quadrature component (filter branch)
    WINDOW(_push)(_f->wq, _x[0]);
    WINDOW(_read)(_f->wq, &r);
    // TODO yq = DOTPROD(_execute)(_f->dpq, r);
    DOTPROD(_run)(_f->hq, r, _f->hq_len, &yq);

    // compute in-phase component (delay branch)
    yi = _f->wi[_f->wi_index];
    _f->wi[_f->wi_index] = _x[1];
    _f->wi_index = (_f->wi_index+1) % (_f->m);

    // set return value
    *_y = yi + _Complex_I * yq;
}

void FIRHILB(_interp_execute)(FIRHILB() _f, T complex _x, T *_y)
{
    T * r;  // read pointer

    // TODO macro for crealf, cimagf?
    
    // compute first branch (delay)
    _y[0] = _f->wi[_f->wi_index];
    _f->wi[_f->wi_index] = cimagf(_x);
    _f->wi_index = (_f->wi_index+1) % (_f->m);

    // compute second branch (filter)
    WINDOW(_push)(_f->wq, crealf(_x));
    WINDOW(_read)(_f->wq, &r);
    //yq = DOTPROD(_execute)(_f->dpq, r);
    DOTPROD(_run)(_f->hq, r, _f->hq_len, &_y[1]);

}

