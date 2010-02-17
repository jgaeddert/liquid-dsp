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

//
// Lightweight autotest header
//
// Similar to CxxTest, but written for liquid DSP code in C
//

#ifndef __LIQUID_AUTOTEST_H__
#define __LIQUID_AUTOTEST_H__

#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <inttypes.h>

static unsigned long int liquid_autotest_num_checks=0;
static unsigned long int liquid_autotest_num_passed=0;
static unsigned long int liquid_autotest_num_failed=0;

static unsigned long int liquid_autotest_num_warnings=0;

static bool liquid_autotest_verbose = true;

static inline void liquid_autotest_failed()
{
    liquid_autotest_num_checks++;
    liquid_autotest_num_failed++;
}

static inline void liquid_autotest_passed()
{
    liquid_autotest_num_checks++;
    liquid_autotest_num_passed++;
}

static inline void liquid_autotest_failed_expr(const char * _file,
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

static inline void liquid_autotest_failed_msg(const char * _file,
                                              unsigned int _line,
                                              const char * _message)
{
    if (liquid_autotest_verbose)
        printf("  TEST FAILED: %s line %u : %s\n", _file, _line, _message);
    liquid_autotest_failed();
}

static void autotest_print_results(void)
{
    if (liquid_autotest_num_warnings > 0) {
        printf("==================================\n");
        printf(" WARNINGS : %-lu\n", liquid_autotest_num_warnings);
    }

    printf("==================================\n");
    if (liquid_autotest_num_failed==0) {
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

// CONTEND_EQUALITY
#  define TEST_EQUALITY(F,L,EX,X,EY,Y)                              \
     if ((X)!=(Y)) liquid_autotest_failed_expr(F,L,EX,X,"==",EY,Y); \
     else liquid_autotest_passed();
#  define CONTEND_EQUALITY_FL(F,L,X,Y)      TEST_EQUALITY(F,L,#X,(X),#Y,(Y))
#  define CONTEND_EQUALITY(X,Y)             CONTEND_EQUALITY_FL(__FILE__,__LINE__,X,Y)

// CONTEND_INEQUALITY
#  define TEST_INEQUALITY(F,L,EX,X,EY,Y)                            \
     if ((X)==(Y)) liquid_autotest_failed_expr(F,L,EX,X,"!=",EY,Y); \
     else liquid_autotest_passed();
#  define CONTEND_INEQUALITY_FL(F,L,X,Y)    TEST_INEQUALITY(F,L,#X,(X),#Y,(Y))
#  define CONTEND_INEQUALITY(X,Y)           CONTEND_INEQUALITY_FL(__FILE__,__LINE__,X,Y)

// CONTEND_GREATER_THAN
#  define TEST_GREATER_THAN(F,L,EX,X,EY,Y)                          \
     if ((X)<=(Y)) liquid_autotest_failed_expr(F,L,EX,X,">",EY,Y);  \
     else liquid_autotest_passed();
#  define CONTEND_GREATER_THAN_FL(F,L,X,Y)  TEST_GREATER_THAN(F,L,#X,(X),#Y,(Y))
#  define CONTEND_GREATER_THAN(X,Y)         CONTEND_GREATER_THAN_FL(__FILE__,__LINE__,X,Y)

// CONTEND_LESS_THAN
#  define TEST_LESS_THAN(F,L,EX,X,EY,Y)                             \
     if ((X)>=(Y)) liquid_autotest_failed_expr(F,L,EX,X,">",EY,Y);  \
     else liquid_autotest_passed();
#  define CONTEND_LESS_THAN_FL(F,L,X,Y)     TEST_LESS_THAN(F,L,#X,(X),#Y,(Y))
#  define CONTEND_LESS_THAN(X,Y)            CONTEND_LESS_THAN_FL(__FILE__,__LINE__,X,Y)

// CONTEND_DELTA
#  define TEST_DELTA(F,L,EX,X,EY,Y,ED,D)                            \
    if (fabs((X)-(Y))>D)                                            \
        liquid_autotest_failed_expr(F,L,"abs(" #X "-" #Y ")",       \
                                    fabs(X-Y),"<",ED,D);            \
    else                                                            \
        liquid_autotest_passed();
#  define CONTEND_DELTA_FL(F,L,X,Y,D)       TEST_DELTA(F,L,#X,(X),#Y,(Y),#D,(D))
#  define CONTEND_DELTA(X,Y,D)              CONTEND_DELTA_FL(__FILE__,__LINE__,X,Y,D)

// CONTEND_EXPRESSION
#  define TEST_EXPRESSION(F,L,EX,X)                                 \
     if (!X) liquid_autotest_failed_expr(F,L,#X,(X),"is","1",1);    \
     else liquid_autotest_passed();
#  define CONTEND_EXPRESSION_FL(F,L,X)      TEST_EXPRESSION(F,L,#X,(X))
#  define CONTEND_EXPRESSION(X)             CONTEND_EXPRESSION_FL(__FILE__,__LINE__,X)

static inline bool liquid_autotest_same_data(uint8_t * _x, uint8_t * _y, unsigned int _n)
{
    unsigned int i;
    for (i=0; i<_n; i++) {
        if (_x[i] != _y[i])
            return false;
    }
    return true;
}

static inline void liquid_autotest_print_array(uint8_t * _x, unsigned int _n)
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
#  define TEST_SAME_DATA(F,L,EX,X,EY,Y,EN,N)                                    \
    if (!liquid_autotest_same_data((uint8_t*)(X),(uint8_t*)(Y),(N))) {          \
        liquid_autotest_failed_msg(F,L,EX "[] != " EY "[] for " EN " bytes");   \
        if (liquid_autotest_verbose) {                                          \
            liquid_autotest_print_array((uint8_t*)(X),N);                       \
            liquid_autotest_print_array((uint8_t*)(Y),N);                       \
        }                                                                       \
    } else {                                                                    \
        liquid_autotest_passed();                                               \
    }
#  define CONTEND_SAME_DATA_FL(F,L,X,Y,N)  TEST_SAME_DATA(F,L,#X,(X),#Y,(Y),#N,(N))
#  define CONTEND_SAME_DATA(X,Y,N)         CONTEND_SAME_DATA_FL(__FILE__,__LINE__,X,Y,N)


// print warning
void liquid_autotest_warn(const char * _file,
                          unsigned int _line,
                          const char * _message)
{
    if (liquid_autotest_verbose)
        fprintf(stderr,"  WARNING: %s line %u : %s\n", _file, _line, _message);

    liquid_autotest_num_warnings++;
}

// AUTOTEST WARN
#  define AUTOTEST_WARN_FL(F,L,MSG)      liquid_autotest_warn(F,L,#MSG)
#  define AUTOTEST_WARN(MSG)             AUTOTEST_WARN_FL(__FILE__,__LINE__,MSG)

// AUTOTEST PASS
#  define AUTOTEST_PASS_FL(F,L)          liquid_autotest_passed()
#  define AUTOTEST_PASS()                AUTOTEST_PASS_FL(__FILE__,__LINE__)

// AUTOTEST FAIL
#  define AUTOTEST_FAIL_FL(F,L,MSG)      liquid_autotest_failed_msg(F,L,MSG)
#  define AUTOTEST_FAIL(MSG)             AUTOTEST_FAIL_FL(__FILE__,__LINE__,MSG)

#endif // __LIQUID_AUTOTEST_H__

