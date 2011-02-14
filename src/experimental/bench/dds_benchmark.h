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

#include <sys/resource.h>
#include "liquid.experimental.h"

typedef enum {
    LIQUID_DDS_BENCHMARK_DECIM,     // decimator (down-converter)
    LIQUID_DDS_BENCHMARK_INTERP     // interpolator (up-converter)
} liquid_dds_benchmark_type;

// Helper function to keep code base small
void dds_crcf_bench(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations,
    unsigned int _num_stages,
    float _As,
    float _bw,
    liquid_dds_benchmark_type _type)
{
    unsigned long int i;

    // normalize number of iterations
    *_num_iterations /= 1<<_num_stages;
    if (*_num_iterations < 1) *_num_iterations = 1;

    // up-converted center frequency
    float fc = 0.0f;

    // create dds object
    dds_cccf q = dds_cccf_create(_num_stages,
                                 fc,
                                 _bw,
                                 _As);

    // initialize buffers
    unsigned int n = 1<<_num_stages;
    float complex x = 1.0f;
    float complex y[n];
    for (i=0; i<n; i++) y[i] = 1.0f;

    // start trials
    getrusage(RUSAGE_SELF, _start);
    if (_type == LIQUID_DDS_BENCHMARK_INTERP) {

        // run interpolator / up-converter
        for (i=0; i<(*_num_iterations); i++) {
            dds_cccf_interp_execute(q,x,y);
            dds_cccf_interp_execute(q,x,y);
            dds_cccf_interp_execute(q,x,y);
            dds_cccf_interp_execute(q,x,y);
        }
    } else {

        // run decimator / down-converter
        for (i=0; i<(*_num_iterations); i++) {
            dds_cccf_decim_execute(q,y,&x);
            dds_cccf_decim_execute(q,y,&x);
            dds_cccf_decim_execute(q,y,&x);
            dds_cccf_decim_execute(q,y,&x);
        }
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;

    // clean up dds memory object
    dds_cccf_destroy(q);
}

#define LIQUID_DDS_CCCF_BENCHMARK_API(NUM_STAGES,AS,BW,TYPE)    \
(   struct rusage *_start,                                      \
    struct rusage *_finish,                                     \
    unsigned long int *_num_iterations)                         \
{ dds_crcf_bench(_start, _finish, _num_iterations, NUM_STAGES, AS, BW, TYPE); }

//
// Decimators
//
void benchmark_dds_cccf_decim_n2    LIQUID_DDS_CCCF_BENCHMARK_API(1,60.0f,0.5f,LIQUID_DDS_BENCHMARK_DECIM)
void benchmark_dds_cccf_decim_n4    LIQUID_DDS_CCCF_BENCHMARK_API(2,60.0f,0.5f,LIQUID_DDS_BENCHMARK_DECIM)
void benchmark_dds_cccf_decim_n8    LIQUID_DDS_CCCF_BENCHMARK_API(3,60.0f,0.5f,LIQUID_DDS_BENCHMARK_DECIM)
void benchmark_dds_cccf_decim_n16   LIQUID_DDS_CCCF_BENCHMARK_API(4,60.0f,0.5f,LIQUID_DDS_BENCHMARK_DECIM)

// 
// Interpolators
//
void benchmark_dds_cccf_interp_n2   LIQUID_DDS_CCCF_BENCHMARK_API(1,60.0f,0.5f,LIQUID_DDS_BENCHMARK_INTERP)
void benchmark_dds_cccf_interp_n4   LIQUID_DDS_CCCF_BENCHMARK_API(2,60.0f,0.5f,LIQUID_DDS_BENCHMARK_INTERP)
void benchmark_dds_cccf_interp_n8   LIQUID_DDS_CCCF_BENCHMARK_API(3,60.0f,0.5f,LIQUID_DDS_BENCHMARK_INTERP)
void benchmark_dds_cccf_interp_n16  LIQUID_DDS_CCCF_BENCHMARK_API(4,60.0f,0.5f,LIQUID_DDS_BENCHMARK_INTERP)


