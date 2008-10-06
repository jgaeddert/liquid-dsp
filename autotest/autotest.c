// autotest.c
//
// This file is used in conjunction with autotest_include.h (generated with
// autotest_gen.py) to produce an executable for automatically testing the
// various signal processing algorithms in liquid.


// default include headers
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>

// define benchmark function pointer
typedef void(*autotest_function) (void);

// define autotest_s
struct autotest_s {
    unsigned int id;
    autotest_function api;
    const char* name;
    long unsigned int num_checks;
    long unsigned int num_passed;
    long unsigned int num_failed;
    bool pass;
};

typedef struct autotest_s * autotest;

// define package_s
struct package_s {
    unsigned int id;
    unsigned int benchmark_index;
    unsigned int num_benchmarks;
    const char* name;
};

typedef struct package_s * package;

// include auto-generated autotest header
//
// defines the following symbols:
//   #define NUM_AUTOTESTS
//   bench_t benchmarks[NUM_AUTOTESTS]
//   #define NUM_PACKAGES
//   package_t packages[NUM_PACKAGES]
//#include "../benchinclude.h"
#include "autotest_include.example.h"

// helper functions:
void print_help();
void execute_autotest(autotest _test, bool _verbose);
void execute_package(package _p, bool _verbose);
void print_autotest_results(autotest _test);
void print_package_results(package _p);

// main function
int main(int argc, char *argv[])
{
#if 0
    // initialize timing variables
    unsigned int i, j;

    // options
    enum {RUN_ALL, RUN_SINGLE_BENCH, RUN_SINGLE_PACKAGE} mode = RUN_ALL;
    unsigned int benchmark_id = 0;
    unsigned int package_id = 0;
    bool verbose = true;

    // get input options
    int d;
    while((d = getopt(argc,argv,"n:b:p:lhvq")) != EOF){
        switch (d) {
        case 'n':
            num_trials = atoi(optarg);
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
            // list packages, benchmarks and exit
            for (i=0; i<NUM_PACKAGES; i++) {
                printf("%u: %s\n", packages[i].id, packages[i].name);
                for (j=packages[i].benchmark_index; j<packages[i].num_benchmarks+packages[i].benchmark_index; j++)
                    printf("    %u: %s\n", benchmarks[j].id, benchmarks[j].name);
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
#endif

    // run all tests
    unsigned int i;
    for (i=0; i<NUM_AUTOTESTS; i++) {
        execute_autotest( &autotests[i], true );
    }
    autotest_print_results();
    return 0;
}

void print_help()
{
    // help
    printf("bench options:\n");
    printf("  -h : prints this help file\n");
    printf("  -n<num_trials>\n");
    printf("  -b<benchmark_index>\n");
    printf("  -p<package_index>\n");
    printf("  -l : lists available benchmarks\n");
    printf("  -v : verbose\n");
    printf("  -q : quiet\n");
}

void execute_autotest(autotest _test, bool _verbose)
{
    unsigned long int autotest_num_passed_init = _autotest_num_passed;
    unsigned long int autotest_num_failed_init = _autotest_num_failed;

    // execute test
    _test->api();

    _test->num_passed = _autotest_num_passed - autotest_num_passed_init;
    _test->num_failed = _autotest_num_failed - autotest_num_failed_init;
    _test->num_checks = _test->num_passed + _test->num_failed;

    if (_verbose)
        print_autotest_results(_test);
}

/*
void execute_package(package _p, bool _verbose)
{
    if (_verbose)
        printf("%u: %s\n", _package->id, _package->name);
    
    unsigned int i;
    for (i=0; i<_package->num_benchmarks; i++) {
        execute_benchmark( &benchmarks[ i + _package->benchmark_index ], _verbose );
    }
}
*/

void print_autotest_results(autotest _test)
{
    float percent_passed = 100.0f * (float) (_test->num_passed) / (float) (_test->num_checks);
    printf("\tpassed\t%lu\t%lu\t(%0.1f%%)\n",
            _test->num_passed,
            _test->num_checks,
            percent_passed);
}

/*
void print_package_results(package _p)
{
    unsigned int i;
    printf("%u: %s:\n", _package->id, _package->name);
    for (i=_package->benchmark_index; i<(_package->benchmark_index+_package->num_benchmarks); i++)
        print_benchmark_results( &benchmarks[i] );

    printf("\n");
}
*/
