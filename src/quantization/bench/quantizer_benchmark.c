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
void benchmark_quantize_adc(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations)
{
    unsigned long int i;

    unsigned int q;
    unsigned int num_bits=8;
    float x=-0.1f;

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        q = quantize_adc(x,num_bits);
        q = quantize_adc(x,num_bits);
        q = quantize_adc(x,num_bits);
        q = quantize_adc(x,num_bits);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;
}

// 
void benchmark_quantize_dac(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations)
{
    unsigned long int i;

    unsigned int q=0x0f;
    unsigned int num_bits=8;
    float x;

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        x = quantize_dac(q,num_bits);
        x = quantize_dac(q,num_bits);
        x = quantize_dac(q,num_bits);
        x = quantize_dac(q,num_bits);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;
}

