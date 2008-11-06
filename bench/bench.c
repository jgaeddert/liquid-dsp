// bench.c
//
// This file is used in conjunction with benchinclude.h (generated with
// benchmarkgen.py) to produce an executable for benchmarking the various
// signal processing algorithms in liquid.
//


// default include headers
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>
#include <sys/resource.h>

// define benchmark function pointer
typedef void(*benchmark_function_t) (
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations);
//  int argc
//  char * argv[]

// define bench_t
typedef struct {
    unsigned int id;
    benchmark_function_t api;
    const char* name;
    unsigned int name_len;
    unsigned int num_trials;
    float extime;
    float rate;
} bench_t;

// define package_t
typedef struct {
    unsigned int id;
    unsigned int benchmark_index;
    unsigned int num_benchmarks;
    const char* name;
} package_t;

// include auto-generated benchmark header
//
// defines the following symbols:
//   #define BENCHMARK_VERSION
//   #define NUM_BENCHMARKS
//   bench_t benchmarks[NUM_BENCHMARKS]
//   #define NUM_PACKAGES
//   package_t packages[NUM_PACKAGES]
#include "../benchmark_include.h"

// helper functions:
void print_help();
void estimate_cpu_speed(void);
void execute_benchmark(bench_t* _benchmark, bool _verbose);
void execute_package(package_t* _package, bool _verbose);
void print_benchmark_results(bench_t* _benchmark);
void print_package_results(package_t* _package);
double calculate_execution_time(struct rusage, struct rusage);

unsigned long int num_trials = 1<<12;

// main function
int main(int argc, char *argv[])
{
    // initialize timing variables
    unsigned int i, j;

    // options
    enum {RUN_ALL, RUN_SINGLE_BENCH, RUN_SINGLE_PACKAGE} mode = RUN_ALL;
    unsigned int benchmark_id = 0;
    unsigned int package_id = 0;
    bool verbose = true;
    bool autoscale = true;

    // get input options
    int d;
    while((d = getopt(argc,argv,"en:b:p:lLhvq")) != EOF){
        switch (d) {
        case 'e':
            estimate_cpu_speed();
            return 0;
        case 'n':
            num_trials = atoi(optarg);
            autoscale = false;
            break;
        case 'b':
            benchmark_id = atoi(optarg);
            if (benchmark_id >= NUM_BENCHMARKS) {
                printf("error, cannot run benchmark %u; index exceeded\n", benchmark_id);
                return -1;
            } else {
                mode = RUN_SINGLE_BENCH;
            }
            break;
        case 'p':
            package_id = atoi(optarg);
            if (package_id >= NUM_PACKAGES) {
                printf("error, cannot run package %u; index exceeded\n", package_id);
                return -1;
            } else {
                mode = RUN_SINGLE_PACKAGE;
            }
            break;
        case 'l':
            // list only packages and exit
            for (i=0; i<NUM_PACKAGES; i++)
                printf("%u: %s\n", packages[i].id, packages[i].name);
            return 0;
        case 'L':
            // list packages, benchmarks and exit
            for (i=0; i<NUM_PACKAGES; i++) {
                printf("%u: %s\n", packages[i].id, packages[i].name);
                for (j=packages[i].benchmark_index; j<packages[i].num_benchmarks+packages[i].benchmark_index; j++)
                    printf("    %u:\t%s\n", benchmarks[j].id, benchmarks[j].name);
            }
            return 0;
        case 'v':
            verbose = true;
            break;
        case 'q':
            verbose = false;
            break;
        case 'h':
            print_help();
            return 0;
        default:
            print_help();
            return 0;
        }
    }

    // run empty loop; a bug was found that sometimes the first package run
    // resulted in a longer execution time than what the benchmark really
    // reflected.  This loop prevents that from happening.
    for (i=0; i<1e6; i++) {
        // do nothing
    }

    if (autoscale)
        estimate_cpu_speed();

    switch (mode) {
    case RUN_ALL:
        for (i=0; i<NUM_PACKAGES; i++)
            execute_package( &packages[i], verbose );

        //for (i=0; i<NUM_PACKAGES; i++)
        //    print_package_results( &packages[i] );
        break;
    case RUN_SINGLE_BENCH:
        execute_benchmark( &benchmarks[benchmark_id], verbose );
        //print_benchmark_results( &benchmarks[benchmark_id] );
        return 0;
    case RUN_SINGLE_PACKAGE:
        execute_package( &packages[package_id], verbose );
        //print_package_results( &packages[package_id] );
        break;
    }

    return 0;
}

void print_help()
{
    // help
    printf("liquid benchmark version %s\n\n", BENCHMARK_VERSION);
    printf("bench options:\n");
    printf("  -h : prints this help file\n");
    printf("  -e : estimate cpu clock frequency and exit\n");
    printf("  -n<num_trials>\n");
    printf("  -p<package_index>\n");
    printf("  -b<benchmark_index>\n");
    printf("  -l : lists available packages\n");
    printf("  -L : lists all available benchmarks\n");
    printf("  -v : verbose\n");
    printf("  -q : quiet\n");
}

// run basic benchmark to estimate CPU clock frequency
void estimate_cpu_speed(void)
{
    printf("  estimating cpu clock frequency...\n");
    unsigned long int i, n = 1<<4;
    struct rusage start, finish;
    double extime;
    
    do {
        // trials
        n <<= 1;
        unsigned int x=0;
        getrusage(RUSAGE_SELF, &start);
        for (i=0; i<n; i++) {
            // perform mindless task
            x <<= 1;
            x |= 1;
            x &= 0xff;
            x ^= 0xff;
        }
        getrusage(RUSAGE_SELF, &finish);

        extime = calculate_execution_time(start, finish);
    } while (extime < 0.5 && n < (1<<28));

    // estimate cpu clock frequency
    double cpu_clock = 23.9 * n / extime;

    printf("  performed %ld trials in %5.1f ms\n", n, extime * 1e3);
    printf("  estimated clock speed: %E Hz\n", cpu_clock);

    unsigned long int min_trials = 256;
    num_trials = (unsigned long int) ( cpu_clock / 20e3 );
    num_trials = (num_trials < min_trials) ? min_trials : num_trials;

    printf("  setting number of trials to %ld\n", num_trials);
}

void execute_benchmark(bench_t* _benchmark, bool _verbose)
{
    unsigned long int n = num_trials;
    struct rusage start, finish;

    _benchmark->api(&start, &finish, &n);
    _benchmark->extime = calculate_execution_time(start, finish);
    _benchmark->num_trials = n;
    _benchmark->rate = (float)(_benchmark->num_trials) / _benchmark->extime;
    if (_verbose)
        print_benchmark_results(_benchmark);
}

void execute_package(package_t* _package, bool _verbose)
{
    if (_verbose)
        printf("%u: %s\n", _package->id, _package->name);
    
    unsigned int i;
    for (i=0; i<_package->num_benchmarks; i++) {
        execute_benchmark( &benchmarks[ i + _package->benchmark_index ], _verbose );
    }
}

void print_benchmark_results(bench_t* _b)
{
    // format time output to use units
    float extime_format = _b->extime;
    char *extime_units = "";
    if (extime_format < 1e-9) {
        extime_format *= 1e12;
        extime_units = "ps";
    } else if (extime_format < 1e-6) {
        extime_format *= 1e9;
        extime_units = "ns";
    } else if (extime_format < 1e-3) {
        extime_format *= 1e6;
        extime_units = "us";
    } else if (extime_format < 1) {
        extime_format *= 1e3;
        extime_units = "ms";
    } else {
        extime_units = "s";
    }

    // format rate output to use units
    float rate_format = _b->rate;
    char *rate_units = "";
    if (rate_format > 1e9) {
        rate_format /= 1e9;
        rate_units = "G ";
    } else if (rate_format > 1e6) {
        rate_format /= 1e6;
        rate_units = "M ";
    } else if (rate_format > 1e3) {
        rate_format /= 1e3;
        rate_units = "k ";
    }
    printf("    %u:\t%30s: %8d trials in %7.2f %s (%8.3f %strials/s)\n",
        _b->id, _b->name, _b->num_trials, extime_format, extime_units, rate_format, rate_units);
}

void print_package_results(package_t* _package)
{
    unsigned int i;
    printf("%u: %s:\n", _package->id, _package->name);
    for (i=_package->benchmark_index; i<(_package->benchmark_index+_package->num_benchmarks); i++)
        print_benchmark_results( &benchmarks[i] );

    printf("\n");
}

double calculate_execution_time(struct rusage _start, struct rusage _finish)
{
    return _finish.ru_utime.tv_sec - _start.ru_utime.tv_sec
        + 1e-6*(_finish.ru_utime.tv_usec - _start.ru_utime.tv_usec)
        + _finish.ru_stime.tv_sec - _start.ru_stime.tv_sec
        + 1e-6*(_finish.ru_stime.tv_usec - _start.ru_stime.tv_usec);
}

