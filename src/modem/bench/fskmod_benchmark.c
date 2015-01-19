/*
 * Copyright (c) 2007 - 2015 Joseph Gaeddert
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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/resource.h>
#include "liquid.h"

#define FSKMOD_BENCH_API(m,k,bandwidth)     \
(   struct rusage *     _start,             \
    struct rusage *     _finish,            \
    unsigned long int * _num_iterations)    \
{ fskmod_bench(_start, _finish, _num_iterations, m, k, bandwidth); }

// Helper function to keep code base small
void fskmod_bench(struct rusage *     _start,
                  struct rusage *     _finish,
                  unsigned long int * _num_iterations,
                  unsigned int        _m,
                  unsigned int        _k,
                  float               _bandwidth)
{
    // normalize number of iterations
    *_num_iterations /= _k;

    if (*_num_iterations < 1) *_num_iterations = 1;

    // initialize modulator
    fskmod mod = fskmod_create(_m,_k,_bandwidth);

    unsigned int M = 1 << _m;   // constellation size
    float complex buf[_k];      // transmit buffer
    
    unsigned long int i;

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        fskmod_modulate(mod, (i+ 0) % M, buf);
        fskmod_modulate(mod, (i+ 1) % M, buf);
        fskmod_modulate(mod, (i+ 2) % M, buf);
        fskmod_modulate(mod, (i+ 3) % M, buf);
        fskmod_modulate(mod, (i+ 4) % M, buf);
        fskmod_modulate(mod, (i+ 5) % M, buf);
        fskmod_modulate(mod, (i+ 6) % M, buf);
        fskmod_modulate(mod, (i+ 7) % M, buf);
        fskmod_modulate(mod, (i+ 8) % M, buf);
        fskmod_modulate(mod, (i+ 9) % M, buf);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 10;

    fskmod_destroy(mod);
}

// BENCHMARKS: basic properties: M=2^m, k = 2*M, bandwidth = 0.25
void benchmark_fskmod_norm_M2      FSKMOD_BENCH_API( 1,    4, 0.25f    )
void benchmark_fskmod_norm_M4      FSKMOD_BENCH_API( 2,    8, 0.25f    )
void benchmark_fskmod_norm_M8      FSKMOD_BENCH_API( 3,   16, 0.25f    )
void benchmark_fskmod_norm_M16     FSKMOD_BENCH_API( 4,   32, 0.25f    )
void benchmark_fskmod_norm_M32     FSKMOD_BENCH_API( 5,   64, 0.25f    )
void benchmark_fskmod_norm_M64     FSKMOD_BENCH_API( 6,  128, 0.25f    )
void benchmark_fskmod_norm_M128    FSKMOD_BENCH_API( 7,  256, 0.25f    )
void benchmark_fskmod_norm_M256    FSKMOD_BENCH_API( 8,  512, 0.25f    )
void benchmark_fskmod_norm_M512    FSKMOD_BENCH_API( 9, 1024, 0.25f    )
void benchmark_fskmod_norm_M1024   FSKMOD_BENCH_API(10, 2048, 0.25f    )

// BENCHMARKS: obscure properties: M=2^m, k not relative to M, bandwidth basically irrational
void benchmark_fskmod_misc_M2      FSKMOD_BENCH_API( 1,    5, 0.3721451)
void benchmark_fskmod_misc_M4      FSKMOD_BENCH_API( 2,   10, 0.3721451)
void benchmark_fskmod_misc_M8      FSKMOD_BENCH_API( 3,   20, 0.3721451)
void benchmark_fskmod_misc_M16     FSKMOD_BENCH_API( 4,   30, 0.3721451)
void benchmark_fskmod_misc_M32     FSKMOD_BENCH_API( 5,   60, 0.3721451)
void benchmark_fskmod_misc_M64     FSKMOD_BENCH_API( 6,  100, 0.3721451)
void benchmark_fskmod_misc_M128    FSKMOD_BENCH_API( 7,  200, 0.3721451)
void benchmark_fskmod_misc_M256    FSKMOD_BENCH_API( 8,  500, 0.3721451)
void benchmark_fskmod_misc_M512    FSKMOD_BENCH_API( 9, 1000, 0.3721451)
void benchmark_fskmod_misc_M1024   FSKMOD_BENCH_API(10, 2000, 0.3721451)

