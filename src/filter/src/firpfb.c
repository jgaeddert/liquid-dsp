//
// FIR Polyphase filter bank
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "filter.h"
#include "../../dotprod/src/dotprod.h"

firpfb firpfb_create(unsigned int _num_filters, float * _h, unsigned int _h_len)
{
    firpfb b = (firpfb) malloc(sizeof(struct firpfb_s));
    b->num_filters = _num_filters;
    b->f = (fir_filter*) malloc((b->num_filters)*sizeof(struct fir_filter_s));

    // generate bank of sub-samped filters
    // length of each sub-sampled filter
    unsigned int h_sub_len = _h_len / b->num_filters;
    float h_sub[h_sub_len];
    unsigned int i, n;
    for (i=0; i<b->num_filters; i++) {
        for (n=0; n<h_sub_len; n++) {
            h_sub[n] = _h[i + n*(b->num_filters)];
        }

        b->f[i] = fir_filter_create(h_sub,h_sub_len);
    }

    b->h_len = h_sub_len;

    return b;
}

void firpfb_destroy(firpfb _b)
{
    unsigned int i;
    for (i=0; i<_b->num_filters; i++)
        fir_filter_destroy(_b->f[i]);
    free(_b->f);
    free(_b);
}

void firpfb_print(firpfb _b)
{
    printf("fir polyphase filterbank [%u] :\n", _b->num_filters);
    unsigned int i,n,s=8;

    for (i=0; i<_b->num_filters; i++)
        printf("%*u",s,i);
    printf("\n");

    for (n=0; n<_b->h_len; n++) {
        for (i=0; i<_b->num_filters; i++) {
            printf(" %*.*f", s-1, s-3-1, _b->f[i]->h[n]);
        }
        printf("\n");
    }
}

void firpfb_execute(firpfb _b, unsigned int _i, float _x, float *_y)
{
}


