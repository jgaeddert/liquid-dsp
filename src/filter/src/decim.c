//
// Decimator
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// defined:
//  DECIM()     name-mangling macro
//  TO          output data type
//  TC          coefficients data type
//  TI          input data type
//  WINDOW()    window macro
//  DOTPROD()   dotprod macro
//  PRINTVAL()  print macro

struct DECIM(_s) {
    TC * h;
    unsigned int h_len;
    unsigned int D;

    fir_prototype p;
    WINDOW() w;
    //DOTPROD() dp;
};

DECIM() DECIM(_create)(unsigned int _D, TC *_h, unsigned int _h_len)
{
    DECIM() q = (DECIM()) malloc(sizeof(struct DECIM(_s)));
    q->h_len = _h_len;
    q->h = (TC*) malloc((q->h_len)*sizeof(TC));

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

void DECIM(_execute)(DECIM() _q, TI *_x, TO *_y, unsigned int _index)
{
    TI * r; // read pointer
    unsigned int i;
    for (i=0; i<_q->D; i++) {
        WINDOW(_push)(_q->w, _x[i]);
        if (i==_index) {
            WINDOW(_read)(_q->w, &r);
            DOTPROD(_run)(_q->h, r, _q->h_len, _y);
        }
    }
}

