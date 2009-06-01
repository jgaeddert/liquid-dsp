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

#ifndef __AGC_BENCHMARK_H__
#define __AGC_BENCHMARK_H__

#include <sys/resource.h>

#include "liquid.h"

void benchmark_agc(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations)
{
    unsigned int i;

    // initialize AGC object
    agc g = agc_create(1.0f, 0.05f);

    float complex x=1.0f, y;

    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        agc_execute(g, x, &y);
        agc_execute(g, x, &y);
        agc_execute(g, x, &y);
        agc_execute(g, x, &y);
    }
    getrusage(RUSAGE_SELF, _finish);

    *_num_iterations *= 4;

    agc_destroy(g);
}

#endif // __AGC_BENCHMARK_H__

