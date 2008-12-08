//
// Infinite impulse response filter
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//#include "../../dotprod/src/dotprod_internal.h"

// defined:
//  IIR_FILTER  name-mangling macro
//  T           coefficients type
//  PRINTLINE   print macro

struct IIR_FILTER(_s) {
    T * b;          // feedforward coefficients
    T * a;          // feedback coefficients
    T * v;          // internal filter state
    unsigned int n; // filter length
    unsigned int i; // state index

    unsigned int nb;
    unsigned int na;
};

IIR_FILTER() IIR_FILTER(_create)(T * _b, unsigned int _nb, T * _a, unsigned int _na)
{
    IIR_FILTER() f = (IIR_FILTER()) malloc(sizeof(struct IIR_FILTER(_s)));
    f->nb = _nb;
    f->na = _na;
    f->n = (f->na > f->nb) ? f->na : f->nb;

    f->b = (T *) malloc((f->na)*sizeof(T));
    f->a = (T *) malloc((f->nb)*sizeof(T));

    memcpy(f->b, _b, (f->n)*sizeof(T));
    memcpy(f->a, _a, (f->n)*sizeof(T));

    f->v = (T *) malloc((f->n)*sizeof(T));
    unsigned int i;
    for (i=0; i<f->n; i++)
        f->v[i] = 0;

    f->i = 0;
    
    return f;
}

IIR_FILTER() IIR_FILTER(_create_prototype)(unsigned int _n)
{
    printf("warning: iir_filter_create_prototype(), not yet implemented\n");
    return NULL;
}

void IIR_FILTER(_destroy)(IIR_FILTER() _f)
{
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
        printf("%12.4e", _f->b[i]);
    printf("\n");

    printf("  a :");
    for (i=0; i<_f->na; i++)
        printf("%12.4e", _f->a[i]);
    printf("\n");

    printf("  v :");
    for (i=0; i<_f->n; i++)
        printf("%12.4e", _f->v[i]);
    printf("\n");
}

void IIR_FILTER(_clear)(IIR_FILTER() _f)
{
    // set to zero
    unsigned int i;
    for (i=0; i<_f->n; i++)
        _f->v[i] = 0;

    _f->i = 0;
}

void IIR_FILTER(_execute)(IIR_FILTER() _f, T _x, T *_y)
{
    unsigned int i;

    T v0 = _x;

    for (i=1; i<_f->na; i++)
        v0 += _f->a[i] * _f->v[i];

    T y0 = 0;

    for (i=0; i<_f->nb; i++)
        y0 += _f->b[i] * _f->v[i];

    // update state
    for (i=_f->n-1; i>0; i--)
        _f->v[i] = _f->v[i-1];

    // set return value
    *_y = y0;
}

unsigned int IIR_FILTER(_get_length)(IIR_FILTER() _f)
{
    return _f->n;
}

