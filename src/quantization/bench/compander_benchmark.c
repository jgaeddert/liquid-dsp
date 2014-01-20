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

// 
void benchmark_compress_mulaw(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations)
{
    unsigned long int i;

    float x = -0.1f;
    float mu = 255.0f;
    float y;

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        y = compress_mulaw(x,mu);
        y = compress_mulaw(x,mu);
        y = compress_mulaw(x,mu);
        y = compress_mulaw(x,mu);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;
}

// 
void benchmark_expand_mulaw(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations)
{
    unsigned long int i;

    float x;
    float mu = 255.0f;
    float y = 0.75f;

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        x = expand_mulaw(y,mu);
        x = expand_mulaw(y,mu);
        x = expand_mulaw(y,mu);
        x = expand_mulaw(y,mu);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;
}

