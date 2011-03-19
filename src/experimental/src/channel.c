/*
 * Copyright (c) 2007, 2008, 2009, 2010 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010 Virginia Polytechnic
 *                                      Institute & State University
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
// Finite impulse response filter
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "liquid.internal.h"

channel channel_create()
{
    channel q = (channel) malloc(sizeof(struct channel_s));

    // generate fading filter
    //q->f_ricek = cfirfilt_create();
    q->f_ricek = NULL;

    // generate shadowing filter
    //q->f_lognorm = firfilt_create();
    q->f_lognorm = NULL;

    return q;
}

void channel_destroy(channel _q)
{
    firfilt_cccf_destroy(_q->f_ricek);
    firfilt_rrrf_destroy(_q->f_lognorm);
    free(_q);
}

void channel_print(channel _q)
{

}

void channel_execute(channel _q, float complex _x, float complex *_y)
{
    float complex r, x, y;
    float z;

    // advance fading filter
    r = icrandnf();
    firfilt_cccf_push(_q->f_ricek, r);
    firfilt_cccf_execute(_q->f_ricek, &x);

    // TODO: compensate for filter

    // generate complex fading envelope
    y = _Complex_I*( crealf(x)*(_q->sig) + _q->s ) +
                   ( cimagf(x)*(_q->sig)         );

    // advance shadowing filter
    r = randnf();
    firfilt_rrrf_push(_q->f_lognorm, r);
    firfilt_rrrf_execute(_q->f_lognorm, &z);


    // additive white gaussian noise
    float complex n;
    n = icrandnf();  // TODO: noise power?

    // set return value
    *_y = (_x * y * z) + n;
}

