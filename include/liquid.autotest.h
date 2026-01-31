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

// Lightweight autotest header, customized for liquid-dsp

#ifndef __LIQUID_AUTOTEST_H__
#define __LIQUID_AUTOTEST_H__

#include <inttypes.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#include "liquid.h"

// forward declaration of pointer to test structure
typedef struct liquid_autotest_s * liquid_autotest;

// individual test
struct liquid_autotest_s
{
    // configuration
    const char *      name;         // test name
    void (*func)(liquid_autotest);  // pointer to function to run (passes self)
    const char *      docstr;       // documentation string describing test
    const char *      keywords;     // optional keywords (comma-separated) for searching
    float             cost;         // rough cost for test (helpful for parallelization)
    //int               line;         // line number where test is defined

    // status and results
    enum {
        LIQUID_AUTOTEST_INIT = 0,   // test has been initialized
        // ...
        LIQUID_AUTOTEST_SCHED= 1,   // test has been scheduled to run
        LIQUID_AUTOTEST_ACTIVE=2,   // test is actively running
        LIQUID_AUTOTEST_PASS = 3,   // test finished: result = passed
        LIQUID_AUTOTEST_FAIL = 4,   // test finished: result = failed
        LIQUID_AUTOTEST_SKIP = 5,   // test skipped
    } status;
    int     num_pass;   // number of tests passsed
    int     num_fail;   // number of tests failed
    int     num_warn;   // number of warnings exhibited
    float   runtime;    // execution time [seconds]
};

// print test info
int liquid_autotest_print_info(liquid_autotest _q);

// print test status
int liquid_autotest_print_status(liquid_autotest _q);

// execute test
int liquid_autotest_execute(liquid_autotest _q);

//
void liquid_autotest_pass(liquid_autotest _q);

// log that test failed given file name, line number, and expression/message
void liquid_autotest_fail(liquid_autotest _q,
                          const char *    _file,
                          unsigned int    _line,
                          const char *    _expression);

// log that the test raised a warning
void liquid_autotest_warn(liquid_autotest _q,
                          const char *    _file,
                          unsigned int    _line,
                          const char *    _message);

// initialize autotest harness
// define liquid_autotest(...) __liquid_autotest_internal( __VA_ARGS__ )
#define LIQUID_AUTOTEST(FUNC, DOCSTR, KEYWORDS, COST)                           \
    /* forward declaration of test function                                 */  \
    void FUNC##_autotest(liquid_autotest);                                      \
    /* define structure                                                     */  \
    struct liquid_autotest_s FUNC##_s = {                                       \
        #FUNC,              /* test name                                    */  \
        FUNC##_autotest,    /* function pointer                             */  \
        DOCSTR,             /* user-defined documentation string            */  \
        KEYWORDS,           /* string representing comma-separated keywords */  \
        COST,               /* cost estimate (runtime) for executing test   */  \
        LIQUID_AUTOTEST_INIT, 0, 0, 0.0f,                                       \
    };                                                                          \
    /* define pointer to struct */                                              \
    /*static const liquid_autotest FUNC = &FUNC##_s;*/                          \
    /* define function */                                                       \
    void FUNC##_autotest(liquid_autotest __q__)  /* function definition: */     \

#if 0
// this is how a test should get expanded by the macro

// forward declaration of test function
void firfilt_crcf_basic_0_autotest(liquid_autotest);
// definte struct
struct liquid_autotest_s firfilt_crcf_basic_0_s = {
    "firfilt_crcf_basic_0",         // test name
    firfilt_crcf_basic_0_autotest,  // function pointer
    "basic filter test",            // description
    "FIR,filter,basic",             // keywords
    0.12,                           // cost (estimated runtime)
    //88,                           // line number
    LIQUID_AUTOTEST_INIT, 0, 0, 0   // status
};
// define pointer to struct
static const liquid_autotest firfilt_crcf_basic_0 = &firfilt_crcf_basic_0_s;
// define function
void firfilt_crcf_basic_0_autotest(liquid_autotest __q__)
// user-defined info here
{
    printf("firfilt_crcf_basic_0 test\n");
}
#endif

// print registry, either info or full status
int liquid_registry_print(const liquid_autotest * _registry,
                          bool _info);

// define a registry as an array of tests. Note that we use the 'weak'
// attribute in case we want to link this file against another program
// to define a separate registry
//#define LIQUID_AUTOTEST_REGISTRY \
//    __attribute__((weak)) const liquid_autotest liquid_autotest_registry[]


// Compute magnitude of (possibly) complex number
#define LIQUID_AUTOTEST_VMAG(V) (sqrt(creal(V)*creal(V)+cimag(V)*cimag(V)))

// Compute isnan on (possibly) complex number
#define LIQUID_AUTOTEST_ISNAN(V) (isnan(crealf(V)) || isnan(cimagf(V)))


// indicate pass
// TODO: add optional message?
#define LIQUID_PASS_(Q)             liquid_autotest_pass(Q);
#define LIQUID_PASS()               liquid_autotest_pass(__q__);

// indicate fail with message
#define LIQUID_FAIL_(Q,MSG)         liquid_autotest_fail(Q,__FILE__,__LINE__,MSG)
#define LIQUID_FAIL(MSG)            liquid_autotest_fail(__q__,__FILE__,__LINE__,MSG)

// log a warning
#define LIQUID_WARN_(Q,MSG)         liquid_autotest_warn(Q,__FILE__,__LINE__,MSG)
#define LIQUID_WARN(MSG)            liquid_autotest_warn(__q__,__FILE__,__LINE__,MSG)


// expand macro to run test
#define __LIQUID_TEST__(Q,F,L,X,REQUIRE)                                        \
{                                                                               \
    if (!(X)) {                                                                 \
        liquid_autotest_fail(Q,F,L,#X);                                         \
        if (REQUIRE) return;                                                    \
    } else {                                                                    \
        liquid_autotest_pass(Q);                                                \
    }                                                                           \
}

// check if expression is true
#define LIQUID_CHECK(X)             __LIQUID_TEST__(__q__,__FILE__,__LINE__,X,false)
#define LIQUID_CHECK_(Q,X)          __LIQUID_TEST__(Q,__FILE__,__LINE__,X,false)

// require that expression is true
#define LIQUID_REQUIRE(X)           __LIQUID_TEST__(__q__,__FILE__,__LINE__,X,true)
#define LIQUID_REQUIRE_(Q,X)        __LIQUID_TEST__(Q,__FILE__,__LINE__,X,true)

// check that data in two arrays are identical
int liquid_autotest_same_data(unsigned char * _x,
                              unsigned char * _y,
                              unsigned int    _n);

// test if the values in two arrays is identical
#define __LIQUID_TEST_ARRAY__(Q,F,L,X,Y,N,REQUIRE)                              \
{                                                                               \
    if (!liquid_autotest_same_data((uint8_t*)(X),(uint8_t*)(Y),(N)))            \
    {                                                                           \
        liquid_autotest_fail(Q,F,L,"array[" #N "] " #X "==" #Y);                \
        if (REQUIRE) return;                                                    \
    } else {                                                                    \
        liquid_autotest_pass(Q);                                                \
    }                                                                           \
}
// check that arrays are identical
#define LIQUID_CHECK_ARRAY_(Q,X,Y,N)    __LIQUID_TEST_ARRAY__(Q,__FILE__,__LINE__,X,Y,N,false)
#define LIQUID_CHECK_ARRAY(X,Y,N)       __LIQUID_TEST_ARRAY__(__q__,__FILE__,__LINE__,X,Y,N,false)

// require that arrays are identical
#define LIQUID_REQUIRE_ARRAY_(Q,X,Y,N)  __LIQUID_TEST_ARRAY__(Q,__FILE__,__LINE__,X,Y,N,true)
#define LIQUID_REQUIRE_ARRAY(X,Y,N)     __LIQUID_TEST_ARRAY__(__q__,__FILE__,__LINE__,X,Y,N,true)


// Test delta between two (possibly complex numbers) is within tolerance. Use the
// expanded macro for computing magnitude of difference to account for both real
// as well as complex numbers
#define __LIQUID_TEST_DELTA__(Q,F,L,X,Y,D,REQUIRE)                              \
{                                                                               \
    if (LIQUID_AUTOTEST_VMAG((X)-(Y)) > (D) ||                                  \
        LIQUID_AUTOTEST_ISNAN((X)) || LIQUID_AUTOTEST_ISNAN((Y)) )              \
    {                                                                           \
        liquid_autotest_fail(Q,F,L,"abs(" #X "-" #Y ") < " #D);                 \
        if (REQUIRE) return;                                                    \
    } else {                                                                    \
        liquid_autotest_pass(Q);                                                \
    }                                                                           \
}
#define LIQUID_CHECK_DELTA_(Q,X,Y,D)    __LIQUID_TEST_DELTA__(Q,__FILE__,__LINE__,X,Y,D,false)
#define LIQUID_CHECK_DELTA(X,Y,D)       __LIQUID_TEST_DELTA__(__q__,__FILE__,__LINE__,X,Y,D,false)

// supporting methods
typedef struct {
    float fmin, fmax;
    float pmin, pmax;
    int   test_lo, test_hi;
} autotest_psd_s;

// validate spectral content
int liquid_autotest_validate_spectrum(liquid_autotest __q__, float * _psd, unsigned int _nfft,
        autotest_psd_s * _regions, unsigned int num_regions, const char * debug_filename);

// validate spectral content of a signal (complex)
int liquid_autotest_validate_psd_signal(liquid_autotest __q__, float complex * _buf, unsigned int _buf_len,
        autotest_psd_s * _regions, unsigned int num_regions, const char * debug_filename);

// validate spectral content of a signal (real)
int liquid_autotest_validate_psd_signalf(liquid_autotest __q__, float * _buf, unsigned int _buf_len,
        autotest_psd_s * _regions, unsigned int num_regions, const char * debug_filename);

// validate spectral content of a filter (real coefficients)
int liquid_autotest_validate_psd_firfilt_crcf(liquid_autotest __q__, firfilt_crcf _q, unsigned int _nfft,
        autotest_psd_s * _regions, unsigned int num_regions, const char * debug_filename);

// validate spectral content of a filter (complex coefficients)
int liquid_autotest_validate_psd_firfilt_cccf(liquid_autotest __q__, firfilt_cccf _q, unsigned int _nfft,
        autotest_psd_s * _regions, unsigned int num_regions, const char * debug_filename);

// validate spectral content of an iir filter (real coefficients, input)
int liquid_autotest_validate_psd_iirfilt_rrrf(liquid_autotest __q__, iirfilt_rrrf _q, unsigned int _nfft,
        autotest_psd_s * _regions, unsigned int num_regions, const char * debug_filename);

// validate spectral content of a spectral periodogram object
int liquid_autotest_validate_psd_spgramcf(liquid_autotest __q__, spgramcf _q,
        autotest_psd_s * _regions, unsigned int num_regions, const char * debug_filename);

// callback function to simplify testing for framing objects
#define FRAMING_AUTOTEST_SECRET 0x01234567
int framing_autotest_callback(
    unsigned char *  _header,
    int              _header_valid,
    unsigned char *  _payload,
    unsigned int     _payload_len,
    int              _payload_valid,
    framesyncstats_s _stats,
    void *           _context);

#endif // __LIQUID_AUTOTEST_H__

