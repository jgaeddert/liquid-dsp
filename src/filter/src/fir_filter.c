//
// Finite impulse response filter
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "filter_internal.h"

// defined:
//  FIR_FILTER()    name-mangling macro
//  T               coefficients type
//  WINDOW()        window macro
//  DOTPROD()       dotprod macro
//  PRINTVAL()      print macro

struct FIR_FILTER(_s) {
    TC * h;
    unsigned int h_len;

    WINDOW() w;

    fir_prototype p;
};

FIR_FILTER() FIR_FILTER(_create)(TC * _h, unsigned int _n)
{
    FIR_FILTER() f = (FIR_FILTER()) malloc(sizeof(struct FIR_FILTER(_s)));
    f->h_len = _n;
    f->h = (TC *) malloc((f->h_len)*sizeof(TC));

    // load filter in reverse order
    unsigned int i;
    for (i=_n; i>0; i--)
        f->h[i-1] = _h[_n-i];

    f->w = WINDOW(_create)(f->h_len);
    WINDOW(_clear)(f->w);

    return f;
}

FIR_FILTER() FIR_FILTER(_create_prototype)(unsigned int _n)
{
    printf("warning: fir_filter_create_prototype(), not yet implemented\n");
    FIR_FILTER() f = (FIR_FILTER()) malloc(sizeof(struct FIR_FILTER(_s)));
    f->h_len = _n;
    f->h = (TC *) malloc((f->h_len)*sizeof(TC));

    // use remez here

    return f;
}

void FIR_FILTER(_destroy)(FIR_FILTER() _f)
{
    WINDOW(_destroy)(_f->w);
    free(_f->h);
    free(_f);
}

void FIR_FILTER(_print)(FIR_FILTER() _f)
{
    printf("filter coefficients:\n");
    unsigned int i, n = _f->h_len;
    for (i=0; i<n; i++) {
        printf("  h(%3u) = ", i+1);
        PRINTVAL(_f->h[n-i-1]);
        printf("\n");
    }
}

void FIR_FILTER(_push)(FIR_FILTER() _f, TI _x)
{
    WINDOW(_push)(_f->w, _x);
}

void FIR_FILTER(_execute)(FIR_FILTER() _f, TO *_y)
{
    TI *r;
    WINDOW(_read)(_f->w, &r);
    *_y = DOTPROD(_run)(_f->h, r, _f->h_len);
}

unsigned int FIR_FILTER(_get_length)(FIR_FILTER() _f)
{
    return _f->h_len;
}

