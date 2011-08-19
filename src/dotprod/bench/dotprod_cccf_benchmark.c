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
#include "liquid.h"

// Helper function to keep code base small
void dotprod_cccf_bench(struct rusage *_start,
                        struct rusage *_finish,
                        unsigned long int *_num_iterations,
                        unsigned int _n)
{
    // normalize number of iterations
    *_num_iterations *= 4;
    *_num_iterations /= _n;
    if (*_num_iterations < 1) *_num_iterations = 1;

    float complex x[_n], h[_n], y[8];
    unsigned int i;
    for (i=0; i<_n; i++) {
        x[i] = randnf() + _Complex_I*randnf();
        h[i] = randnf() + _Complex_I*randnf();
    }

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        dotprod_cccf_run(h, x, _n, &y[0]);
        dotprod_cccf_run(h, x, _n, &y[1]);
        dotprod_cccf_run(h, x, _n, &y[2]);
        dotprod_cccf_run(h, x, _n, &y[3]);
        dotprod_cccf_run(h, x, _n, &y[4]);
        dotprod_cccf_run(h, x, _n, &y[5]);
        dotprod_cccf_run(h, x, _n, &y[6]);
        dotprod_cccf_run(h, x, _n, &y[7]);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 8;

}

#define DOTPROD_CCCF_BENCHMARK_API(N)   \
(   struct rusage *_start,              \
    struct rusage *_finish,             \
    unsigned long int *_num_iterations) \
{ dotprod_cccf_bench(_start, _finish, _num_iterations, N); }

void benchmark_dotprod_cccf_4      DOTPROD_CCCF_BENCHMARK_API(4)
void benchmark_dotprod_cccf_16     DOTPROD_CCCF_BENCHMARK_API(16)
void benchmark_dotprod_cccf_64     DOTPROD_CCCF_BENCHMARK_API(64)
void benchmark_dotprod_cccf_256    DOTPROD_CCCF_BENCHMARK_API(256)

