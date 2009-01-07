//
// 
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "channel_internal.h"

#include "../../random/src/random.h"

awgn_channel awgn_channel_create(float _nvar)
{
    awgn_channel q = (awgn_channel) malloc(sizeof(struct awgn_channel_s));

    awgn_channel_set_noise_variance(q,_nvar);

    return q;
}

void awgn_channel_destroy(awgn_channel _q)
{
    free(_q);
}

void awgn_channel_print(awgn_channel _q)
{
    printf("awgn channel: [noise var: %12.8fdB]\n", 10*log10f(_q->nvar));
}

void awgn_channel_execute(awgn_channel _q, float complex _x, float complex *_y)
{
    *_y = _x + crandnf() * (_q->nstd);
}

void awgn_channel_set_noise_variance(awgn_channel _q, float _nvar)
{
    if (_nvar < 0) {
        printf("error: awgn_channel_set_noise_variance(), variance is negative\n");
        exit(0);
    }

    _q->nvar = _nvar;
    _q->nstd = sqrtf(_q->nvar);
}

