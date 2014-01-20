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

//
// Example of a benchmark header
//

#include <sys/resource.h>
#include <math.h>

// strings parsed by benchmarkgen.py
const char * mybench_opts[3] = {
    "opt1a opt1b",
    "opt2a opt2b opt2c",
    "opt3a opt3b opt3c"
};


void benchmark_mybench(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations)
//    unsigned int argc,
//    char *argv[])
{
    // DSP initiazation goes here

    unsigned int i;
    float x, y, theta;
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        // DSP execution goes here
        x = cosf(M_PI/2.0f);
        y = sinf(M_PI/2.0f);
        theta = atan2(y,x);
    }
    getrusage(RUSAGE_SELF, _finish);

    // DSP cleanup goes here
}


