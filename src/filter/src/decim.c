//
// Decimator
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "filter.h"

#include "../../dotprod/src/dotprod.h"

decim decim_create(unsigned int _D, float *_h, unsigned int _h_len)
{
    decim q = (decim) malloc(sizeof(struct decim_s));
    q->h_len = _h_len;
    q->h = (float*) malloc((q->h_len)*sizeof(float));

    // load filter in reverse order
    unsigned int i;
    for (i=0; i<q->h_len; i++)
        q->h[i] = _h[_h_len-i-1];

    q->D = _D;

    q->w = fwindow_create(q->h_len);
    fwindow_clear(q->w);

    return q;
}

void decim_destroy(decim _q)
{
    fwindow_destroy(_q->w);
    free(_q->h);
    free(_q);
}

void decim_print(decim _q)
{
    printf("decim [%u] :\n", _q->D);
    printf("  window:\n");
    fwindow_print(_q->w);
}

void decim_execute(decim _q, float *_x, float *_y)
{
    float * r; // read pointer
    fwindow_write(_q->w, _x, _q->D);
    fwindow_read(_q->w, &r);
    *_y = fdotprod_run(_q->h, r, _q->h_len);
}

