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

decim decim_create_generic(unsigned int _d, float _fc, float _b, float _slsl)
{
    unsigned int h_len = estimate_req_filter_len(_b, _slsl);
    validate_filter_length(&h_len);

    decim d = (decim) malloc(sizeof(struct decim_s));
    d->h_len = h_len;
    d->h = (float*) malloc((d->h_len)*sizeof(float));

    d->fc = _fc;
    d->b = _b;
    d->t = 1.0f / ((float)(d->D));
    d->slsl = _slsl;

    // use windowed sinc fir filter design
    fir_kaiser_window(d->h_len, d->b, d->slsl, d->h);

    return d;
}

decim decim_create_halfband(float _fc, float _b, float _slsl)
{
    unsigned int h_len = estimate_req_filter_len(_b, _slsl);
    validate_filter_length(&h_len);

    decim d = (decim) malloc(sizeof(struct decim_s));
    d->h_len = h_len;
    d->h = (float*) malloc((d->h_len)*sizeof(float));

    // use windowed sinc fir filter design
    //fir_design_windowed_sinc(d->h, d->h_len);

    d->fc = _fc;
    d->b = _b;

    return d;
}

// Destroy decimator object
void decim_destroy(decim _d)
{
    free(_d->h);
    free(_d);
}


