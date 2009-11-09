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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "liquid.internal.h"

#if HAVE_FFTW3_H
#   include <fftw3.h>
#endif

#define DEBUG_OFDMOQAMFRAME64GEN            1

struct ofdmoqamframe64gen_s {
    unsigned int num_subcarriers;

};

ofdmoqamframe64gen ofdmoqamframe64gen_create()
{
    ofdmoqamframe64gen q = (ofdmoqamframe64gen) malloc(sizeof(struct ofdmoqamframe64gen_s));

    return q;
}

void ofdmoqamframe64gen_destroy(ofdmoqamframe64gen _q)
{
    free(_q);
}

void ofdmoqamframe64gen_print(ofdmoqamframe64gen _q)
{
    printf("ofdmoqamframe64gen:\n");
    printf("    num subcarriers     :   %u\n", _q->num_subcarriers);
}

void ofdmoqamframe64gen_reset(ofdmoqamframe64gen _q)
{
}

void ofdmoqamframe64gen_writeshortsequence(ofdmoqamframe64gen _q,
                                       float complex * _y)
{
}


void ofdmoqamframe64gen_writelongsequence(ofdmoqamframe64gen _q,
                                      float complex * _y)
{
}

void ofdmoqamframe64gen_writeheader(ofdmoqamframe64gen _q,
                                float complex * _y)
{
}

void ofdmoqamframe64gen_writesymbol(ofdmoqamframe64gen _q,
                                float complex * _x,
                                float complex * _y)
{
}

