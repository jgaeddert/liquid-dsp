//
// Decimator: create
//

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "decim_internal.h"
#include "window.h"
#include "firdes.h"

// Create decimator object
decim decim_create(unsigned int _d, float _fc, float _b, float _slsl)
{
    if (_d == 0) {
        printf("error: decim_create(), invalid decimation factor: 0\n");
        return NULL;
    } else if (_d > DECIM_FACTOR_MAX) {
        printf("error: decim_create(), decimation limit exceeded: %u\n", DECIM_FACTOR_MAX);
        return NULL;
    } else if (_d == 2) {
        return decim_create_halfband(_fc, _b, _slsl);
    } else {
        return decim_create_generic(_d, _fc, _b, _slsl);
    }

    // should never get to this point, but add this return statement
    // anyway to keep compiler happy
    return NULL;
}

decim decim_create_generic(unsigned int _D, float _fc, float _t, float _slsl)
{
    unsigned int h_len = estimate_req_filter_len(_t, _slsl);
    validate_filter_length(&h_len);

    decim d = (decim) malloc(sizeof(struct decim_s));
    d->h_len = h_len;
    d->h = (float*) malloc((d->h_len)*sizeof(float));

    d->D = _D;
    d->fc = _fc;
    d->t = _t;
    d->b = 1.0f / ((float)(d->D));
    d->slsl = _slsl;

    // use windowed sinc fir filter design
    fir_kaiser_window(d->h_len, d->b, d->slsl, d->h);

    d->branch = 0;

    // create buffer
    d->num_buffers = 1;
    d->buffers = (fbuffer*) malloc(1*sizeof(fbuffer));
    d->buffers[0] = fbuffer_create(CIRCULAR, d->h_len);
    // TODO: fill buffer with zeros

    return d;
}

decim decim_create_halfband(float _fc, float _t, float _slsl)
{
    unsigned int h_len = estimate_req_filter_len(_t, _slsl);
    validate_filter_length(&h_len);

    decim d = (decim) malloc(sizeof(struct decim_s));

    // ensure h_len = 2*n + 1, where n is even
    h_len += (h_len%2) ? 0 : 1;
    h_len += ((h_len-1)%4) ? 2 : 0;
    h_len -= (h_len>FIR_FILTER_LEN_MAX) ? 4 : 0;
    d->h_len = h_len;
    d->h = (float*) malloc((d->h_len)*sizeof(float));

    d->D = 2;
    d->fc = _fc;
    d->t = _t;
    d->b = 0.5f;
    d->slsl = _slsl;

    // use windowed sinc fir filter design
    fir_kaiser_window(d->h_len, d->b, d->slsl, d->h);

    d->branch = 0;

    // create buffers
    d->num_buffers = 2;
    d->buffers = (fbuffer*) malloc(2*sizeof(fbuffer));
    d->buffers[0] = fbuffer_create(CIRCULAR, d->h_len);
    d->buffers[1] = fbuffer_create(CIRCULAR, d->h_len);
    // TODO: fill buffer with zeros

    return d;
}

// Destroy decimator object
void decim_destroy(decim _d)
{
    unsigned int i;
    for (i=0; i<_d->num_buffers; i++)
        fbuffer_destroy(_d->buffers[i]);
    free(_d->buffers);
    free(_d->h);
    free(_d);
}


