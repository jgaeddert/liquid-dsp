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

// define autotest function pointer
typedef void(*autotest_function) (void);

// define autotest_s
struct autotest_s {
    unsigned int id;
    autotest_function api;
    const char* name;
    long unsigned int num_checks;
    long unsigned int num_passed;
    long unsigned int num_failed;
    float percent_passed;
    bool pass;
};

typedef struct autotest_s * autotest;

// define package_s
struct package_s {
    unsigned int id;
    unsigned int autotest_index;
    unsigned int num_autotests;
    const char* name;
};

typedef struct package_s * package;

// include auto-generated autotest header
//
// defines the following symbols:
//   #define NUM_AUTOTESTS
//   struct autotest_s autotests[NUM_AUTOTESTS]
//   #define NUM_PACKAGES
//   struct package_s packages[NUM_PACKAGES]
#include "../autotest_include.h"

// helper functions:
void print_help();
void execute_autotest(autotest _test, bool _verbose);
void execute_package(package _p, bool _verbose);
void print_autotest_results(autotest _test);
void print_package_results(package _p);

// main function
int main(int argc, char *argv[])
{

    // initialize timing variables
    unsigned int i, j;

    // options
    enum {RUN_ALL, RUN_SINGLE_TEST, RUN_SINGLE_PACKAGE} mode = RUN_ALL;
    unsigned int autotest_id = 0;
    unsigned int package_id = 0;
    bool verbose = true;

    // get input options
    int d;
    while((d = getopt(argc,argv,"t:p:lvq")) != EOF){
        switch (d) {
        case 't':
            autotest_id = atoi(optarg);
            if (autotest_id >= NUM_AUTOTESTS) {
                printf("error, cannot run autotest %u; index exceeded\n", autotest_id);
                return -1;
            } else {
                mode = RUN_SINGLE_TEST;
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
            // list packages, autotests and exit
            for (i=0; i<NUM_PACKAGES; i++) {
                printf("%u: %s\n", packages[i].id, packages[i].name);
                for (j=packages[i].autotest_index; j<packages[i].num_autotests+packages[i].autotest_index; j++)
                    printf("    %u: %s\n", autotests[j].id, autotests[j].name);
            }
            return 0;
        case 'v':
            verbose = true;
            _autotest_verbose = true;
            break;
        case 'q':
            verbose = false;
            _autotest_verbose = false;
            break;
        case 'h':
            print_help();
            return 0;
        default:
            print_help();
            return 0;
        }
    }

    switch (mode) {
    case RUN_ALL:
        for (i=0; i<NUM_PACKAGES; i++)
            execute_package( &packages[i], verbose );

        for (i=0; i<NUM_PACKAGES; i++)
            print_package_results( &packages[i] );
        break;
    case RUN_SINGLE_TEST:
        execute_autotest( &autotests[autotest_id], verbose );
        print_autotest_results( &autotests[autotest_id] );
        break;
    case RUN_SINGLE_PACKAGE:
        execute_package( &packages[package_id], verbose );
        print_package_results( &packages[package_id] );
        break;
    }

    autotest_print_results();
    return 0;
}

void print_help()
{
    // help
    printf("autotest options:\n");
    printf("  -h : prints this help file\n");
    printf("  -t <autotest_index>\n");
    printf("  -p <package_index>\n");
    printf("  -l : lists available autotests\n");
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
    _test->pass = (_test->num_failed==0) ? true : false;
    if (_test->num_checks > 0)
        _test->percent_passed = 100.0f * (float) (_test->num_passed) / (float) (_test->num_checks);
    else
        _test->percent_passed = 0.0f;

    if (_verbose)
        print_autotest_results(_test);
}

void execute_package(package _p, bool _verbose)
{
    if (_verbose)
        printf("%u: %s\n", _p->id, _p->name);
    
    unsigned int i;
    for (i=0; i<_p->num_autotests; i++) {
        execute_autotest( &autotests[ i + _p->autotest_index ], _verbose );
    }
}

void print_autotest_results(autotest _test)
{
    if (_test->pass)
        printf("\tPASS:");
    else
        printf("\tFAIL:");

    printf("\tpassed\t%lu\t/ %lu\t checks (%0.1f%%)\t%s\n",
            _test->num_passed,
            _test->num_checks,
            _test->percent_passed,
            _test->name);
}

void print_package_results(package _p)
{
    unsigned int i;
    printf("%u: %s:\n", _p->id, _p->name);
    for (i=_p->autotest_index; i<(_p->autotest_index+_p->num_autotests); i++)
        print_autotest_results( &autotests[i] );

    printf("\n");
}
