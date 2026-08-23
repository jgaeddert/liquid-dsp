/*
 * Copyright (c) 2007 - 2026 Joseph Gaeddert
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

// Lightweight benchmark header, customized for liquid-dsp

#ifndef __LIQUID_BENCHMARK_H__
#define __LIQUID_BENCHMARK_H__

#include <stdio.h>

#include "liquid.h"

// forward declaration of pointer to benchmark structure
typedef struct liquid_benchmark_s * liquid_benchmark;

// benchmark function interface: the harness passes the number of iterations
// that the benchmark should run; the function times its own inner loop and
// returns the runtime in seconds. The harness calls the function repeatedly
// with a growing iteration count until the runtime meets the target.
typedef float (liquid_benchmark_function_t)(unsigned long int _num_iterations);

// individual benchmark
struct liquid_benchmark_s
{
    // configuration
    const char *                  name;     // benchmark name
    liquid_benchmark_function_t * func;     // pointer to function to run
    const char *                  docstr;   // documentation string describing benchmark
    const char *                  keywords; // optional keywords (comma-separated) for searching

    // status and results
    enum {
        LIQUID_BENCHMARK_INIT   = 0,// benchmark has been initialized
        LIQUID_BENCHMARK_SCHED  = 1,// benchmark has been scheduled to run
        LIQUID_BENCHMARK_ACTIVE = 2,// benchmark is actively running
        LIQUID_BENCHMARK_DONE   = 3,// benchmark finished
        LIQUID_BENCHMARK_NOTRUN = 4,// benchmark could not run (e.g. missing dependency)
        LIQUID_BENCHMARK_SKIP   = 5,// benchmark skipped
    } status;
    unsigned long int num_trials;   // work units processed (set by body)
    float             extime;       // timed duration [seconds] (set by toc)
    float             rate;         // throughput: num_trials / extime [work units/s]
    float             cycles_per_trial; // TBD
};

// print benchmark info
int liquid_benchmark_print_info(liquid_benchmark _q, unsigned int _index);

// print benchmark status
int liquid_benchmark_print_status(liquid_benchmark _q);

// execute benchmark: repeatedly run with growing _num_iterations until extime
// meets the harness target time, then compute rate and cycles_per_trial
int liquid_benchmark_execute(liquid_benchmark  _q,
                             unsigned long int _num_trials,
                             float             _target_runtime);

// Define a benchmark: forward-declares the function, emits the companion
// status structure, and opens the function body. The block written
// immediately after the macro is the function body; the iteration count is in
// scope as "num_iterations" (by value). The body times its own inner loop
// (e.g. with liquid_timer) and returns the runtime in seconds.
//
//   LIQUID_BENCHMARK(firfilt_crcf_4, "firfilt_crcf execute, n=4", "FIR,filter")
//   {
//       firfilt_crcf f = firfilt_crcf_create(h, 4);          // setup (untimed)
//       liquid_timer q = liquid_timer_create(LIQUID_TIMER_RUSAGE); // create + tic (after setup)
//       unsigned long int i, n = num_iterations / 4;         // round down to multiple of 4
//       for (i=0; i<n; i++) { /* push/execute ... (4 work units per iter) */ }
//       float extime = liquid_toc(q);                       // toc + destroy (before cleanup)
//       firfilt_crcf_destroy(f);                             // cleanup (untimed)
//       return extime;
//   }
#define LIQUID_BENCHMARK(FUNC, DOCSTR, KEYWORDS)                                \
    /* forward declaration of benchmark function                            */  \
    float FUNC##_benchmark(unsigned long int);                                  \
    /* define companion structure (results zero-initialized)                */  \
    struct liquid_benchmark_s FUNC##_s = {                                      \
        #FUNC,                /* benchmark name                             */  \
        FUNC##_benchmark,     /* function pointer                           */  \
        DOCSTR,               /* user-defined documentation string          */  \
        KEYWORDS,             /* string with comma-separated keywords       */  \
        LIQUID_BENCHMARK_INIT,    /* status                                     */  \
    };                                                                          \
    /* define function: the following { ... } is the body                   */  \
    float FUNC##_benchmark(unsigned long int num_iterations)

#if 0
// this is how a benchmark should get expanded by the macro

// forward declaration of benchmark function
float firfilt_crcf_4_benchmark(unsigned long int);
// define companion struct (results zero-initialized by partial initializer)
struct liquid_benchmark_s firfilt_crcf_4_s = {
    "firfilt_crcf_4",               // name
    firfilt_crcf_4_benchmark,       // function pointer
    "firfilt_crcf execute, n=4",    // description
    "FIR,filter",                   // keywords
    LIQUID_BENCHMARK_INIT,              // status
    // num_trials, extime, rate, cycles_per_trial all zero-initialized
};
// define function
float firfilt_crcf_4_benchmark(unsigned long int num_iterations)
{
    firfilt_crcf f = firfilt_crcf_create(h, 4);          // setup (untimed)
    liquid_timer q = liquid_timer_create(LIQUID_TIMER_RUSAGE); // create + tic (after setup)
    unsigned long int i, n = num_iterations / 4;         // round down to multiple of 4
    for (i=0; i<n; i++) { /* timed inner loop (4 work units per iter) */ }
    float extime = liquid_toc(q);                       // toc + destroy (before cleanup)
    firfilt_crcf_destroy(f);                             // cleanup (untimed)
    return extime;
}
#endif

// structured registry to simplify benchmarking
struct liquid_benchmark_registry_s
{
    // total benchmarks within registry
    unsigned int num_benchmarks;

    // pointer to list of benchmarks (NULL-terminated)
    liquid_benchmark * benchmarks;

    // keep track of runtime
    liquid_timer timer;
};

// pointer to struct
typedef struct liquid_benchmark_registry_s * liquid_benchmark_registry;

// create registry from pointer to benchmarks
liquid_benchmark_registry liquid_benchmark_registry_create(liquid_benchmark * _benchmarks);

// destroy registry
int liquid_benchmark_registry_destroy(liquid_benchmark_registry _q);

// schedule all benchmarks to run
int liquid_benchmark_registry_schedule_all(liquid_benchmark_registry _q);

// schedule one specific benchmark to run
int liquid_benchmark_registry_schedule_one(liquid_benchmark_registry _q, unsigned int _id);

// schedule only benchmarks that match search string
int liquid_benchmark_registry_schedule_search(liquid_benchmark_registry _q, const char * _query);

// filter benchmarks based on keywords; only benchmarks matching all requested
// keywords will be marked to run
//  _q          : benchmark registry
//  _keywords   : string with comma-separated values, e.g. "fft,composite"
int liquid_benchmark_registry_schedule_keywords(liquid_benchmark_registry _q,
                                                const char * _keywords);

// find specific benchmark that matches name
liquid_benchmark liquid_benchmark_registry_find(liquid_benchmark_registry _q,
                                                const char * _name);

// run all scheduled benchmarks
int liquid_benchmark_registry_execute(liquid_benchmark_registry _q);

// print status of benchmarks
int liquid_benchmark_registry_print_status(liquid_benchmark_registry _q);

// print summary of benchmark run
int liquid_benchmark_registry_print_summary(liquid_benchmark_registry _q);

// export registry to JSON file
int liquid_benchmark_registry_json(liquid_benchmark_registry _q, FILE * _fid);

#endif // __LIQUID_BENCHMARK_H__
