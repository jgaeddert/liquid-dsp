/*
 * Copyright (c) 2007 - 2015 Joseph Gaeddert
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

//
// autotestlib.c
//

// default include headers
#include <stdio.h>
#include <stdlib.h>
#include "autotest/autotest.h"

// total number of checks invoked
unsigned long int liquid_autotest_num_checks=0;

// total number of checks which passed
unsigned long int liquid_autotest_num_passed=0;

// total number of checks which failed
unsigned long int liquid_autotest_num_failed=0;

// total number of warnings
unsigned long int liquid_autotest_num_warnings=0;

// verbosity flag
int liquid_autotest_verbose = 1;

// fail test
// increment liquid_autotest_num_checks
// increment liquid_autotest_num_failed
void liquid_autotest_failed()
{
    liquid_autotest_num_checks++;
    liquid_autotest_num_failed++;
}

// pass test
// increment liquid_autotest_num_checks
// increment liquid_autotest_num_passed
void liquid_autotest_passed()
{
    liquid_autotest_num_checks++;
    liquid_autotest_num_passed++;
}

// fail test, given expression
//  _file       :   filename (string)
//  _line       :   line number of test
//  _exprL      :   left side of expression (string)
//  _valueL     :   left side of expression (value)
//  _qualifier  :   expression qualifier
//  _exprR      :   right side of expression (string)
//  _valueR     :   right side of expression (value)
void liquid_autotest_failed_expr(const char * _file,
                                 unsigned int _line,
                                 const char * _exprL,
                                 double _valueL,
                                 const char * _qualifier,
                                 const char * _exprR,
                                 double _valueR)
{
    if (liquid_autotest_verbose) {
        printf("  TEST FAILED: %s line %u : expected %s (%0.2E) %s %s (%0.2E)\n",
                _file, _line, _exprL, _valueL, _qualifier, _exprR, _valueR);
    }
    liquid_autotest_failed();
}

//  _file       :   filename (string)
//  _line       :   line number of test
//  _message    :   message string
void liquid_autotest_failed_msg(const char * _file,
                                unsigned int _line,
                                const char * _message)
{
    if (liquid_autotest_verbose)
        printf("  TEST FAILED: %s line %u : %s\n", _file, _line, _message);
    liquid_autotest_failed();
}

// print basic autotest results to stdout
void autotest_print_results(void)
{
    if (liquid_autotest_num_warnings > 0) {
        printf("==================================\n");
        printf(" WARNINGS : %-lu\n", liquid_autotest_num_warnings);
    }

    printf("==================================\n");
    if (liquid_autotest_num_checks==0) {
        printf(" NO CHECKS RUN\n");
    } else if (liquid_autotest_num_failed==0) {
        printf(" PASSED ALL %lu CHECKS\n", liquid_autotest_num_passed);
    } else {
        // compute and print percentage of failed tests
        double percent_failed = (double) liquid_autotest_num_failed /
                                (double) liquid_autotest_num_checks;
        printf(" FAILED %lu / %lu CHECKS (%7.2f%%)\n",
                liquid_autotest_num_failed,
                liquid_autotest_num_checks,
                100.0*percent_failed);
    }
    printf("==================================\n");
}

// print warning to stderr
// increment liquid_autotest_num_warnings
//  _file       :   filename (string)
//  _line       :   line number of test
//  _message    :   message string
void liquid_autotest_warn(const char * _file,
                          unsigned int _line,
                         const char * _message)
{
    if (liquid_autotest_verbose)
        fprintf(stderr,"  WARNING: %s line %u : %s\n", _file, _line, _message);

    liquid_autotest_num_warnings++;
}

// contend that data in two arrays are identical
//  _x      :   input array [size: _n x 1]
//  _y      :   input array [size: _n x 1]
//  _n      :   input array size
int liquid_autotest_same_data(unsigned char * _x,
                              unsigned char * _y,
                              unsigned int _n)
{
    unsigned int i;
    for (i=0; i<_n; i++) {
        if (_x[i] != _y[i])
            return 0;
    }
    return 1;
}

// print array to standard out
//  _x      :   input array [size: _n x 1]
//  _n      :   input array size
void liquid_autotest_print_array(unsigned char * _x,
                                 unsigned int _n)
{
    unsigned int i;
    printf("   {");
    for (i=0; i<_n; i++) {
        printf("%.2x, ", (unsigned int)(_x[i]));
        if ( ((i+1)%16 == 0) && (i != (_n-1)) )
            printf("\n    ");
    }
    printf("}\n");
}

