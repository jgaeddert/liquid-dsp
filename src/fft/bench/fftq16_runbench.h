/*
 * Copyright (c) 2012 Joseph Gaeddert
 * Copyright (c) 2012 Virginia Polytechnic Institute & State University
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

//
// fftq16_runbench.h : benchmark execution program declaration (fixed-point)
//

#ifndef __FFTQ16_RUNBENCH_H__
#define __FFTQ16_RUNBENCH_H__

#include <sys/resource.h>

#define FFTQ16_BENCHMARK_API(NFFT,D)    \
(   struct rusage *_start,              \
    struct rusage *_finish,             \
    unsigned long int *_num_iterations) \
{ fftq16_runbench(_start, _finish, _num_iterations, NFFT, D); }

// Helper function to keep code base small
void fftq16_runbench(struct rusage *     _start,
                     struct rusage *     _finish,
                     unsigned long int * _num_iterations,
                     unsigned int        _nfft,
                     int                 _direction);

#endif // __FFTQ16_RUNBENCH_H__

