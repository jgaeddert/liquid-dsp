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
};

IIR_FILTER() IIR_FILTER(_create)(T * _b, T * _a, unsigned int _n)
{
    IIR_FILTER() f = (IIR_FILTER()) malloc(sizeof(struct IIR_FILTER(_s)));
    f->n = _n;

    f->b = (T *) malloc((f->n)*sizeof(T));
    f->a = (T *) malloc((f->n)*sizeof(T));

    memcpy(f->b, _b, (f->n)*sizeof(T));
    memcpy(f->a, _a, (f->n)*sizeof(T));

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
    free(_f);
}

void IIR_FILTER(_print)(IIR_FILTER() _f)
{
    printf("filter coefficients:\n");
    //unsigned int i, n = _f->n;
    //for (i=0; i<n; i++)
    //    printf(" h(%u) = %12.4E;\n", i+1, _f->h[n-i-1]);
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
    // push value into buffer
    _f->v[_f->i] = _x;

    // update state
    unsigned int i, k;
    for (i=0; i<_f->n; i++) {
        k = (i + _f->i) % _f->n;
        _f->v[k] = 0.0f;
    }

    // set return value
    *_y = _f->v[i];

    _f->i++;
}

unsigned int IIR_FILTER(_get_length)(IIR_FILTER() _f)
{
    return _f->n;
}

