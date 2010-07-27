/*
 * Copyright (c) 2007, 2008, 2009, 2010 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010 Virginia Polytechnic
 *                                      Institute & State University
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

// autotest.c
//
// This file is used in conjunction with autotest_include.h (generated with
// autotest_gen.py) to produce an executable for automatically testing the
// various signal processing algorithms in liquid.


// default include headers
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <getopt.h>
#include "autotest/autotest.h"

// define autotest function pointer
typedef void(*autotest_function) (void);

// define autotest_s
struct autotest_s {
    unsigned int id;                // test identification
    autotest_function api;          // test function, e.g. autotest_modem()
    const char* name;               // test name
    long unsigned int num_checks;   // number of checks that were run for this test
    long unsigned int num_passed;   // number of checks that passed
    long unsigned int num_failed;   // number of checks that failed
    long unsigned int num_warnings; // number of warnings 
    float percent_passed;           // percent of checks that passed
    bool executed;                  // was the test executed?
    bool pass;                      // did the test pass? (i.e. no failures)
};

typedef struct autotest_s * autotest;

// define package_s
struct package_s {
    unsigned int id;                // package identification
    unsigned int autotest_index;    // index of first autotest
    unsigned int num_autotests;     // number of tests in package
    const char* name;               // package name
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

// 
// helper functions:
//

// print help/usage and exit
void print_help();

// execute a specific test
void execute_autotest(autotest _test, bool _verbose);

// execute a specific package
void execute_package(package _p, bool _verbose);

// print all autotest results
void print_autotest_results(autotest _test);

// print the results of a particular package
void print_package_results(package _p);

// print all unstable tests (those which failed or gave warnings)
void print_unstable_tests(void);

// main function
int main(int argc, char *argv[])
{
    // options
    enum {RUN_ALL,              // run all tests
          RUN_SINGLE_TEST,      // run just a single test
          RUN_SINGLE_PACKAGE,   // run just a single package
          RUN_SEARCH
    } mode = RUN_ALL;

    unsigned int autotest_id = 0;
    unsigned int package_id = 0;
    bool verbose = true;
    bool stop_on_fail = false;
    char search_string[128];

    unsigned int i, j;

    // get input options
    int d;
    while((d = getopt(argc,argv,"hut:p:Llxs:vq")) != EOF){
        switch (d) {
        case 'h':
        case 'u':
            print_help();
            return 0;
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
        case 'L':
            // list packages, autotests and exit
            for (i=0; i<NUM_PACKAGES; i++) {
                printf("%u: %s\n", packages[i].id, packages[i].name);
                for (j=packages[i].autotest_index; j<packages[i].num_autotests+packages[i].autotest_index; j++)
                    printf("    %u: %s\n", autotests[j].id, autotests[j].name);
            }
            return 0;
        case 'l':
            // list only packages and exit
            for (i=0; i<NUM_PACKAGES; i++)
                printf("%u: %s\n", packages[i].id, packages[i].name);
            return 0;
        case 'x':
            stop_on_fail = true;
            break;
        case 's':
            mode = RUN_SEARCH;
            strncpy(search_string, optarg, 128);
            search_string[127] = '\0';
            break;
        case 'v':
            verbose = true;
            liquid_autotest_verbose = true;
            break;
        case 'q':
            verbose = false;
            liquid_autotest_verbose = false;
            break;
        default:
            print_help();
            return 0;
        }
    }

    unsigned int n=0;
    switch (mode) {
    case RUN_ALL:
        for (i=0; i<NUM_PACKAGES; i++) {
            execute_package( &packages[i], verbose );

            n++;
            if (stop_on_fail && liquid_autotest_num_failed > 0)
                break;
        }

        for (i=0; i<n; i++) {
            if (verbose)
                print_package_results( &packages[i] );
        }
        break;
    case RUN_SINGLE_TEST:
        execute_autotest( &autotests[autotest_id], verbose );
        if (verbose)
            print_autotest_results( &autotests[autotest_id] );
        break;
    case RUN_SINGLE_PACKAGE:
        execute_package( &packages[package_id], verbose );
        if (verbose)
            print_package_results( &packages[package_id] );
        break;
    case RUN_SEARCH:
        printf("running all autotests matching '%s'...\n", search_string);
        for (i=0; i<NUM_AUTOTESTS; i++) {
            // see if search string matches autotest name
            if (strstr(autotests[i].name, search_string) != NULL) {
                // run the autotest
                execute_autotest( &autotests[i], verbose );
            }
        }
        break;
    }

    if (liquid_autotest_verbose)
        print_unstable_tests();

    autotest_print_results();
    return 0;
}

void print_help()
{
    // help
    printf("autotest options:\n");
    printf("  -h,-u : prints this help file\n");
    printf("  -t<n> : run specific test\n");
    printf("  -p<n> : run specific package\n");
    printf("  -L    : lists all autotests\n");
    printf("  -l    : lists all packages\n");
    printf("  -x    : stop on fail\n");
    printf("  -s<string>: run all tests matching search string\n");
    printf("  -v    : verbose\n");
    printf("  -q    : quiet\n");
}

// execute a specific autotest
//  _test       :   pointer to autotest object
//  _verbose    :   verbose output flag
void execute_autotest(autotest _test,
                      bool _verbose)
{
    unsigned long int autotest_num_passed_init = liquid_autotest_num_passed;
    unsigned long int autotest_num_failed_init = liquid_autotest_num_failed;
    unsigned long int autotest_num_warnings_init = liquid_autotest_num_warnings;

    // execute test
    _test->api();

    _test->num_passed = liquid_autotest_num_passed - autotest_num_passed_init;
    _test->num_failed = liquid_autotest_num_failed - autotest_num_failed_init;
    _test->num_warnings = liquid_autotest_num_warnings - autotest_num_warnings_init;
    _test->num_checks = _test->num_passed + _test->num_failed;
    _test->pass = (_test->num_failed==0) ? true : false;
    if (_test->num_checks > 0)
        _test->percent_passed = 100.0f * (float) (_test->num_passed) / (float) (_test->num_checks);
    else
        _test->percent_passed = 0.0f;

    _test->executed = true;

    //if (_verbose)
    //    print_autotest_results(_test);
}

// execute a specific package
//  _p          :   pointer to package object
//  _verbose    :   verbose output flag
void execute_package(package _p,
                     bool _verbose)
{
    if (_verbose)
        printf("%u: %s\n", _p->id, _p->name);
    
    unsigned int i;
    for (i=0; i<_p->num_autotests; i++) {
        execute_autotest( &autotests[ i + _p->autotest_index ], _verbose );
    }
}

// print results of a particular test
void print_autotest_results(autotest _test)
{
    if (!_test->executed)
        printf("    %3u :   IGNORED ", _test->id);
    else if (_test->pass)
        printf("    %3u :   PASS    ", _test->id);
    else
        printf("    %3u : <<FAIL>>  ", _test->id);

    printf("passed %4lu / %4lu checks (%5.1f%%) : %s\n",
            _test->num_passed,
            _test->num_checks,
            _test->percent_passed,
            _test->name);
}

// print results of a particular package
void print_package_results(package _p)
{
    unsigned int i;
    printf("%u: %s:\n", _p->id, _p->name);
    for (i=_p->autotest_index; i<(_p->autotest_index+_p->num_autotests); i++)
        print_autotest_results( &autotests[i] );

    printf("\n");
}

// print all unstable tests (those which failed or gave warnings)
void print_unstable_tests(void)
{
    if (liquid_autotest_num_failed == 0 &&
        liquid_autotest_num_warnings == 0)
    {
        return;
    }

    printf("==================================\n");
    printf(" UNSTABLE TESTS:\n");
    unsigned int t;
    for (t=0; t<NUM_AUTOTESTS; t++) {
        if (autotests[t].executed) {
            if (!autotests[t].pass) {
                printf("    %3u : <<FAIL>> %s\n", autotests[t].id,
                                                  autotests[t].name);
            }
            
            if (autotests[t].num_warnings > 0) {
                printf("    %3u : %4lu warnings %s\n", autotests[t].id,
                                                       autotests[t].num_warnings,
                                                       autotests[t].name);
            }
        }
    }
}

