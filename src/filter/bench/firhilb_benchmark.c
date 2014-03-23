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

// Helper function to keep code base small
void firhilbf_decim_bench(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations,
    unsigned int _m)
{
    // normalize number of trials
    *_num_iterations *= 20;
    *_num_iterations /= liquid_nextpow2(_m+1);

    // create hilber transform object
    firhilbf q = firhilbf_create(_m,60.0f);

    float x[] = {1.0f, -1.0f};
    float complex y;
    unsigned long int i;

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        firhilbf_decim_execute(q,x,&y);
        firhilbf_decim_execute(q,x,&y);
        firhilbf_decim_execute(q,x,&y);
        firhilbf_decim_execute(q,x,&y);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;

    firhilbf_destroy(q);
}

#define FIRHILB_DECIM_BENCHMARK_API(M)  \
(   struct rusage *_start,              \
    struct rusage *_finish,             \
    unsigned long int *_num_iterations) \
{ firhilbf_decim_bench(_start, _finish, _num_iterations, M); }

void benchmark_firhilbf_decim_m3    FIRHILB_DECIM_BENCHMARK_API(3)  // m=3
void benchmark_firhilbf_decim_m5    FIRHILB_DECIM_BENCHMARK_API(5)  // m=5
void benchmark_firhilbf_decim_m9    FIRHILB_DECIM_BENCHMARK_API(9)  // m=9
void benchmark_firhilbf_decim_m13   FIRHILB_DECIM_BENCHMARK_API(13) // m=13

