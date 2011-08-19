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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/resource.h>
#include "liquid.experimental.h"

// Helper function to keep code base small
void fbasc_bench(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations,
    unsigned int _num_channels,
    unsigned int _samples_per_frame,
    unsigned int _bytes_per_frame,
    int _type)
{
    unsigned long int i;
    *_num_iterations /= log2f(_num_channels) * _samples_per_frame;

    // create fbasc codecs
    fbasc encoder = fbasc_create(FBASC_ENCODER, _num_channels, _samples_per_frame, _bytes_per_frame);
    fbasc decoder = fbasc_create(FBASC_DECODER, _num_channels, _samples_per_frame, _bytes_per_frame);

    // derived values
    unsigned int num_samples = _samples_per_frame;
    unsigned int header_len = fbasc_compute_header_length(_num_channels,
                                                          _samples_per_frame,
                                                          _bytes_per_frame);

    // buffers
    float x[num_samples];
    //float y[num_samples];
    unsigned char header[header_len];
    unsigned char frame[_bytes_per_frame];

    // initialize x, run encoder once
    for (i=0; i<num_samples; i++)
        x[i] = cosf(2*M_PI*0.01f*i) + randnf()*0.1f;
    fbasc_encode(encoder,x,header,frame);

    // start trials
    getrusage(RUSAGE_SELF, _start);
    if (_type == FBASC_ENCODER) {
        for (i=0; i<(*_num_iterations); i++)
            fbasc_encode(encoder,x,header,frame);
    } else {
        for (i=0; i<(*_num_iterations); i++)
            fbasc_decode(decoder,header,frame,x);
    }
    getrusage(RUSAGE_SELF, _finish);

    fbasc_destroy(encoder);
    fbasc_destroy(decoder);
}

#define FBASC_BENCHMARK_API(N,S,B,T)            \
(   struct rusage *_start,                      \
    struct rusage *_finish,                     \
    unsigned long int *_num_iterations)         \
{ fbasc_bench(_start, _finish, _num_iterations, N,S,B,T); }

//
// fbasc benchmark prototypes
//
void benchmark_fbasc_encoder_n64_s128   FBASC_BENCHMARK_API(64,  128, 100, FBASC_ENCODER);
void benchmark_fbasc_encoder_n64_s256   FBASC_BENCHMARK_API(64,  256, 100, FBASC_ENCODER);
void benchmark_fbasc_encoder_n64_s512   FBASC_BENCHMARK_API(64,  512, 100, FBASC_ENCODER);

void benchmark_fbasc_decoder_n64_s128   FBASC_BENCHMARK_API(64,  128, 100, FBASC_DECODER);
void benchmark_fbasc_decoder_n64_s256   FBASC_BENCHMARK_API(64,  256, 100, FBASC_DECODER);
void benchmark_fbasc_decoder_n64_s512   FBASC_BENCHMARK_API(64,  512, 100, FBASC_DECODER);

