char __docstr__[] = "Run benchmark programs in liquid-dsp";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "liquid.h"
#include "liquid.benchmark.h"
#include "liquid.argparse.h"

// benchmark registry (lists all LIQUID_BENCHMARK companion structs)
#include "liquid_benchmark_registry.h"

// run basic benchmark to estimate CPU clock frequency
float estimate_cpu_clock(float _target_runtime);

// benchmark main
int main(int argc, char* argv[])
{
    // set default logging level
    liquid_logger_set_level(NULL, LIQUID_INFO);
    //liquid_logger_set_config(NULL, LIQUID_LOG_FULL | LIQUID_LOG_COLOR);

    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(int,  num_trials,     1, 'T', "baseline trials", NULL);
    liquid_argparse_add(float,target_runtime,0.1,'r', "target runtime [seconds]", NULL);
    liquid_argparse_add(int,  test_id,       -1, 't', "run a specific benchmark", NULL);
    liquid_argparse_add(bool, list,       false, 'l', "list benchmarks and exit", NULL);
    liquid_argparse_add(char*,search,        "", 's', "run benchmarks with search string in name", NULL);
    liquid_argparse_add(char*,keywords,      "", 'k', "run benchmarks with matching keywords", NULL);
    liquid_argparse_add(bool, estimate_cpu,false,'c', "estimate CPU clock speed and exit", NULL);
    liquid_argparse_parse(argc,argv);

    // list benchmarks and exit if requested
    if (list) {
        unsigned int i = 0;
        while (liquid_benchmarks[i] != NULL) {
            liquid_benchmark_print_info(liquid_benchmarks[i], i);
            i++;
        }
        return LIQUID_OK;
    }

    if (estimate_cpu) {
        estimate_cpu_clock(1.5f);
        return LIQUID_OK;
    }

    // create registry from the NULL-terminated list of benchmarks
    liquid_benchmark_registry registry =
        liquid_benchmark_registry_create(liquid_benchmarks);

    // schedule benchmarks to run (default: all)
    if (test_id >= 0)
        liquid_benchmark_registry_schedule_one(registry, test_id);
    if (strlen(search) > 0)
        liquid_benchmark_registry_schedule_search(registry, search);
    if (strlen(keywords) > 0)
        liquid_benchmark_registry_schedule_keywords(registry, keywords);

    // run scheduled benchmarks, printing each result as it finishes
    unsigned int i;
    for (i=0; i<registry->num_benchmarks; i++) {
        liquid_benchmark b = registry->benchmarks[i];
        if (b->status == LIQUID_BENCHMARK_SCHED) {
            liquid_benchmark_execute(b, num_trials, target_runtime);
            liquid_benchmark_print_status(b);
        }
    }

    // print summary
    liquid_benchmark_registry_print_summary(registry);

    liquid_benchmark_registry_destroy(registry);
    return LIQUID_OK;
}

// run basic benchmark to estimate CPU clock frequency
float estimate_cpu_clock(float _target_runtime)
{
    // run NULL benchmark
    liquid_benchmark_execute(&null_s,1,_target_runtime);
    unsigned long int num_trials = null_s.num_trials;
    float             runtime    = null_s.extime;

    // estimate cpu clock frequency
    // TODO: adjust this based on runtime cpu info?
    float cpu_clock = 9.5 * num_trials / runtime;

    liquid_log_info("  performed %ld trials in %5.1f ms", num_trials, runtime * 1e3);
    
    float clock_format = cpu_clock;
    char clock_units = liquid_convert_units(&clock_format);
    liquid_log_info("  estimated clock speed: %7.3f %cHz", clock_format, clock_units);
    return cpu_clock;
}

