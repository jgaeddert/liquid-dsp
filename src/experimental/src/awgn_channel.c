/*
 * Copyright (c) 2007, 2009 Joseph Gaeddert
 * Copyright (c) 2007, 2009 Virginia Polytechnic Institute & State University
 *
 * This file is part of liquid.
 *
 * liquid is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * liquid is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with liquid.  If not, see <http://www.gnu.org/licenses/>.
 */

//
// 
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.internal.h"

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
    *_y = _x + icrandnf() * (_q->nstd);
}

void awgn_channel_set_noise_variance(awgn_channel _q, float _nvar)
{
    if (_nvar < 0) {
        fprintf(stderr,"error: awgn_channel_set_noise_variance(), variance is negative\n");
        exit(1);
    }

    _q->nvar = _nvar;
    _q->nstd = sqrtf(_q->nvar);
}

