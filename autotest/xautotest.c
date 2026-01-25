char __docstr__[] = "Run all autotest programs in liquid-dsp";

// default include headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <time.h>
#include <sys/resource.h>

#include "liquid.internal.h"
#include "liquid.argparse.h"
#include "liquid.autotest.h"

// list of tests
#include "autotest/liquid_autotest_registry.h"

// autotest main
int main(int argc, char* argv[])
{
#if 1
    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(bool, list, false, 'L', "list tests and exit", NULL);
    liquid_argparse_parse(argc,argv);

    if (list)
        return liquid_registry_print(liquid_autotest_registry, true);

    // mark all to run
    unsigned int i = 0;
    while (liquid_autotest_registry[i] != NULL)
    {
        liquid_autotest_registry[i++]->status = LIQUID_AUTOTEST_SCHED;
    }

    // run all scheduled tests
    i = 0;
    while (liquid_autotest_registry[i] != NULL)
    {
        liquid_autotest autotest = liquid_autotest_registry[i];
        if (autotest->status == LIQUID_AUTOTEST_SCHED)
        {
            liquid_autotest_execute(autotest);
        } else if (autotest->status == LIQUID_AUTOTEST_SKIP) {
            //liquid_log_trace("skipping test '%s'\n", autotest->docstr);
        }
        i++;
    }

    // print summary
    liquid_registry_print(liquid_autotest_registry, false);
#else
    unsigned int i = 0;
    while (liquid_autotest_registry[i] != 0)
        printf("%d\n", liquid_autotest_registry[i++]);
#endif
    return 0;
}

