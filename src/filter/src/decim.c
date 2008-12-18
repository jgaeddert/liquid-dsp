//
// Decimator
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../dotprod/src/dotprod_internal.h"
#include "../../buffer/src/buffer.h"

// defined:
//  DECIM()     name-mangling macro
//  T           data type
//  WINDOW()    window macro
//  DOTPROD()   dotprod macro
//  PRINTVAL()  print macro

struct DECIM(_s) {
    T * h;
    unsigned int h_len;
    unsigned int D;

    fir_prototype p;
    WINDOW() w;
    //DOTPROD() dp;
};

DECIM() DECIM(_create)(unsigned int _D, T *_h, unsigned int _h_len)
{
    DECIM() q = (DECIM()) malloc(sizeof(struct DECIM(_s)));
    q->h_len = _h_len;
    q->h = (T*) malloc((q->h_len)*sizeof(T));

    // load filter in reverse order
    unsigned int i;
    for (i=0; i<q->h_len; i++)
        q->h[i] = _h[_h_len-i-1];

    q->D = _D;

    q->w = WINDOW(_create)(q->h_len);
    WINDOW(_clear)(q->w);

    return q;
}

void DECIM(_destroy)(DECIM() _q)
{
    WINDOW(_destroy)(_q->w);
    free(_q->h);
    free(_q);
}

void DECIM(_print)(DECIM() _q)
{
    printf("DECIM() [%u] :\n", _q->D);
    printf("  window:\n");
    WINDOW(_print)(_q->w);
}

void DECIM(_execute)(DECIM() _q, T *_x, T *_y, unsigned int _index)
{
    T * r; // read pointer
    unsigned int i;
    for (i=0; i<_q->D; i++) {
        WINDOW(_push)(_q->w, _x[i]);
        if (i==_index) {
            WINDOW(_read)(_q->w, &r);
            *_y = DOTPROD(_run)(_q->h, r, _q->h_len);
        }
    }
}

