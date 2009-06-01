/*
 * Copyright (c) 2007, 2009 Joseph Gaeddert
 * Copyright (c) 2007, 2009 Virginia Polytechnic Institute & State University
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
// Lightweight autotest header
//
// Similar to CxxTest, but written for Liquid DSP code in C
//

#ifndef __LIQUID_AUTOTEST_H__
#define __LIQUID_AUTOTEST_H__

#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <inttypes.h>

static unsigned long int _autotest_num_checks=0;
static unsigned long int _autotest_num_passed=0;
static unsigned long int _autotest_num_failed=0;

static bool _autotest_verbose = true;

static inline void test_failed()
{
    _autotest_num_checks++;
    _autotest_num_failed++;
}

static inline void test_passed()
{
    _autotest_num_checks++;
    _autotest_num_passed++;
}

static inline void test_failed_expr(
    const char * _file,
    unsigned int _line,
    const char * _exprL,
    double _valueL,
    const char * _qualifier,
    const char * _exprR,
    double _valueR)
{
    if (_autotest_verbose) {
        printf("  TEST FAILED: %s line %u : expected %s (%0.2E) %s %s (%0.2E)\n",
                _file, _line, _exprL, _valueL, _qualifier, _exprR, _valueR);
    }
    test_failed();
}

static inline void test_failed_msg(
    const char * _file,
    unsigned int _line,
    const char * _message)
{
    if (_autotest_verbose)
        printf("  TEST FAILED: %s line %u : %s\n", _file, _line, _message);
    test_failed();
}

static void autotest_print_results(void)
{
    printf("==================================\n");
    if (_autotest_num_failed==0) {
        printf(" PASSED ALL %lu CHECKS\n", _autotest_num_passed);
    } else {
        double percent_failed = (double) _autotest_num_failed / (double) _autotest_num_checks;
        printf(" FAILED %lu / %lu CHECKS (%7.2f%%)\n",
                _autotest_num_failed, _autotest_num_checks, 100.0*percent_failed);
    }
    printf("==================================\n");
}

// CONTEND_EQUALITY
#  define TEST_EQUALITY(F,L,EX,X,EY,Y)      \
     if ((X)!=(Y)) test_failed_expr(F,L,EX,X,"==",EY,Y); else test_passed();
#  define CONTEND_EQUALITY_FL(F,L,X,Y)      TEST_EQUALITY(F,L,#X,(X),#Y,(Y))
#  define CONTEND_EQUALITY(X,Y)             CONTEND_EQUALITY_FL(__FILE__,__LINE__,X,Y)

// CONTEND_INEQUALITY
#  define TEST_INEQUALITY(F,L,EX,X,EY,Y)    \
     if ((X)==(Y)) test_failed_expr(F,L,EX,X,"!=",EY,Y); else test_passed();
#  define CONTEND_INEQUALITY_FL(F,L,X,Y)    TEST_INEQUALITY(F,L,#X,(X),#Y,(Y))
#  define CONTEND_INEQUALITY(X,Y)           CONTEND_INEQUALITY_FL(__FILE__,__LINE__,X,Y)

// CONTEND_GREATER_THAN
#  define TEST_GREATER_THAN(F,L,EX,X,EY,Y)    \
     if ((X)<=(Y)) test_failed_expr(F,L,EX,X,">",EY,Y); else test_passed();
#  define CONTEND_GREATER_THAN_FL(F,L,X,Y)  TEST_GREATER_THAN(F,L,#X,(X),#Y,(Y))
#  define CONTEND_GREATER_THAN(X,Y)         CONTEND_GREATER_THAN_FL(__FILE__,__LINE__,X,Y)

// CONTEND_LESS_THAN
#  define TEST_LESS_THAN(F,L,EX,X,EY,Y)    \
     if ((X)>=(Y)) test_failed_expr(F,L,EX,X,">",EY,Y); else test_passed();
#  define CONTEND_LESS_THAN_FL(F,L,X,Y)     TEST_LESS_THAN(F,L,#X,(X),#Y,(Y))
#  define CONTEND_LESS_THAN(X,Y)            CONTEND_LESS_THAN_FL(__FILE__,__LINE__,X,Y)

// CONTEND_DELTA
#  define TEST_DELTA(F,L,EX,X,EY,Y,ED,D)    \
    if (fabs((X)-(Y))>D)                    \
        test_failed_expr(F,L,"abs(" #X "-" #Y ")",fabs(X-Y),"<",ED,D); \
    else                                    \
        test_passed();
#  define CONTEND_DELTA_FL(F,L,X,Y,D)       TEST_DELTA(F,L,#X,(X),#Y,(Y),#D,(D))
#  define CONTEND_DELTA(X,Y,D)              CONTEND_DELTA_FL(__FILE__,__LINE__,X,Y,D)

// CONTEND_EXPRESSION
#  define TEST_EXPRESSION(F,L,EX,X)         \
     if (!X) test_failed_expr(F,L,#X,(X),"is","1",1); else test_passed();
#  define CONTEND_EXPRESSION_FL(F,L,X)      TEST_EXPRESSION(F,L,#X,(X))
#  define CONTEND_EXPRESSION(X)             CONTEND_EXPRESSION_FL(__FILE__,__LINE__,X)

static inline bool _autotest_same_data(uint8_t * _x, uint8_t * _y, unsigned int _n)
{
    unsigned int i;
    for (i=0; i<_n; i++) {
        if (_x[i] != _y[i])
            return false;
    }
    return true;
}

static inline void _autotest_print_array(uint8_t * _x, unsigned int _n)
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

// CONTEND_SAME_DATA
#  define TEST_SAME_DATA(F,L,EX,X,EY,Y,EN,N)                        \
    if (!_autotest_same_data((uint8_t*)(X),(uint8_t*)(Y),(N))) {    \
        test_failed_msg(F,L,EX "[] != " EY "[] for " EN " bytes");  \
        if (_autotest_verbose) {                                    \
            _autotest_print_array((uint8_t*)(X),N);                 \
            _autotest_print_array((uint8_t*)(Y),N);                 \
        }                                                           \
    } else {                                                        \
        test_passed();                                              \
    }
#  define CONTEND_SAME_DATA_FL(F,L,X,Y,N)  TEST_SAME_DATA(F,L,#X,(X),#Y,(Y),#N,(N))
#  define CONTEND_SAME_DATA(X,Y,N)         CONTEND_SAME_DATA_FL(__FILE__,__LINE__,X,Y,N)

#endif // __LIQUID_AUTOTEST_H__

