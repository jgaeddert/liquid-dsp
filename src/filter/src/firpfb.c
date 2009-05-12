//
// FIR Polyphase filter bank
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// defined:
//  FIRPFB()    name-mangling macro
//  TO          output data type
//  TC          coefficients data type
//  TI          input data type
//  WINDOW()    window macro
//  DOTPROD()   dotprod macro
//  PRINTVAL()  print macro

struct FIRPFB(_s) {
    TC * h;
    unsigned int h_len;
    unsigned int num_filters;

    WINDOW() w;
    DOTPROD() * dp;

    fir_prototype p;
};

FIRPFB() FIRPFB(_create)(unsigned int _num_filters, TC * _h, unsigned int _h_len)
{
    FIRPFB() b = (FIRPFB()) malloc(sizeof(struct FIRPFB(_s)));
    b->num_filters = _num_filters;
    b->h_len = _h_len;

    // each filter is realized as a dotprod object
    b->dp = (DOTPROD()*) malloc((b->num_filters)*sizeof(DOTPROD()));

    // generate bank of sub-samped filters
    // length of each sub-sampled filter
    unsigned int h_sub_len = _h_len / b->num_filters;
    TC h_sub[h_sub_len];
    unsigned int i, n;
    for (i=0; i<b->num_filters; i++) {
        for (n=0; n<h_sub_len; n++) {
            h_sub[n] = _h[i + n*(b->num_filters)];
        }

        b->dp[i] = DOTPROD(_create)(h_sub,h_sub_len);
    }

    b->h_len = h_sub_len;

    // create window buffer
    b->w = WINDOW(_create)(b->h_len);
    WINDOW(_clear)(b->w);

    return b;
}

void FIRPFB(_destroy)(FIRPFB() _b)
{
    unsigned int i;
    for (i=0; i<_b->num_filters; i++)
        DOTPROD(_destroy)(_b->dp[i]);
    free(_b->dp);
    WINDOW(_destroy)(_b->w);
    free(_b);
}

void FIRPFB(_print)(FIRPFB() _b)
{
    printf("fir polyphase filterbank [%u] :\n", _b->num_filters);
    unsigned int i,n;

    for (i=0; i<_b->num_filters; i++)
        printf("%13u",i);
    printf("\n");

    for (n=0; n<_b->h_len; n++) {
        for (i=0; i<_b->num_filters; i++) {
            printf(" ");
            PRINTVAL(_b->dp[i]->h[n]);
        }
        printf("\n");
    }
}

void FIRPFB(_push)(FIRPFB() _b, TI _x)
{
    // push value into window buffer
    WINDOW(_push)(_b->w, _x);
}

void FIRPFB(_execute)(FIRPFB() _b, unsigned int _i, TO *_y)
{
    // read buffer
    TI *r;
    WINDOW(_read)(_b->w, &r);

    // execute dot product
    DOTPROD(_execute)(_b->dp[_i], r, _y);
}

void FIRPFB(_clear)(FIRPFB() _b)
{
    WINDOW(_clear)(_b->w);
}

