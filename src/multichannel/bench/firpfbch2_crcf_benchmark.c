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

#define FIRPFBCH2_EXECUTE_BENCH_API(NUM_CHANNELS,M,TYPE)    \
(   struct rusage *_start,                                  \
    struct rusage *_finish,                                 \
    unsigned long int *_num_iterations)                     \
{ firpfbch2_crcf_execute_bench(_start, _finish, _num_iterations, NUM_CHANNELS, M, TYPE); }

// Helper function to keep code base small
void firpfbch2_crcf_execute_bench(struct rusage *     _start,
                                  struct rusage *     _finish,
                                  unsigned long int * _num_iterations,
                                  unsigned int        _num_channels,
                                  unsigned int        _m,
                                  int                 _type)
{
    // initialize channelizer
    float As         = 60.0f;
    firpfbch2_crcf q = firpfbch2_crcf_create_kaiser(_type,_num_channels,_m,As);

    unsigned long int i;

    float complex x[_num_channels];
    float complex y[_num_channels];
    for (i=0; i<_num_channels; i++)
        x[i] = 1.0f + _Complex_I*1.0f;

    // scale number of iterations to keep execution time
    // relatively linear
    *_num_iterations /= _num_channels;

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        firpfbch2_crcf_execute(q, x, y);
        firpfbch2_crcf_execute(q, x, y);
        firpfbch2_crcf_execute(q, x, y);
        firpfbch2_crcf_execute(q, x, y);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;

    firpfbch2_crcf_destroy(q);
}

// analysis
void benchmark_firpfbch2_crcf_a4    FIRPFBCH2_EXECUTE_BENCH_API(4,    2,  LIQUID_ANALYZER)
void benchmark_firpfbch2_crcf_a16   FIRPFBCH2_EXECUTE_BENCH_API(16,   2,  LIQUID_ANALYZER)
void benchmark_firpfbch2_crcf_a64   FIRPFBCH2_EXECUTE_BENCH_API(64,   2,  LIQUID_ANALYZER)
void benchmark_firpfbch2_crcf_a256  FIRPFBCH2_EXECUTE_BENCH_API(256,  2,  LIQUID_ANALYZER)
void benchmark_firpfbch2_crcf_a512  FIRPFBCH2_EXECUTE_BENCH_API(512,  2,  LIQUID_ANALYZER)
void benchmark_firpfbch2_crcf_a1024 FIRPFBCH2_EXECUTE_BENCH_API(1024, 2,  LIQUID_ANALYZER)

// synthesis
void benchmark_firpfbch2_crcf_s4    FIRPFBCH2_EXECUTE_BENCH_API(4,    2,  LIQUID_SYNTHESIZER)
void benchmark_firpfbch2_crcf_s16   FIRPFBCH2_EXECUTE_BENCH_API(16,   2,  LIQUID_SYNTHESIZER)
void benchmark_firpfbch2_crcf_s64   FIRPFBCH2_EXECUTE_BENCH_API(64,   2,  LIQUID_SYNTHESIZER)
void benchmark_firpfbch2_crcf_s256  FIRPFBCH2_EXECUTE_BENCH_API(256,  2,  LIQUID_SYNTHESIZER)
void benchmark_firpfbch2_crcf_s512  FIRPFBCH2_EXECUTE_BENCH_API(512,  2,  LIQUID_SYNTHESIZER)
void benchmark_firpfbch2_crcf_s1024 FIRPFBCH2_EXECUTE_BENCH_API(1024, 2,  LIQUID_SYNTHESIZER)


