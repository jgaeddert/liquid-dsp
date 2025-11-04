char __docstr__[] =
"Complex finite impulse response filter example. Demonstrates the"
" functionality of firfilt by designing a low-order prototype and using it"
" to filter a noisy signal.  The filter coefficients are real, but the"
" input and output arrays are complex. The filter order and cutoff"
" frequency are specified at the beginning.";

#include <stdio.h>
#include <math.h>
#include <complex.h>

#include "liquid.h"
#include "liquid.argparse.h"
//#include "liquid.autotest.h"

// individual test
struct liquid_autotest_s
{
    // configuration
    const char *      name;     // test name
    void (*func)(void);         // pointer to function to run
    const char *      docstr;   // documentation string describing test
    const char *      keywords; // optional keywords (comma-separated) for searching
    float             cost;     // rough cost for test (helpful for parallelization)
    //int               line;     // line number where test is defined

    // status and results
    enum {
        LIQUID_AUTOTEST_INIT = 0,
        LIQUID_AUTOTEST_PASS = 1,
        LIQUID_AUTOTEST_FAIL = 2,
        LIQUID_AUTOTEST_SKIP = 3
    } status;
    int     num_tests;  // number of actual tests run
    int     num_pass;   // number of tests passsed
    int     num_fail;   // number of tests failed
};

typedef struct liquid_autotest_s * liquid_autotest;

// initialize the package
#define liquid_autotest_package_init(DOCSTR)                                    \
    /* declare package object and initialize with options */                    \
    /* TODO: if we want to include this package in one global function, we  */  \
    /*       will need to mangle its name so as to avoid linker errors;     */  \
    /*       might consider using something like:                           */  \
    /*          __COUNTER__ ## __LINE__                                     */  \
    struct liquid_autotest_package_s __package;                                 \
    __package.file   = __FILE__; /* store reference file */                     \
    __package.docstr = DOCSTR;                                                  \
    __package.num_autotests = 0;                                                \

// add autotest to package
// define liquid_autotest(...) __liquid_autotest_internal( __VA_ARGS__ )
#define LIQUID_AUTOTEST(FUNC, DOCSTR, KEYWORDS, COST)                           \
    /* forward declaration of test function                                 */  \
    void FUNC##_autotest(void);                                                 \
    /* define structure                                                     */  \
    struct liquid_autotest_s FUNC##_s = {                                       \
        #FUNC, /* test name */                                                  \
        FUNC##_autotest, /* function pointer */                                 \
        DOCSTR,                                                                 \
        KEYWORDS,                                                               \
        COST,                                                                   \
        LIQUID_AUTOTEST_INIT, 0, 0, 0,                                          \
    };                                                                          \
    /* define pointer to struct */                                              \
    static const liquid_autotest FUNC = &FUNC##_s;                              \
    /* define function */                                                       \
    void FUNC##_autotest(void)  /* continue with function definition */  \

// forward declaration of test function
void firfilt_crcf_basic_0_autotest(void);
// definte struct
struct liquid_autotest_s firfilt_crcf_basic_0_s = {
    "firfilt_crcf_basic_0", // test name
    firfilt_crcf_basic_0_autotest,
    "finite impulse response blah blah",
    "FIR,filter,basic",
    0.12,
    //88, //line
    LIQUID_AUTOTEST_INIT, 0, 0, 0 // status
};
// define pointer to struct
static const liquid_autotest firfilt_crcf_basic_0 = &firfilt_crcf_basic_0_s;
// define function
void firfilt_crcf_basic_0_autotest(void)
// user-defined info here
{
    printf("firfilt_crcf_basic_0 test\n");
}

// forward declaration of test function
void firfilt_crcf_basic_1_autotest(void);
// definte struct
struct liquid_autotest_s firfilt_crcf_basic_1_s = {
    "firfilt_crcf_basic_1", // test name
    firfilt_crcf_basic_1_autotest,
    "another finite impulse response test",
    "FIR,filter,basic",
    0.12,
    //108, //line
    LIQUID_AUTOTEST_INIT, 0, 0, 0 // status
};
// define pointer to struct
static const liquid_autotest firfilt_crcf_basic_1 = &firfilt_crcf_basic_1_s;
// define function
void firfilt_crcf_basic_1_autotest(void)
// user-defined info here
{
    printf("firfilt_crcf_basic_1 test\n");
}

LIQUID_AUTOTEST(firfilt_crcf_basic_2, "basic filter test", "a,b,c", 0.1)
{
    printf("firfilt_crcf_basic_2 test\n");
}

//
#define liquid_autotest_list const liquid_autotest liquid_registry[]

// test list
liquid_autotest_list =
{
    firfilt_crcf_basic_0,
    firfilt_crcf_basic_1,
    firfilt_crcf_basic_2,
    NULL,
};

// autotest main
int main(int argc, char* argv[])
{
    // define variables and parse command-line options
    /*
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*,    filename, "firfilt_crcf_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(unsigned, h_len,        65, 'H', "filter length", NULL);
    liquid_argparse_add(float,    fc,          0.1, 'c', "filter cutoff frequency", NULL);
    liquid_argparse_add(float,    As,           60, 's', "filter stop-band suppression", NULL);
    liquid_argparse_add(unsigned, num_samples, 240, 'n', "number of samples", NULL);
    liquid_argparse_parse(argc,argv);
    */

    // run all tests
    unsigned int i = 0;
    liquid_autotest autotest = liquid_registry[i++];
    do {
        printf("running test '%s' [%s]\n", autotest->docstr, autotest->keywords);
        autotest->func();
        autotest = liquid_registry[i++];
    } while (autotest != NULL);

    return 0;
}

