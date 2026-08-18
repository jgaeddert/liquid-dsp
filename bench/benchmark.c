char __docstr__[] = "Run benchmark programs in liquid-dsp";
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

// Benchmark harness for liquid-dsp

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "liquid.h"
#include "liquid.benchmark.h"
#include "liquid.argparse.h"

// convert a raw value into a metric-scaled magnitude and return the unit prefix
// example: 0.01397 -> 13.97 with unit 'm'
static char convert_units(float * _v)
{
    char unit;
    if      (*_v < 1e-9)  { (*_v) *= 1e12;  unit = 'p'; }
    else if (*_v < 1e-6)  { (*_v) *= 1e9;   unit = 'n'; }
    else if (*_v < 1e-3)  { (*_v) *= 1e6;   unit = 'u'; }
    else if (*_v < 1e+0)  { (*_v) *= 1e3;   unit = 'm'; }
    else if (*_v < 1e3)   { (*_v) *= 1e+0;  unit = ' '; }
    else if (*_v < 1e6)   { (*_v) *= 1e-3;  unit = 'k'; }
    else if (*_v < 1e9)   { (*_v) *= 1e-6;  unit = 'M'; }
    else if (*_v < 1e12)  { (*_v) *= 1e-9;  unit = 'G'; }
    else                  { (*_v) *= 1e-12; unit = 'T'; }
    return unit;
}

// print benchmark info
int liquid_benchmark_print_info(liquid_benchmark _q, unsigned int _index)
{
    printf("index=%4u, name=%s, description=%s, keywords=%s\n",
        _index, _q->name, _q->docstr, _q->keywords);
    return LIQUID_OK;
}

// print benchmark status
int liquid_benchmark_print_status(liquid_benchmark _q)
{
    if (_q->status == LIQUID_BENCHMARK_SKIP)
        return LIQUID_OK;

    float trials_format = (float)(_q->num_trials);  char tu = convert_units(&trials_format);
    float extime_format = _q->extime;               char eu = convert_units(&extime_format);
    float rate_format   = _q->rate;                 char ru = convert_units(&rate_format);

    printf("  %-30s: %6.2f %c trials / %6.2f %cs (%6.2f %c t/s)\n",
        _q->name,
        trials_format, tu,
        extime_format, eu,
        rate_format, ru);
    return LIQUID_OK;
}

// execute benchmark: repeatedly run with growing iteration count until extime
// meets the target time, then compute throughput and processor efficiency
int liquid_benchmark_execute(liquid_benchmark  _q,
                             unsigned long int _num_trials,
                             float             _target_runtime)
{
    _q->status = LIQUID_BENCHMARK_ACTIVE;

    unsigned int num_attempts = 0;
    float runtime = 0.0f;
    for (num_attempts=1; num_attempts<30; num_attempts++)
    {
        runtime = _q->func(_num_trials);
        liquid_log_debug("%s : runtime=%.6f s, trials=%lu", _q->name, runtime, _num_trials);

        if (runtime > _target_runtime)
            break;

        // adjust iteration count and retry
        if (runtime <= 0)
            _num_trials *= 32;
        else if (_target_runtime / runtime > 256)
            _num_trials *= 256;
        else
            _num_trials *= 1.2 * _target_runtime / runtime;
    }
    if (num_attempts >= 30) {
        _q->status = LIQUID_BENCHMARK_DONE;
        return liquid_error(LIQUID_ENOCONV, "benchmark '%s' could not reach target time after %u attempts",
            _q->name, num_attempts);
    }

    _q->extime           = runtime;
    _q->num_trials       = _num_trials;
    _q->rate             = (float)(_q->num_trials) / _q->extime;
    _q->cycles_per_trial = 0; // TODO: bench_cpu_clock / _q->rate;
    _q->status           = LIQUID_BENCHMARK_DONE;
    return LIQUID_OK;
}

// create registry from a NULL-terminated list of benchmarks
liquid_benchmark_registry liquid_benchmark_registry_create(liquid_benchmark * _benchmarks)
{
    unsigned int max_benchmarks = 8000;  // safeguard
    unsigned int num_benchmarks = 0;
    while (_benchmarks[num_benchmarks] != NULL && num_benchmarks < max_benchmarks)
        num_benchmarks++;
    if (num_benchmarks >= max_benchmarks) {
        fprintf(stderr, "error: number of benchmarks exceeds maximum (%u)\n", max_benchmarks);
        return NULL;
    }

    liquid_benchmark_registry q = (liquid_benchmark_registry)malloc(sizeof(struct liquid_benchmark_registry_s));
    q->num_benchmarks = num_benchmarks;
    q->benchmarks     = _benchmarks;
    q->timer          = liquid_timer_create(LIQUID_TIMER_CLOCK);

    // schedule all benchmarks to run by default
    liquid_benchmark_registry_schedule_all(q);
    return q;
}

// destroy registry
int liquid_benchmark_registry_destroy(liquid_benchmark_registry _q)
{
    liquid_timer_destroy(_q->timer);
    free(_q);
    return LIQUID_OK;
}

// schedule all benchmarks to run
int liquid_benchmark_registry_schedule_all(liquid_benchmark_registry _q)
{
    unsigned int i;
    for (i=0; i<_q->num_benchmarks; i++)
        _q->benchmarks[i]->status = LIQUID_BENCHMARK_SCHED;
    return LIQUID_OK;
}

// schedule one specific benchmark to run (skip the rest)
int liquid_benchmark_registry_schedule_one(liquid_benchmark_registry _q, unsigned int _id)
{
    unsigned int i;
    for (i=0; i<_q->num_benchmarks; i++)
        _q->benchmarks[i]->status = (i == _id) ? LIQUID_BENCHMARK_SCHED : LIQUID_BENCHMARK_SKIP;
    if (_id >= _q->num_benchmarks)
        fprintf(stderr, "error: id (%u) exceeds number of benchmarks (%u)\n", _id, _q->num_benchmarks);
    return LIQUID_OK;
}

// schedule only benchmarks whose name matches the search string
int liquid_benchmark_registry_schedule_search(liquid_benchmark_registry _q, const char * _query)
{
    unsigned int i;
    unsigned int num_found = 0;
    for (i=0; i<_q->num_benchmarks; i++) {
        if (strstr(_q->benchmarks[i]->name, _query) != NULL) {
            _q->benchmarks[i]->status = LIQUID_BENCHMARK_SCHED;
            num_found++;
        } else {
            _q->benchmarks[i]->status = LIQUID_BENCHMARK_SKIP;
        }
    }
    if (num_found == 0)
        printf("no benchmarks matched query '%s'\n", _query);
    return LIQUID_OK;
}

// run all scheduled benchmarks
int liquid_benchmark_registry_execute(liquid_benchmark_registry _q)
{
    unsigned int i;
    for (i=0; i<_q->num_benchmarks; i++) {
        liquid_benchmark b = _q->benchmarks[i];
        if (b->status == LIQUID_BENCHMARK_SCHED)
            liquid_benchmark_execute(b, 1LU, 0.1f);
    }
    return LIQUID_OK;
}

// print status of all benchmarks
int liquid_benchmark_registry_print_status(liquid_benchmark_registry _q)
{
    printf("=========== benchmark results ===========\n");
    unsigned int i;
    for (i=0; i<_q->num_benchmarks; i++)
        liquid_benchmark_print_status(_q->benchmarks[i]);
    return LIQUID_OK;
}

// print summary of benchmark run
int liquid_benchmark_registry_print_summary(liquid_benchmark_registry _q)
{
    float runtime = liquid_timer_toc(_q->timer);
    printf("=========== benchmark summary ===========\n");
    printf("benchmarks: %u\n", _q->num_benchmarks);
    printf("runtime:    %.3f s\n", runtime);
    return LIQUID_OK;
}

// export registry results to JSON
int liquid_benchmark_registry_json(liquid_benchmark_registry _q, FILE * _fid)
{
    fprintf(_fid, "  \"benchmarks\" : [\n");
    unsigned int i;
    for (i=0; i<_q->num_benchmarks; i++) {
        liquid_benchmark b = _q->benchmarks[i];
        fprintf(_fid,
            "    {\"id\":%4u, \"trials\":%12lu, \"extime\":%12.4e, \"rate\":%12.4e, \"cycles_per_trial\":%12.4e, \"name\":\"%s\"}%s\n",
            i,
            b->num_trials,
            b->extime,
            b->rate,
            b->cycles_per_trial,
            b->name,
            (i == _q->num_benchmarks-1) ? "" : ",");
    }
    fprintf(_fid, "  ]\n");
    return LIQUID_OK;
}

float dotprod(float * _h, float * _x, unsigned int _n)
{
    float y = 0.0f;
    unsigned int i;
    for (i=0; i<_n; i++)
        y += _h[i] * _x[i];
    return y;
}

LIQUID_BENCHMARK(dummy, "dummy benchmark program", "dummy")
{
    float h[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    float x[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    float y;

    unsigned long int i;
    liquid_timer q = liquid_timer_create(LIQUID_TIMER_RUSAGE);
    for (i = 0; i < num_iterations; i++) {
        y = dotprod(h, x, 4);
        h[0] = 0.9f * h[0] + 0.1 * y;
    }
    return liquid_toc(q);
}

// autotest main
int main(int argc, char* argv[])
{
    // set default logging level
    liquid_logger_set_level(NULL, LIQUID_INFO);
    //liquid_logger_set_config(NULL, LIQUID_LOG_FULL | LIQUID_LOG_COLOR);

    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(int,  num_trials,     1, 'T', "baseline trials", NULL);
    liquid_argparse_add(float,target_runtime,0.1,'r', "target runtime [seconds]", NULL);
    //liquid_argparse_add(int,  test_id,       -1, 't', "run a specific test", NULL);
    //liquid_argparse_add(int,  random_seed,   -1, 'R', "specify random seed value", NULL);
    //liquid_argparse_add(bool, list,       false, 'l', "list tests and exit", NULL);
    //liquid_argparse_add(bool, stop_fail,  false, 'x', "stop on fail", NULL);
    //liquid_argparse_add(char*,search,        "", 's', "run tests with search string in name", NULL);
    //liquid_argparse_add(char*,json,          "", 'o', "output JSON file", NULL);
    //liquid_argparse_add(char*,logfile,       "", 'g', "output logfile", NULL);
    //liquid_argparse_add(bool, status,  false, 'P', "print full status of all at the end", NULL);
    liquid_argparse_parse(argc,argv);

    liquid_benchmark_execute(&dummy_s, num_trials, target_runtime);
    liquid_benchmark_print_status(&dummy_s);

    return LIQUID_OK;
}
