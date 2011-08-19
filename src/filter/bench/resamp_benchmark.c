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

#include <sys/resource.h>
#include "liquid.h"

// Helper function to keep code base small
void resamp_crcf_bench(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations,
    unsigned int _h_len)
{
    unsigned long int i;
    float r = 1.03f;        // resampling rate
    float bw = 0.35f;       // filter bandwidth
    float As = 60.0f;       // stop-band attenuation [dB]
    unsigned int npfb = 32; // number of polyphase filters
    unsigned int h_len = _h_len / 2; // filter semi-length

    resamp_crcf q = resamp_crcf_create(r,h_len,bw,As,npfb);

    float complex x[] = {1.0f, -1.0f};
    float complex y[] = {1.0f, -1.0f};

    unsigned int num_written;

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        resamp_crcf_execute(q,x[0],y,&num_written);
        resamp_crcf_execute(q,x[1],y,&num_written);
        resamp_crcf_execute(q,x[0],y,&num_written);
        resamp_crcf_execute(q,x[1],y,&num_written);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;

    resamp_crcf_destroy(q);
}

#define RESAMP_CRCF_BENCHMARK_API(H_LEN)    \
(   struct rusage *_start,                  \
    struct rusage *_finish,                 \
    unsigned long int *_num_iterations)     \
{ resamp_crcf_bench(_start, _finish, _num_iterations, H_LEN); }

//
// Resampler benchmark prototypes
//
void benchmark_resamp_crcf_h3    RESAMP_CRCF_BENCHMARK_API(3)
void benchmark_resamp_crcf_h7    RESAMP_CRCF_BENCHMARK_API(7)
void benchmark_resamp_crcf_h13   RESAMP_CRCF_BENCHMARK_API(13)
void benchmark_resamp_crcf_h21   RESAMP_CRCF_BENCHMARK_API(21)
void benchmark_resamp_crcf_h37   RESAMP_CRCF_BENCHMARK_API(37)
void benchmark_resamp_crcf_h53   RESAMP_CRCF_BENCHMARK_API(53)

