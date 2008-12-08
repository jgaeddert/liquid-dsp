//
// FIR Polyphase filter bank
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../../buffer/src/window.h"
#include "../../dotprod/src/dotprod_internal.h"

// defined:
//  FIRPFB()    name-mangling macro
//  T           data type
//  WINDOW()
//  DOTPROD()

struct FIRPFB(_s) {
    T * h;
    unsigned int h_len;
    unsigned int num_filters;

    WINDOW() w;
    DOTPROD() * dp;

    fir_prototype p;
};

FIRPFB() FIRPFB(_create)(unsigned int _num_filters, T * _h, unsigned int _h_len)
{
    FIRPFB() b = (firpfb) malloc(sizeof(struct FIRPFB(_s)));
    b->num_filters = _num_filters;
    b->h_len = _h_len;

    // each filter is realized as a dotprod object
    b->dp = (DOTPROD()*) malloc((b->num_filters)*sizeof(DOTPROD()));

    // generate bank of sub-samped filters
    // length of each sub-sampled filter
    unsigned int h_sub_len = _h_len / b->num_filters;
    T h_sub[h_sub_len];
    unsigned int i, n;
    for (i=0; i<b->num_filters; i++) {
        for (n=0; n<h_sub_len; n++) {
            h_sub[n] = _h[i + n*(b->num_filters)];
        }

        b->dp[i] = DOTPROD(_create)(h_sub,h_sub_len);
    }

    b->h_len = h_sub_len;

    return b;
}

void FIRPFB(_destroy)(FIRPFB() _b)
{
    unsigned int i;
    for (i=0; i<_b->num_filters; i++)
        DOTPROD(_destroy)(_b->dp[i]);
    free(_b->dp);
    free(_b);
}

void FIRPFB(_print)(FIRPFB() _b)
{
    printf("fir polyphase filterbank [%u] :\n", _b->num_filters);
    unsigned int i,n,s=8;

    for (i=0; i<_b->num_filters; i++)
        printf("%*u",s,i);
    printf("\n");

    for (n=0; n<_b->h_len; n++) {
        for (i=0; i<_b->num_filters; i++) {
            printf(" %*.*f", s-1, s-3-1, _b->dp[i]->v[n]);
        }
        printf("\n");
    }
}

void FIRPFB(_execute)(FIRPFB() _b, unsigned int _i, T _x, T *_y)
{
}


