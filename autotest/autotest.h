//
// Lightweight autotest header
//
// Similar to CxxTest, but written for Liquid DSP code in C
//

#ifndef __LIQUID_AUTOTEST_H__
#define __LIQUID_AUTOTEST_H__

#include <stdio.h>
#include <math.h>

static unsigned long int _autotest_num_passed=0;
static unsigned long int _autotest_num_failed=0;

static inline void test_failed(
    const char * _file,
    unsigned int _line,
    const char * _exprL,
    double _valueL,
    const char * _qualifier,
    const char * _exprR,
    double _valueR)
{
    printf("  TEST FAILED: %s line %u : expected %s (%0.2E) %s %s (%0.2E)\n",
            _file, _line, _exprL, _valueL, _qualifier, _exprR, _valueR);
    _autotest_num_failed++;
}

static inline void test_passed()
{
    _autotest_num_passed++;
}

static void autotest_print_results(void)
{
    printf("==================================\n");
    if (_autotest_num_failed==0) {
        printf(" PASSED ALL %lu TESTS\n", _autotest_num_passed);
    } else {
        unsigned long int total_tests = _autotest_num_passed + _autotest_num_failed;
        double percent_failed = (double) _autotest_num_failed / (double) total_tests;
        printf(" FAILED %lu / %lu TESTS (%.1f%%)\n",
                _autotest_num_failed, total_tests, 100.0*percent_failed);
    }
    printf("==================================\n");
}

// CONTEND_EQUALITY
#  define TEST_EQUALITY(F,L,EX,X,EY,Y)      \
     if ((X)!=(Y)) test_failed(F,L,EX,X,"==",EY,Y); else test_passed();
#  define _CONTEND_EQUALITY(F,L,X,Y)        TEST_EQUALITY(F,L,#X,(X),#Y,(Y))
#  define CONTEND_EQUALITY(X,Y)             _CONTEND_EQUALITY(__FILE__,__LINE__,X,Y)

// CONTEND_INEQUALITY
#  define TEST_INEQUALITY(F,L,EX,X,EY,Y)    \
     if ((X)==(Y)) test_failed(F,L,EX,X,"!=",EY,Y); else test_passed();
#  define _CONTEND_INEQUALITY(F,L,X,Y)      TEST_INEQUALITY(F,L,#X,(X),#Y,(Y))
#  define CONTEND_INEQUALITY(X,Y)           _CONTEND_INEQUALITY(__FILE__,__LINE__,X,Y)

// CONTEND_GREATER_THAN
#  define TEST_GREATER_THAN(F,L,EX,X,EY,Y)    \
     if ((X)<=(Y)) test_failed(F,L,EX,X,">",EY,Y); else test_passed();
#  define _CONTEND_GREATER_THAN(F,L,X,Y)    TEST_GREATER_THAN(F,L,#X,(X),#Y,(Y))
#  define CONTEND_GREATER_THAN(X,Y)         _CONTEND_GREATER_THAN(__FILE__,__LINE__,X,Y)

// CONTEND_LESS_THAN
#  define TEST_LESS_THAN(F,L,EX,X,EY,Y)    \
     if ((X)>=(Y)) test_failed(F,L,EX,X,">",EY,Y); else test_passed();
#  define _CONTEND_LESS_THAN(F,L,X,Y)       TEST_LESS_THAN(F,L,#X,(X),#Y,(Y))
#  define CONTEND_LESS_THAN(X,Y)            _CONTEND_LESS_THAN(__FILE__,__LINE__,X,Y)

// CONTEND_DELTA
#  define TEST_DELTA(F,L,EX,X,EY,Y,ED,D)    \
     if (fabs((X)-(Y))>D) test_failed(F,L,"abs(" #X "-" #Y ")",fabs(X-Y),"<",ED,D); else test_passed();
#  define _CONTEND_DELTA(F,L,X,Y,D)         TEST_DELTA(F,L,#X,(X),#Y,(Y),#D,(D))
#  define CONTEND_DELTA(X,Y,D)              _CONTEND_DELTA(__FILE__,__LINE__,X,Y,D)

#endif // __LIQUID_AUTOTEST_H__

