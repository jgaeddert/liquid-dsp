//
// Finite impulse response filter
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "filter.h"
#include "../../dotprod/src/dotprod.h"

fir_filter fir_filter_create(float * _h, unsigned int _n)
{
    fir_filter f = (fir_filter) malloc(sizeof(struct fir_filter_s));
    f->h_len = _n;
    f->h = (float*) malloc((f->h_len)*sizeof(float));

    // load filter in reverse order
    unsigned int i;
    for (i=_n; i>0; i--)
        f->h[i-1] = _h[_n-i];

    return f;
}

fir_filter fir_filter_create_prototype(unsigned int _n)
{
    printf("warning: fir_filter_create_prototype(), not yet implemented\n");
    fir_filter f = (fir_filter) malloc(sizeof(struct fir_filter_s));
    f->h_len = _n;
    f->h = (float*) malloc((f->h_len)*sizeof(float));

    // use remez here

    return f;
}

void fir_filter_destroy(fir_filter _f)
{
    free(_f->h);
    free(_f);
}

void fir_filter_print(fir_filter _f)
{
    printf("filter coefficients:\n");
    unsigned int i, n = _f->h_len;
    for (i=0; i<n; i++)
        printf(" %3u : %12.4E\n", i, _f->h[n-i-1]);
}

float fir_filter_execute(fir_filter _f, float * _v)
{
    return fdotprod_run(_f->h, _v, _f->h_len);
}

unsigned int fir_filter_get_length(fir_filter _f)
{
    return _f->h_len;
}

