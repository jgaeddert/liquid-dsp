//
// fft
//

#include <stdlib.h>
#include <math.h>

#include "fft_internal.h"

void fft_destroy_plan(fftplan _p)
{
    free(_p->twiddle);
    free(_p);
}

fftplan fft_create_plan(unsigned int _n, float complex * _x, float complex * _y, int _dir)
{
    fftplan p = (fftplan) malloc(_n*sizeof(struct fftplan_s));

    p->n = _n;
    p->len = p->n;
    p->x = _x;
    p->y = _y;

    if (_dir == FFT_FORWARD)
        p->direction = FFT_FORWARD;
    else
        p->direction = FFT_REVERSE;

    // initialize twiddle
    p->twiddle = NULL;

    return p;
}

