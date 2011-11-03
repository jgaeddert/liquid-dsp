/*
 * Copyright (c) 2007, 2008, 2009, 2010, 2011 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010, 2011  Virginia Polytechnic
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

#include <sys/resource.h>

#include "liquid.h"

// helper function to keep code base small
void agc_crcf_unlocked_bench(struct rusage *_start,
                             struct rusage *_finish,
                             unsigned long int *_num_iterations,
                             int _squelch,
                             int _locked)
{
    // scale if locked
    if (_locked) *_num_iterations *= 8;

    unsigned int i;

    // initialize AGC object
    agc_crcf q = agc_crcf_create();
    agc_crcf_set_bandwidth(q,0.05f);

    // squelch?
    if (_squelch) agc_crcf_squelch_activate(q);
    else          agc_crcf_squelch_deactivate(q);

    // locked?
    if (_locked)  agc_crcf_lock(q);
    else          agc_crcf_unlock(q);

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

#define AGC_CRCF_BENCHMARK_API(SQUELCH,LOCKED)  \
(   struct rusage *_start,                      \
    struct rusage *_finish,                     \
    unsigned long int *_num_iterations)         \
{ agc_crcf_unlocked_bench(_start, _finish, _num_iterations, SQUELCH, LOCKED); }

void benchmark_agc_crcf                 AGC_CRCF_BENCHMARK_API(0, 0)
void benchmark_agc_crcf_squelch         AGC_CRCF_BENCHMARK_API(1, 0)
void benchmark_agc_crcf_locked          AGC_CRCF_BENCHMARK_API(0, 1)

