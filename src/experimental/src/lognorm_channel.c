/*
 * Copyright (c) 2010 Joseph Gaeddert
 * Copyright (c) 2010 Virginia Polytechnic Institute & State University
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
// Log-normal channel model
//
// References:
//  [Hara:1996] S. Hara, A. Ogino, M. Araki, M. Okada, and N. Morinaga,
//      "Throughput performance of SAW-ARQ protocol with adaptive packet
//      length in mobile packet data transmission," IEEE Transactions on
//      Vehicular Technology, vol. 45, no. 3, pp. 561–9, August 1996.

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.internal.h"

lognorm_channel lognorm_channel_create(unsigned int _h_len,
                                       float _sig_dB,
                                       float _fd)
{
    lognorm_channel q = (lognorm_channel) malloc(sizeof(struct lognorm_channel_s));

    if (_h_len == 0) {
        fprintf(stderr,"error: lognorm_channel_create(), filter length must be greater than zero\n");
        exit(1);
    } else if (_sig_dB < 0) {
        fprintf(stderr,"error: lognorm_channel_create(), sig (%f) is negative\n", _sig_dB);
        exit(1);
    } else if (_fd < 0) {
        fprintf(stderr,"error: lognorm_channel_create()< fd (%f) is negative\n", _fd);
        exit(1);
    }

    q->sig = _sig_dB;
    q->fd = _fd;
    q->h_len = _h_len;
    q->h_len = (q->h_len < 5) ? 5 : q->h_len;
    //q->h_len += (q->h_len) % 2 ? 1 : 0;

    // create filter
    //
    // autocorrelation function:
    //  p(t) = exp{ -0.0101 fd lambda t }
    // where 'fd' is the Doppler frequency and 'lambda' is the carrier
    // wavelength [Hara:1996]
    float h[q->h_len];
    unsigned int i;
    for (i=0; i<q->h_len; i++)
        h[i] = i==0 ? 1.0f : 0.0f;

    // normalize filter
    float t=0.0f;
    for (i=0; i<q->h_len; i++)
        t += h[i]*h[i];
    t = sqrtf(t);
    for (i=0; i<q->h_len; i++)
        h[i] /= t;

    // create filter
    q->f = firfilt_rrrf_create(h, q->h_len);

    // load with complex values
    for (i=0; i<q->h_len; i++)
        firfilt_rrrf_push(q->f, 1.0f);

    return q;
}

void lognorm_channel_destroy(lognorm_channel _q)
{
    firfilt_rrrf_destroy(_q->f);
    free(_q);
}

void lognorm_channel_print(lognorm_channel _q)
{
    printf("Log-normal channel:\n");
}

void lognorm_channel_execute(lognorm_channel _q,
                             float complex _x,
                             float complex *_y)
{
    float r;

    // advance fading filter
    firfilt_rrrf_push(_q->f, 1.0f);
    firfilt_rrrf_execute(_q->f, &r);

    // set return value
    *_y = _x * r;
}

