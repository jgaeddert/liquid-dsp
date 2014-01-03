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

// compile and run:
//   $ gcc -Wall -I. -I./include -c newbench_example.c -o newbench_example.o 
//   $ gcc newbench_example.o libliquid.a -lm -lfec -lpthread -lc -lfftw3f -o newbench_example 
//   $ ./newbench_example

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <getopt.h>
#include <math.h>
#include "include/liquid.h"

double calculate_execution_time(struct rusage _start, struct rusage _finish)
{
    return _finish.ru_utime.tv_sec - _start.ru_utime.tv_sec
        + 1e-6*(_finish.ru_utime.tv_usec - _start.ru_utime.tv_usec)
        + _finish.ru_stime.tv_sec - _start.ru_stime.tv_sec
        + 1e-6*(_finish.ru_stime.tv_usec - _start.ru_stime.tv_usec);
}

struct decim_crcf_opts {
    unsigned int n; // filter length
    unsigned int D; // decimation factor
};

void benchmark_decim_crcf(
    void * _opts,
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations)
{
    // retrieve options
    struct decim_crcf_opts * opts = (struct decim_crcf_opts*) _opts;

    unsigned long int i;
    // DSP initiazation goes here
    float h[opts->n];
    for (i=0; i<opts->n; i++)
        h[i] = 0.0f;
    decim_crcf decim = decim_crcf_create(opts->D,h,opts->n);

    float complex x[opts->D];
    float complex y;
    for (i=0; i<opts->D; i++)
        x[i] = 1.0f;
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        // DSP execution goes here
        decim_crcf_execute(decim,x,&y,opts->D-1);
    }
    getrusage(RUSAGE_SELF, _finish);

    // DSP cleanup goes here
    decim_crcf_destroy(decim);
}

void precision_decim_crcf(
    unsigned int argc,
    char *argv[],
    float * _error)
{
    // blah...
    *_error = 0.0f;
}

int main() {
    struct rusage start,finish;
    unsigned long int num_trials = 1000000;
    unsigned int n;
    unsigned int D;
    double extime;
    double cpuclock=2.4e9;
    double cycles_per_trial;
    struct decim_crcf_opts opts;
    for (D=2; D<=8; D*=2) {
        printf("***** D = %u\n",D);
        for (n=5; n<31; n+=4) {
            opts.D = D;
            opts.n = n;
            benchmark_decim_crcf((void*)(&opts),&start,&finish,&num_trials);
            extime = calculate_execution_time(start,finish);
            cycles_per_trial = cpuclock * extime / (double)(num_trials);
            printf("n : %3u, D : %3u, cycles/trial : %6.2f\n", n,D,cycles_per_trial);
        }
    }
    printf("done.\n");
    return 0;
}

