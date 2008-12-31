//
// Interpolator
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../dotprod/src/dotprod.h"
#include "../../buffer/src/buffer.h"

// defined:
//  INTERP()    name-mangling macro
//  T           data type
//  WINDOW()    window macro
//  DOTPROD()   dotprod macro
//  PRINTVAL()  print macro

struct INTERP(_s) {
    T * h;
    unsigned int h_len;
    unsigned int M;

    fir_prototype p;
    WINDOW() w;
    //DOTPROD() dp;
};

INTERP() INTERP(_create)(unsigned int _M, T *_h, unsigned int _h_len)
{
    INTERP() q = (INTERP()) malloc(sizeof(struct INTERP(_s)));
    q->h_len = _h_len;
    q->h = (T*) malloc((q->h_len)*sizeof(T));
    // load filter in reverse order
    unsigned int i;
    for (i=0; i<q->h_len; i++)
        q->h[i] = _h[_h_len-i-1];

    q->M = _M;

    q->w = WINDOW(_create)(q->h_len);
    WINDOW(_clear)(q->w);

    return q;
}

void INTERP(_destroy)(INTERP() _q)
{
    WINDOW(_destroy)(_q->w);
    free(_q->h);
    free(_q);
}

void INTERP(_print)(INTERP() _q)
{
    printf("interp() [%u] :\n", _q->M);
    printf("  window:\n");
    WINDOW(_print)(_q->w);
}

void INTERP(_execute)(INTERP() _q, T _x, T *_y)
{
    T * r; // read pointer

    unsigned int i;
    for (i=0; i<_q->M; i++) {
        if (i == 0)
            WINDOW(_push)(_q->w,_x);
        else
            WINDOW(_push)(_q->w,0);

        WINDOW(_read)(_q->w,&r);
        _y[i] = DOTPROD(_run)(_q->h, r, _q->h_len);
    }
}

