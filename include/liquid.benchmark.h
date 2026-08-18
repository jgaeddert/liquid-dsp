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
//
// A benchmark function receives its companion instance (_q) and the iteration
// count by value. The body loops _num_iterations times, brackets the timed
// inner loop with LIQUID_BENCH_TIC/TOC, and writes the number of work units
// processed to _q->num_trials. Run-level configuration (target time, CPU
// clock, base trials) is owned by the harness, not the instance. The harness
// grows the iteration count each attempt until _q->extime meets its target,
// then computes throughput as _q->num_trials / _q->extime.

#ifndef __LIQUID_BENCHMARK_H__
#define __LIQUID_BENCHMARK_H__

#include <stdio.h>

#include "liquid.h"

// forward declaration of pointer to benchmark structure
typedef struct liquid_benchmark_s * liquid_benchmark;

// benchmark function interface: the harness passes the instance (for results)
// and the iteration count by value (the loop bound). The function times only
// its inner loop via LIQUID_BENCH_TIC/TOC and reports work units via _q->num_trials.
typedef void (liquid_benchmark_function_t)(liquid_benchmark  _q,
                                           unsigned long int _num_iterations);

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
        LIQUID_BENCH_INIT   = 0,   // benchmark has been initialized
        LIQUID_BENCH_SCHED  = 1,   // benchmark has been scheduled to run
        LIQUID_BENCH_ACTIVE = 2,   // benchmark is actively running
        LIQUID_BENCH_DONE   = 3,   // benchmark finished
        LIQUID_BENCH_SKIP   = 4,   // benchmark skipped
    } status;
    unsigned long int num_trials;       // work units processed (set by body)
    float             extime;           // timed duration [seconds] (set by toc)
    float             rate;             // throughput: num_trials / extime [work units/s]
    float             cycles_per_trial; // processor efficiency estimate
};

// print benchmark info
int liquid_benchmark_print_info(liquid_benchmark _q, unsigned int _index);

// print benchmark status
int liquid_benchmark_print_status(liquid_benchmark _q);

// execute benchmark: repeatedly run with growing _num_iterations until extime
// meets the harness target time, then compute rate and cycles_per_trial
int liquid_benchmark_execute(liquid_benchmark _q);

// start timing the inner loop (resource usage captured internally)
int liquid_benchmark_tic(liquid_benchmark _q);

// stop timing, store extime on the instance (returns seconds elapsed)
float liquid_benchmark_toc(liquid_benchmark _q);

// bracket the timed inner loop; these reference the _q instance in scope
// inside a LIQUID_BENCHMARK body
#define LIQUID_BENCH_TIC()  liquid_benchmark_tic(_q)
#define LIQUID_BENCH_TOC()  liquid_benchmark_toc(_q)

// Define a benchmark: forward-declares the function, emits the companion
// status structure, and opens the function body. The block written
// immediately after the macro is the function body; the instance is in scope
// as "_q" and the iteration count as "_num_iterations" (by value).
//
//   LIQUID_BENCHMARK(firfilt_crcf_4, "firfilt_crcf execute, n=4", "FIR,filter")
//   {
//       firfilt_crcf f = firfilt_crcf_create(h, 4);   // setup (untimed)
//       unsigned long int i, n = _num_iterations;
//       LIQUID_BENCH_TIC();
//       for (i=0; i<n; i++) { /* push/execute ... */ }
//       LIQUID_BENCH_TOC();
//       _q->num_trials = n * 4;   // 4 work units per loop iteration
//       firfilt_crcf_destroy(f); // cleanup (untimed)
//   }
#define LIQUID_BENCHMARK(FUNC, DOCSTR, KEYWORDS)                               \
    /* forward declaration of benchmark function                              */ \
    void FUNC##_benchmark(liquid_benchmark, unsigned long int);                \
    /* define companion structure (results zero-initialized)                 */ \
    struct liquid_benchmark_s FUNC##_s = {                                     \
        #FUNC,                /* benchmark name                              */ \
        FUNC##_benchmark,     /* function pointer                             */ \
        DOCSTR,               /* user-defined documentation string            */ \
        KEYWORDS,             /* string representing comma-separated keywords */ \
        LIQUID_BENCH_INIT,    /* status                                       */ \
    };                                                                         \
    /* define function: the following { ... } is the body                    */ \
    void FUNC##_benchmark(liquid_benchmark _q, unsigned long int _num_iterations)

#if 0
// this is how a benchmark should get expanded by the macro

// forward declaration of benchmark function
void firfilt_crcf_4_benchmark(liquid_benchmark, unsigned long int);
// define companion struct (results zero-initialized by partial initializer)
struct liquid_benchmark_s firfilt_crcf_4_s = {
    "firfilt_crcf_4",               // name
    firfilt_crcf_4_benchmark,       // function pointer
    "firfilt_crcf execute, n=4",    // description
    "FIR,filter",                   // keywords
    LIQUID_BENCH_INIT,              // status
    // num_trials, extime, rate, cycles_per_trial all zero-initialized
};
// define function
void firfilt_crcf_4_benchmark(liquid_benchmark _q, unsigned long int _num_iterations)
{
    unsigned long int i, n = _num_iterations;
    LIQUID_BENCH_TIC();
    for (i=0; i<n; i++) { /* timed inner loop */ }
    LIQUID_BENCH_TOC();
    _q->num_trials = n * 4;
}
#endif

// structured registry to simplify benchmarking
struct liquid_bench_registry_s
{
    // total benchmarks within registry
    unsigned int num_benchmarks;

    // pointer to list of benchmarks (NULL-terminated)
    liquid_benchmark * benchmarks;

    // keep track of runtime
    liquid_timer timer;
};

// pointer to struct
typedef struct liquid_bench_registry_s * liquid_bench_registry;

// create registry from pointer to benchmarks
liquid_bench_registry liquid_bench_registry_create(liquid_benchmark * _benchmarks);

// destroy registry
int liquid_bench_registry_destroy(liquid_bench_registry _q);

// schedule all benchmarks to run
int liquid_bench_registry_schedule_all(liquid_bench_registry _q);

// schedule one specific benchmark to run
int liquid_bench_registry_schedule_one(liquid_bench_registry _q, unsigned int _id);

// schedule only benchmarks that match search string
int liquid_bench_registry_schedule_search(liquid_bench_registry _q, const char * _query);

// run all scheduled benchmarks
int liquid_bench_registry_execute(liquid_bench_registry _q);

// print status of benchmarks
int liquid_bench_registry_print_status(liquid_bench_registry _q);

// print summary of benchmark run
int liquid_bench_registry_print_summary(liquid_bench_registry _q);

// export registry to JSON file
int liquid_bench_registry_json(liquid_bench_registry _q, FILE * _fid);

#endif // __LIQUID_BENCHMARK_H__
