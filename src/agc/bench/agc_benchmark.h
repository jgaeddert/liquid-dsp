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

#ifndef __AGC_BENCHMARK_H__
#define __AGC_BENCHMARK_H__

#include <sys/resource.h>

#include "liquid.h"

// helper function to keep code base small
void agc_crcf_unlocked_bench(struct rusage *_start,
                             struct rusage *_finish,
                             unsigned long int *_num_iterations,
                             liquid_agc_type _type,
                             unsigned int _D,
                             int _squelch)
{
    unsigned int i;

    // initialize AGC object
    agc_crcf g = agc_crcf_create();
    agc_crcf_set_type(g,_type);
    agc_crcf_set_target(g,1.0f);
    agc_crcf_set_decim(g,_D);
    agc_crcf_set_bandwidth(g,0.05f);

    // squelch
    if (_squelch) agc_crcf_squelch_activate(g);
    else          agc_crcf_squelch_deactivate(g);

    float complex x=1.0f, y;

    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        agc_crcf_execute(g, x, &y);
        agc_crcf_execute(g, x, &y);
        agc_crcf_execute(g, x, &y);
        agc_crcf_execute(g, x, &y);
    }
    getrusage(RUSAGE_SELF, _finish);

    *_num_iterations *= 4;

    agc_crcf_destroy(g);
}

#define AGC_CRCF_BENCHMARK_API(TYPE,D,SQUELCH)  \
(   struct rusage *_start,                      \
    struct rusage *_finish,                     \
    unsigned long int *_num_iterations)         \
{ agc_crcf_unlocked_bench(_start, _finish, _num_iterations, TYPE, D, SQUELCH); }

// default agc type
void benchmark_agc_crcf_default         AGC_CRCF_BENCHMARK_API(LIQUID_AGC_DEFAULT,  1, 0)
void benchmark_agc_crcf_default_D4      AGC_CRCF_BENCHMARK_API(LIQUID_AGC_DEFAULT,  4, 0)

// default agc type with squelch enabled
void benchmark_agc_crcf_default_sq      AGC_CRCF_BENCHMARK_API(LIQUID_AGC_DEFAULT,  1, 1)
void benchmark_agc_crcf_default_sq_D4   AGC_CRCF_BENCHMARK_API(LIQUID_AGC_DEFAULT,  4, 1)

// logarithmic agc type
void benchmark_agc_crcf_log             AGC_CRCF_BENCHMARK_API(LIQUID_AGC_LOG,      1, 0)
void benchmark_agc_crcf_log_D4          AGC_CRCF_BENCHMARK_API(LIQUID_AGC_LOG,      4, 0)

// exponential agc type
void benchmark_agc_crcf_exp             AGC_CRCF_BENCHMARK_API(LIQUID_AGC_EXP,      1, 0)
void benchmark_agc_crcf_exp_D4          AGC_CRCF_BENCHMARK_API(LIQUID_AGC_EXP,      4, 0)

// locked AGC
void benchmark_agc_crcf_locked(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations)
{
    unsigned int i;

    // initialize AGC object
    agc_crcf g = agc_crcf_create();
    agc_crcf_set_target(g,1.0f);
    agc_crcf_set_bandwidth(g,0.05f);
    agc_crcf_lock(g);

    float complex x=1.0f, y;

    getrusage(RUSAGE_SELF, _start);
    *_num_iterations *= 4;
    for (i=0; i<(*_num_iterations); i++) {
        agc_crcf_execute(g, x, &y);
        agc_crcf_execute(g, x, &y);
        agc_crcf_execute(g, x, &y);
        agc_crcf_execute(g, x, &y);
    }
    getrusage(RUSAGE_SELF, _finish);

    *_num_iterations *= 4;

    agc_crcf_destroy(g);
}

#endif // __AGC_BENCHMARK_H__

