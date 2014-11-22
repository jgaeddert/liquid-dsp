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

// helper function to keep code base small
void benchmark_agc_crcf(struct rusage *     _start,
                        struct rusage *     _finish,
                        unsigned long int * _num_iterations)
{
    unsigned int i;

    // initialize AGC object
    agc_crcf q = agc_crcf_create();
    agc_crcf_set_bandwidth(q,0.05f);

    float complex x = 1e-6f;    // input sample
    float complex y;            // output sample

    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        agc_crcf_execute(q, x, &y);
        agc_crcf_execute(q, x, &y);
        agc_crcf_execute(q, x, &y);
        agc_crcf_execute(q, x, &y);
        agc_crcf_execute(q, x, &y);
        agc_crcf_execute(q, x, &y);
        agc_crcf_execute(q, x, &y);
        agc_crcf_execute(q, x, &y);
    }
    getrusage(RUSAGE_SELF, _finish);

    *_num_iterations *= 8;

    // destroy object
    agc_crcf_destroy(q);
}

