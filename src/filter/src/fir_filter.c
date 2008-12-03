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

    f->w = fwindow_create(f->h_len);
    fwindow_clear(f->w);

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
    fwindow_destroy(_f->w);
    free(_f->h);
    free(_f);
}

void fir_filter_print(fir_filter _f)
{
    printf("filter coefficients:\n");
    unsigned int i, n = _f->h_len;
    for (i=0; i<n; i++)
        printf(" h(%u) = %12.4E;\n", i+1, _f->h[n-i-1]);
}

void fir_filter_push(fir_filter _f, float _x)
{
    fwindow_push(_f->w, _x);
}

void fir_filter_execute(fir_filter _f, float *_y)
{
    float *r;
    fwindow_read(_f->w, &r);
    *_y = fdotprod_run(_f->h, r, _f->h_len);
}

unsigned int fir_filter_get_length(fir_filter _f)
{
    return _f->h_len;
}

