/*
 * Copyright (c) 2007 - 2014 Joseph Gaeddert
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

#include <sys/resource.h>
#include "liquid.h"

// benchmark CVSD encoder
void benchmark_cvsd_encode(struct rusage *_start,
                           struct rusage *_finish,
                           unsigned long int *_num_iterations)
{
    unsigned long int i;

    // options
    unsigned int nbits=4;   // number of adjacent bits to observe
    float zeta=1.5f;        // slope adjustment multiplier
    float alpha = 0.95;     // pre-/post-filter coefficient
    char bit    = 0;        // output bit

    // create cvsd encoder
    cvsd encoder = cvsd_create(nbits, zeta, alpha);

    // input time series (random)
    float x[8] = {
       1.19403f,  -0.76765f,  -1.08415f,   0.65095f,
       0.11647f,  -0.80130f,  -0.87540f,  -0.14888f};

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        bit ^= cvsd_encode(encoder, x[0]);
        bit ^= cvsd_encode(encoder, x[2]);
        bit ^= cvsd_encode(encoder, x[3]);
        bit ^= cvsd_encode(encoder, x[3]);
        bit ^= cvsd_encode(encoder, x[4]);
        bit ^= cvsd_encode(encoder, x[5]);
        bit ^= cvsd_encode(encoder, x[6]);
        bit ^= cvsd_encode(encoder, x[7]);

        // randomize input
        x[i%8] += bit ? 0.1f : -0.1f;
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 8;

    // destroy cvsd encoder
    cvsd_destroy(encoder);
}

// benchmark CVSD decoder
void benchmark_cvsd_decode(struct rusage *_start,
                           struct rusage *_finish,
                           unsigned long int *_num_iterations)
{
    unsigned long int i;

    // options
    unsigned int nbits=4;   // number of adjacent bits to observe
    float zeta=1.5f;        // slope adjustment multiplier
    float alpha = 0.95;     // pre-/post-filter coefficient
    float x     = 0.0f;

    // create cvsd decoder
    cvsd decoder = cvsd_create(nbits, zeta, alpha);

    // input bit sequence (random)
    unsigned char b[8] = {1, 1, 1, 0, 1, 0, 0, 0};

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        x += cvsd_decode(decoder, b[0]);
        x += cvsd_decode(decoder, b[2]);
        x += cvsd_decode(decoder, b[3]);
        x += cvsd_decode(decoder, b[3]);
        x += cvsd_decode(decoder, b[4]);
        x += cvsd_decode(decoder, b[5]);
        x += cvsd_decode(decoder, b[6]);
        x += cvsd_decode(decoder, b[7]);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 8;

    // destroy cvsd decoder
    cvsd_destroy(decoder);
}

