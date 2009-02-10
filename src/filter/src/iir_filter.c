//
// Infinite impulse response filter
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//#include "liquid.composite.h"

// defined:
//  IIR_FILTER()    name-mangling macro
//  T               coefficients type
//  WINDOW()        window macro
//  DOTPROD()       dotprod macro
//  PRINTVAL()      print macro

struct IIR_FILTER(_s) {
    T * b;          // feedforward coefficients
    T * a;          // feedback coefficients
    T * v;          // internal filter state
    unsigned int n; // filter length

    unsigned int nb;
    unsigned int na;

    WINDOW() w;
    //DOTPROD() dpa;
    //DOTPROD() dpb;
};

IIR_FILTER() IIR_FILTER(_create)(T * _b, unsigned int _nb, T * _a, unsigned int _na)
{
    IIR_FILTER() f = (IIR_FILTER()) malloc(sizeof(struct IIR_FILTER(_s)));
    f->nb = _nb;
    f->na = _na;
    f->n = (f->na > f->nb) ? f->na : f->nb;

    f->b = (T *) malloc((f->na)*sizeof(T));
    f->a = (T *) malloc((f->nb)*sizeof(T));

    T a0 = _a[0];

    unsigned int i;
#if 0
    // read values in reverse order
    for (i=0; i<f->nb; i++)
        f->b[i] = _b[f->nb - i - 1];

    for (i=0; i<f->na; i++)
        f->a[i] = _a[f->na - i - 1];
#else
    for (i=0; i<f->nb; i++)
        f->b[i] = _b[i] / a0;

    for (i=0; i<f->na; i++)
        f->a[i] = _a[i] / a0;
#endif

    f->v = (T *) malloc((f->n)*sizeof(T));
    for (i=0; i<f->n; i++)
        f->v[i] = 0;

    f->w = WINDOW(_create)(f->n);
    WINDOW(_clear)(f->w);
    
    return f;
}

IIR_FILTER() IIR_FILTER(_create_prototype)(unsigned int _n)
{
    printf("warning: iir_filter_create_prototype(), not yet implemented\n");
    return NULL;
}

void IIR_FILTER(_destroy)(IIR_FILTER() _f)
{
    WINDOW(_destroy)(_f->w);
    free(_f->b);
    free(_f->a);
    free(_f->v);
    free(_f);
}

void IIR_FILTER(_print)(IIR_FILTER() _f)
{
    printf("iir filter coefficients:\n");
    unsigned int i;

    printf("  b :");
    for (i=0; i<_f->nb; i++)
        PRINTVAL(_f->b[i]);
    printf("\n");

    printf("  a :");
    for (i=0; i<_f->na; i++)
        PRINTVAL(_f->a[i]);
    printf("\n");

    printf("  v :");
    for (i=0; i<_f->n; i++)
        PRINTVAL(_f->v[i]);
    printf("\n");
}

void IIR_FILTER(_clear)(IIR_FILTER() _f)
{
    // set to zero
    unsigned int i;
    for (i=0; i<_f->n; i++)
        _f->v[i] = 0;
}

void IIR_FILTER(_execute)(IIR_FILTER() _f, T _x, T *_y)
{
    unsigned int i;

    // advance buffer
    for (i=_f->n-1; i>0; i--)
        _f->v[i] = _f->v[i-1];

    // compute new v
    T v0 = _x;
    for (i=1; i<_f->na; i++)
        v0 -= _f->a[i] * _f->v[i];
    _f->v[0] = v0;

    // compute new y
    T y0 = 0;
    for (i=0; i<_f->nb; i++)
        y0 += _f->b[i] * _f->v[i];

    // set return value
    *_y = y0;
}

unsigned int IIR_FILTER(_get_length)(IIR_FILTER() _f)
{
    return _f->n;
}

