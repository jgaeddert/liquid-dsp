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
        LIQUID_AUTOTEST_INIT = 0,   // test has been initialized
        // ...
        LIQUID_AUTOTEST_SCHED= 1,   // test has been scheduled to run
        LIQUID_AUTOTEST_ACTIVE=2,   // test is actively running
        LIQUID_AUTOTEST_PASS = 3,   // test finished: result = passed
        LIQUID_AUTOTEST_FAIL = 4,   // test finished: result = failed
        LIQUID_AUTOTEST_SKIP = 5,   // test skipped
    } status;
    int     num_tests;  // number of actual evaluations run
    int     num_pass;   // number of tests passsed
    int     num_fail;   // number of tests failed
};

typedef struct liquid_autotest_s * liquid_autotest;

// initialize autotest harness
// define liquid_autotest(...) __liquid_autotest_internal( __VA_ARGS__ )
#define LIQUID_AUTOTEST(FUNC, DOCSTR, KEYWORDS, COST)                           \
    /* forward declaration of test function                                 */  \
    void FUNC##_autotest(void);                                                 \
    /* define structure                                                     */  \
    struct liquid_autotest_s FUNC##_s = {                                       \
        #FUNC,              /* test name                                    */  \
        FUNC##_autotest,    /* function pointer                             */  \
        DOCSTR,             /* user-defined documentation string            */  \
        KEYWORDS,           /* string representing comma-separated keywords */  \
        COST,               /* cost estimate (runtime) for executing test   */  \
        LIQUID_AUTOTEST_INIT, 0, 0, 0,                                          \
    };                                                                          \
    /* define pointer to struct */                                              \
    static const liquid_autotest FUNC = &FUNC##_s;                              \
    /* define function */                                                       \
    void FUNC##_autotest(void)  /* continue with function definition */  \

#if 0
// this is how a test should get expanded by the macro

// forward declaration of test function
void firfilt_crcf_basic_0_autotest(void);
// definte struct
struct liquid_autotest_s firfilt_crcf_basic_0_s = {
    "firfilt_crcf_basic_0", // test name
    firfilt_crcf_basic_0_autotest,
    "basic filter test",
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
#else
LIQUID_AUTOTEST(firfilt_crcf_basic_0, "basic filter test", "FIR,filter,basic", 0.1)
{
    printf("firfilt_crcf_basic_0 test\n");
}
#endif


LIQUID_AUTOTEST(firfilt_crcf_basic_1, "basic filter test", "a,b,c", 0.1)
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

    // mark tests to run
    liquid_registry[0]->status = LIQUID_AUTOTEST_SCHED;
    liquid_registry[1]->status = LIQUID_AUTOTEST_SKIP;   // skip this test
    liquid_registry[2]->status = LIQUID_AUTOTEST_SCHED;

    // run all scheduled tests
    unsigned int i = 0;
    while (liquid_registry[i] != NULL)
    {
        liquid_autotest autotest = liquid_registry[i];
        if (autotest->status == LIQUID_AUTOTEST_SCHED)
        {
            printf("running test '%s' [%s]\n", autotest->docstr, autotest->keywords);
            autotest->func();
        } else if (autotest->status == LIQUID_AUTOTEST_SKIP) {
            printf("skipping test '%s'\n", autotest->docstr);
        }
        i++;
    }

    return 0;
}

