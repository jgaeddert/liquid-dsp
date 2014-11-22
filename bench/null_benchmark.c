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

// null benchmark
void benchmark_null(struct rusage *_start,
                    struct rusage *_finish,
                    unsigned long int *_num_iterations)
{
    unsigned long int i;
    *_num_iterations *= 100;

    getrusage(RUSAGE_SELF, _start);
    unsigned int x = 0;
    for (i=0; i<*_num_iterations; i++) {
        // perform mindless task
        x <<= 1;
        x |= 1;
        x &= 0xff;
        x ^= 0xff;
    }
    getrusage(RUSAGE_SELF, _finish);
}

