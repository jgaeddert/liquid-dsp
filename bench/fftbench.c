/*
 * Copyright (c) 2007, 2009, 2012 Joseph Gaeddert
 * Copyright (c) 2007, 2009, 2012 Virginia Polytechnic
 *                              Institute & State University
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
// fftbench.c : benchmark fft algorithms
//


// default include headers
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <math.h>
#include <complex.h>
#include <sys/resource.h>

#include "liquid.h"

void usage()
{
    // help
    printf("Usage: benchmark [OPTION]\n");
    printf("Execute benchmark scripts for liquid-dsp library.\n");
    printf("  -h            display this help and exit\n");
    printf("  -v/q          verbose/quiet\n");
    printf("  -n[COUNT]     set number of base trials\n");
    printf("  -t[SECONDS]   set minimum execution time (s)\n");
    printf("  -o[FILENAME]  export output\n");
    printf("  -m[MODE]      mode: all, radix2, composite, prime\n");
}

// benchmark structure
struct benchmark_s {
    unsigned int nfft;          // FFT size
    int direction;              // FFT direction
    unsigned int num_trials;    // number of trials
    int flags;                  // FFT flags/method
    float extime;               // execution time
    float flops;                // computation bandwidth
};

// helper functions:
char convert_units(float * _s);
double calculate_execution_time(struct rusage, struct rusage);

unsigned long int num_base_trials = 1<<12;

// run...
#if 0
void execute_benchmarks_radix2();
void execute_benchmarks_composite();
void execute_benchmarks_prime();
void execute_benchmarks_single();
#endif

// execute single benchmark
void execute_benchmark_fft(struct benchmark_s * _benchmark,
                           float                _runtime);

// main benchmark script
void benchmark_fft(struct rusage *      _start,
                   struct rusage *      _finish,
                   struct benchmark_s * _benchmark);

FILE * fid; // output file id
void output_benchmark_to_file(FILE * _fid,
                              struct benchmark_s * _benchmark);

void benchmark_print(struct benchmark_s * _benchmark);

// main function
int main(int argc, char *argv[])
{
    // options
    enum {RUN_ALL=0,
          RUN_RADIX2,
          RUN_COMPOSITE,
          RUN_PRIME,
          RUN_NUMBER
    } mode = 0;
    int verbose = 1;
    int autoscale = 1;
    int output_to_file = 0;
    char filename[128];
    float runtime=0.100f;   // minimum run time (s)

    unsigned int nfft = 1024;

    // get input options
    int d;
    while((d = getopt(argc,argv,"uvqn:t:o:m:")) != EOF){
        switch (d) {
        case 'h':   usage();        return 0;
        case 'v':   verbose = 1;    break;
        case 'q':   verbose = 0;    break;
        case 'n':
            num_base_trials = atoi(optarg);
            autoscale = 0;
            break;
        case 't':
            runtime = atof(optarg);
            if (runtime < 1e-3f)     runtime = 1e-3f;
            else if (runtime > 10.f) runtime = 10.0f;
            printf("minimum runtime: %d ms\n", (int) roundf(runtime*1e3));
            break;
        case 'o':
            output_to_file = 1;
            strcpy(filename, optarg);
            break;
        case 'm':
            printf("mode: %s\n", optarg);
            break;
        default:
            usage();
            return 0;
        }
    }

    // run empty loop; a bug was found that sometimes the first package run
    // resulted in a longer execution time than what the benchmark really
    // reflected.  This loop prevents that from happening.
    unsigned int i;
    for (i=0; i<1e6; i++) {
        // do nothing
    }

#if 0
    switch (mode) {
    case RUN_ALL:
        break;
    case RUN_RADIX2:
        break;
    case RUN_COMPOSITE:
        break;
    case RUN_PRIME:
        break;
    case RUN_NUMBER:
        break;
    default:
        fprintf(stderr,"%s, invalid mode\n", argv[0]);
        exit(1);
    }
#else
    struct benchmark_s benchmark;
    benchmark.nfft       = 1024;
    benchmark.direction  = FFT_FORWARD;
    benchmark.num_trials = 1;
    benchmark.flags      = 0;
    benchmark.extime     = runtime;
    benchmark.flops      = 0.0f;
    execute_benchmark_fft(&benchmark, runtime);
    benchmark_print(&benchmark);
#endif

    if (output_to_file) {
        fid = fopen(filename,"w");
        if (!fid) {
            printf("error: could not open file %s for writing\n", filename);
            return 1;
        }

        // print header
        fprintf(fid,"# %s : auto-generated file\n", filename);
        fprintf(fid,"#\n");
        fprintf(fid,"# invoked as:\n");
        fprintf(fid,"#   ");
        for (i=0; i<argc; i++)
            fprintf(fid," %s", argv[i]);
        fprintf(fid,"\n");
        fprintf(fid,"#\n");
        fprintf(fid,"# properties:\n");
        fprintf(fid,"#  verbose             :   %s\n", verbose ? "true" : "false");
        fprintf(fid,"#  autoscale           :   %s\n", autoscale ? "true" : "false");
        fprintf(fid,"#  runtime             :   %12.8f s\n", runtime);
        fprintf(fid,"#  num_trials          :   %lu\n", num_base_trials);
        fprintf(fid,"#  mode                :   \n");
        fprintf(fid,"#\n");
#if 0
        fprintf(fid,"# %-5s %-30s %12s %12s %12s %12s\n",
                "id", "name", "num trials", "ex.time [s]", "rate [t/s]", "[cycles/t]");

        for (i=0; i<NUM_AUTOSCRIPTS; i++) {
            if (scripts[i].num_trials > 0)
                output_benchmark_to_file(fid, &scripts[i]);
        }
#endif

        fclose(fid);
        printf("results written to %s\n", filename);
    }

    return 0;
}

// convert raw value into metric units,
//   example: "0.01397s" -> "13.97 ms"
char convert_units(float * _v)
{
    char unit;
    if (*_v < 1e-9)     {   (*_v) *= 1e12;  unit = 'p';}
    else if (*_v < 1e-6){   (*_v) *= 1e9;   unit = 'n';}
    else if (*_v < 1e-3){   (*_v) *= 1e6;   unit = 'u';}
    else if (*_v < 1e+0){   (*_v) *= 1e3;   unit = 'm';}
    else if (*_v < 1e3) {   (*_v) *= 1e+0;  unit = ' ';}
    else if (*_v < 1e6) {   (*_v) *= 1e-3;  unit = 'k';}
    else if (*_v < 1e9) {   (*_v) *= 1e-6;  unit = 'M';}
    else if (*_v < 1e12){   (*_v) *= 1e-9;  unit = 'G';}
    else                {   (*_v) *= 1e-12; unit = 'T';}

    return unit;
}

double calculate_execution_time(struct rusage _start, struct rusage _finish)
{
    return _finish.ru_utime.tv_sec - _start.ru_utime.tv_sec
        + 1e-6*(_finish.ru_utime.tv_usec - _start.ru_utime.tv_usec)
        + _finish.ru_stime.tv_sec - _start.ru_stime.tv_sec
        + 1e-6*(_finish.ru_stime.tv_usec - _start.ru_stime.tv_usec);
}

void output_benchmark_to_file(FILE * _fid,
                              struct benchmark_s * _benchmark)
{
    fprintf(_fid,"  %-8u %-8u %12.4e\n",
            _benchmark->nfft,
            _benchmark->num_trials,
            _benchmark->extime);
}

void benchmark_print(struct benchmark_s * _benchmark)
{
    printf("  nfft=%-8u trials=%-8u runtime=%12.8f s\n",
            _benchmark->nfft,
            _benchmark->num_trials,
            _benchmark->extime);
}

// execute single benchmark
void execute_benchmark_fft(struct benchmark_s * _benchmark,
                           float                _runtime)
{
    unsigned long int n = 100; //num_base_trials;
    struct rusage start, finish;

    unsigned int num_attempts = 0;
    do {
        // increment number of attempts
        num_attempts++;

        // set number of trials and run benchmark
        _benchmark->num_trials = n;
        benchmark_fft(&start, &finish, _benchmark);

        // calculate execution time
        _benchmark->extime = calculate_execution_time(start, finish);

        // check exit criteria
        if (_benchmark->extime >= _runtime) {
            break;
        } else if (num_attempts == 30) {
            fprintf(stderr,"warning: benchmark could not execute over minimum run time\n");
            break;
        } else {
            // increase number of trials
            n *= 2;
        }
    } while (1);

    // print results...
}

// main benchmark script
void benchmark_fft(struct rusage *      _start,
                   struct rusage *      _finish,
                   struct benchmark_s * _benchmark)
{
    // initialize arrays, plan
    float complex * x = (float complex *) malloc((_benchmark->nfft)*sizeof(float complex));
    float complex * y = (float complex *) malloc((_benchmark->nfft)*sizeof(float complex));
    fftplan q = fft_create_plan(_benchmark->nfft,
                                x, y,
                                _benchmark->direction,
                                _benchmark->flags);
    
    unsigned long int i;

    // initialize input with random values
    for (i=0; i<_benchmark->nfft; i++)
        x[i] = randnf() + randnf()*_Complex_I;

    // scale number of iterations to keep execution time
    // relatively linear
    unsigned int num_iterations = _benchmark->num_trials;

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<num_iterations; i++) {
        fft_execute(q);
        fft_execute(q);
        fft_execute(q);
        fft_execute(q);
    }
    getrusage(RUSAGE_SELF, _finish);

    // set actual number of iterations in result
    _benchmark->num_trials = num_iterations * 4;

    fft_destroy_plan(q);
    free(x);
    free(y);
}

