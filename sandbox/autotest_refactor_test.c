char __docstr__[] = "Test demonstration of new autotest macros and functions";

#include <stdio.h>
#include <math.h>
#include <complex.h>

#include "liquid.h"
#include "liquid.argparse.h"

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
    float   runtime;    // execution time [seconds]
};

// print test info
int liquid_autotest_print_info(liquid_autotest _q)
{
    printf("name=%s, description=%s, keywords=%s, cost=%g",
        _q->name, _q->docstr, _q->keywords, _q->cost);
    return LIQUID_OK;
}

// print test status
int liquid_autotest_print_status(liquid_autotest _q)
{
    printf("%s ", _q->name);
    printf("[%5u/%5u] ", _q->num_fail, _q->num_pass + _q->num_fail);
    unsigned int j;
    for (j=strlen(_q->name); j<50; j++)
        printf(".");
    printf("  ");
    switch(_q->status) {
    case LIQUID_AUTOTEST_PASS: printf("  pass  "); break;
    case LIQUID_AUTOTEST_FAIL: printf("<<FAIL>>"); break;
    case LIQUID_AUTOTEST_SKIP: printf(" (skip) "); break;
    default: return liquid_error(LIQUID_EINT,"unexpected status");
    }
    printf(" %7.2f sec", _q->runtime);
    return LIQUID_OK;
}

void liquid_autotest_pass(liquid_autotest _q)
{
    _q->num_pass++;
}

void liquid_autotest_fail(liquid_autotest _q,
                          const char *    _file,
                          unsigned int    _line,
                          const char *    _expression)
{
    printf("  TEST FAILED: %s:%u: %s\n", _file, _line, _expression);
    _q->num_fail++;
}

// expand macro
#define AUTOTEST__(F,L,X)                                           \
{                                                                   \
    if (!X)                                                         \
        liquid_autotest_fail(__q__,F,L,#X);                         \
    else                                                            \
        liquid_autotest_pass(__q__);                                \
}
#define AUTOTEST_(F,L,X)        AUTOTEST__(F,L,(X))     
#define AUTOTEST(X)             AUTOTEST_(__FILE__,__LINE__,X)

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
    static const liquid_autotest FUNC = &FUNC##_s;                              \
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
#else
LIQUID_AUTOTEST(firfilt_crcf_basic_0, "basic filter test", "FIR,filter,basic", 0.1)
{
    printf("firfilt_crcf_basic_0 test\n");
    AUTOTEST(2 < 7);
}
#endif


LIQUID_AUTOTEST(firfilt_crcf_basic_1, "basic filter test", "a,b,c", 0.1)
{
    printf("firfilt_crcf_basic_1 test\n");
    AUTOTEST(4 > 9);
}


LIQUID_AUTOTEST(firfilt_crcf_basic_2, "basic filter test", "a,b,c", 0.1)
{
    printf("firfilt_crcf_basic_2 test\n");
    AUTOTEST(4 > 6);
    AUTOTEST(5 > 6);
    AUTOTEST(6 > 6);
    AUTOTEST(7 > 6);
}

// define a registry as an array of tests. Note that we use the 'weak'
// attribute in case we want to link this file against another program
// to define a separate registry
#define LIQUID_AUTOTEST_REGISTRY \
    __attribute__((weak)) const liquid_autotest liquid_autotest_registry[]

// test list
LIQUID_AUTOTEST_REGISTRY =
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
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(bool, list, false, 'L', "list tests and exit", NULL);
    liquid_argparse_parse(argc,argv);

    if (list) {
        unsigned int i = 0;
        while (liquid_autotest_registry[i] != NULL)
        {
            printf("%3u : ", i+1);
            liquid_autotest_print_info(liquid_autotest_registry[i]);
            printf("\n");
            i++;
        }
        return 0;
    }

    // mark tests to run
    liquid_autotest_registry[0]->status = LIQUID_AUTOTEST_SCHED;
    liquid_autotest_registry[1]->status = LIQUID_AUTOTEST_SKIP;   // skip this test
    liquid_autotest_registry[2]->status = LIQUID_AUTOTEST_SCHED;

    // run all scheduled tests
    unsigned int i = 0;
    while (liquid_autotest_registry[i] != NULL)
    {
        liquid_autotest autotest = liquid_autotest_registry[i];
        if (autotest->status == LIQUID_AUTOTEST_SCHED)
        {
            printf("running test '%s' [%s]\n", autotest->docstr, autotest->keywords);
            autotest->status = LIQUID_AUTOTEST_ACTIVE;
            autotest->func(autotest);
            autotest->status = autotest->num_fail > 0 ? LIQUID_AUTOTEST_FAIL : LIQUID_AUTOTEST_PASS;
        } else if (autotest->status == LIQUID_AUTOTEST_SKIP) {
            printf("skipping test '%s'\n", autotest->docstr);
        }
        i++;
    }

    // print summary
    i = 0;
    while (liquid_autotest_registry[i] != NULL)
    {
        printf("%3u : ", i+1);
        liquid_autotest_print_status(liquid_autotest_registry[i]);
        printf("\n");
        i++;
    }

    return 0;
}

