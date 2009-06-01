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

#ifndef __LIQUID_RESAMP2_BENCHMARK_H__
#define __LIQUID_RESAMP2_BENCHMARK_H__

#include <sys/resource.h>
#include "liquid.h"

typedef enum {
    RESAMP2_DECIM,
    RESAMP2_INTERP
} resamp2_type;

// Helper function to keep code base small
void resamp2_cccf_bench(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations,
    unsigned int _h_len,
    resamp2_type _type)
{
    float h[_h_len];
    unsigned int i;
    for (i=0; i<_h_len; i++)
        h[i] = 1.0f;

    resamp2_cccf q = resamp2_cccf_create(_h_len);

    float complex x[] = {1.0f, -1.0f};
    float complex y[] = {1.0f, -1.0f};

    // start trials
    getrusage(RUSAGE_SELF, _start);
    if (_type == RESAMP2_DECIM) {

        // run decimator
        for (i=0; i<(*_num_iterations); i++) {
            resamp2_cccf_decim_execute(q,x,y);
            resamp2_cccf_decim_execute(q,x,y);
            resamp2_cccf_decim_execute(q,x,y);
            resamp2_cccf_decim_execute(q,x,y);
        }
    } else {

        // run interpolator
        for (i=0; i<(*_num_iterations); i++) {
            resamp2_cccf_interp_execute(q,x[0],y);
            resamp2_cccf_interp_execute(q,x[0],y);
            resamp2_cccf_interp_execute(q,x[0],y);
            resamp2_cccf_interp_execute(q,x[0],y);
        }
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;

    resamp2_cccf_destroy(q);
}

#define RESAMP2_CCCF_BENCHMARK_API(H_LEN,T) \
(   struct rusage *_start,                  \
    struct rusage *_finish,                 \
    unsigned long int *_num_iterations)     \
{ resamp2_cccf_bench(_start, _finish, _num_iterations, H_LEN, T); }

//
// Decimators
//
void benchmark_resamp2_cccf_decim_h13   RESAMP2_CCCF_BENCHMARK_API(13,RESAMP2_DECIM)    // m=3
void benchmark_resamp2_cccf_decim_h21   RESAMP2_CCCF_BENCHMARK_API(21,RESAMP2_DECIM)    // m=5
void benchmark_resamp2_cccf_decim_h37   RESAMP2_CCCF_BENCHMARK_API(37,RESAMP2_DECIM)    // m=9
void benchmark_resamp2_cccf_decim_h53   RESAMP2_CCCF_BENCHMARK_API(53,RESAMP2_DECIM)    // m=13

// 
// Interpolators
//
void benchmark_resamp2_cccf_interp_h13  RESAMP2_CCCF_BENCHMARK_API(13,RESAMP2_INTERP)   // m=3
void benchmark_resamp2_cccf_interp_h21  RESAMP2_CCCF_BENCHMARK_API(21,RESAMP2_INTERP)   // m=5
void benchmark_resamp2_cccf_interp_h37  RESAMP2_CCCF_BENCHMARK_API(37,RESAMP2_INTERP)   // m=9
void benchmark_resamp2_cccf_interp_h53  RESAMP2_CCCF_BENCHMARK_API(53,RESAMP2_INTERP)   // m=13

#endif // __LIQUID_RESAMP2_BENCHMARK_H__

