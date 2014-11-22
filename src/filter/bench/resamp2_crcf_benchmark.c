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

typedef enum {
    RESAMP2_DECIM,
    RESAMP2_INTERP
} resamp2_type;

// Helper function to keep code base small
void resamp2_crcf_bench(struct rusage *_start,
                        struct rusage *_finish,
                        unsigned long int * _num_iterations,
                        unsigned int _m,
                        resamp2_type _type)
{
    // scale number of iterations by filter length
    // NOTE: n = 4*m+1
    // cycles/trial ~ 70.5 + 7.74*_m
    *_num_iterations *= 200;
    *_num_iterations /= 70.5 + 7.74*_m;

    unsigned long int i;

    resamp2_crcf q = resamp2_crcf_create(_m,0.0f,60.0f);

    float complex x[] = {1.0f, -1.0f};
    float complex y[] = {1.0f, -1.0f};

    // start trials
    getrusage(RUSAGE_SELF, _start);
    if (_type == RESAMP2_DECIM) {

        // run decimator
        for (i=0; i<(*_num_iterations); i++) {
            resamp2_crcf_decim_execute(q,x,y);
            resamp2_crcf_decim_execute(q,x,y);
            resamp2_crcf_decim_execute(q,x,y);
            resamp2_crcf_decim_execute(q,x,y);
        }
    } else {

        // run interpolator
        for (i=0; i<(*_num_iterations); i++) {
            resamp2_crcf_interp_execute(q,x[0],y);
            resamp2_crcf_interp_execute(q,x[0],y);
            resamp2_crcf_interp_execute(q,x[0],y);
            resamp2_crcf_interp_execute(q,x[0],y);
        }
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;

    resamp2_crcf_destroy(q);
}

#define RESAMP2_CRCF_BENCHMARK_API(M,T) \
(   struct rusage *_start,              \
    struct rusage *_finish,             \
    unsigned long int *_num_iterations) \
{ resamp2_crcf_bench(_start, _finish, _num_iterations, M, T); }

//
// Decimators
//
void benchmark_resamp2_crcf_decim_m2    RESAMP2_CRCF_BENCHMARK_API( 2,RESAMP2_DECIM)  // n=9
void benchmark_resamp2_crcf_decim_m4    RESAMP2_CRCF_BENCHMARK_API( 4,RESAMP2_DECIM)  // n=17
void benchmark_resamp2_crcf_decim_m8    RESAMP2_CRCF_BENCHMARK_API( 8,RESAMP2_DECIM)  // n=33
void benchmark_resamp2_crcf_decim_m16   RESAMP2_CRCF_BENCHMARK_API(16,RESAMP2_DECIM)  // n=65

// 
// Interpolators
//
void benchmark_resamp2_crcf_interp_m2   RESAMP2_CRCF_BENCHMARK_API( 2,RESAMP2_INTERP) // n=9
void benchmark_resamp2_crcf_interp_m4   RESAMP2_CRCF_BENCHMARK_API( 4,RESAMP2_INTERP) // n=17
void benchmark_resamp2_crcf_interp_m8   RESAMP2_CRCF_BENCHMARK_API( 8,RESAMP2_INTERP) // n=33
void benchmark_resamp2_crcf_interp_m16  RESAMP2_CRCF_BENCHMARK_API(16,RESAMP2_INTERP) // n=65

