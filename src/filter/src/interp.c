//
// Interpolator
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "filter.h"

#include "../../dotprod/src/dotprod.h"

interp interp_create(unsigned int _M, float *_h, unsigned int _h_len)
{
    interp q = (interp) malloc(sizeof(struct interp_s));
    q->h_len = _h_len;
    q->h = (float*) malloc((q->h_len)*sizeof(float));
    // load filter in reverse order
    unsigned int i;
    for (i=0; i<q->h_len; i++)
        q->h[i] = _h[_h_len-i-1];

    q->M = _M;

    q->w = fwindow_create(q->h_len);
    fwindow_clear(q->w);

    return q;
}

void interp_destroy(interp _q)
{
    fwindow_destroy(_q->w);
    free(_q->h);
    free(_q);
}

void interp_print(interp _q)
{
    printf("interp [%u] :\n", _q->M);
    printf("  window:\n");
    fwindow_print(_q->w);
}

void interp_execute(interp _q, float _x, float *_y)
{
    float * r; // read pointer

    unsigned int i;
    for (i=0; i<_q->M; i++) {
        if (i == 0)
            fwindow_push(_q->w,_x);
        else
            fwindow_push(_q->w,0);

        fwindow_read(_q->w,&r);
        _y[i] = fdotprod_run(_q->h, r, _q->h_len);
    }
}

