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
    // set default logging level
    liquid_logger_set_level(NULL, LIQUID_INFO);

    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*,logfile, "", 'g', "output logfile", NULL);
    liquid_argparse_add(bool, list, false, 'L', "list tests and exit", NULL);
    liquid_argparse_add(int,  test,    -1, 't', "run a specific test", NULL);
    liquid_argparse_parse(argc,argv);

    FILE * fid = NULL;
    if (strcmp(logfile,""))
    {
        fid = fopen(logfile,"w");
        liquid_logger_add_file(NULL,fid,LIQUID_INFO);
    }

    unsigned int i = 0;
    if (list) {
        i = 0;
        while (liquid_autotest_registry[i] != NULL)
            liquid_autotest_print_info(liquid_autotest_registry[i++]);
        return LIQUID_OK;
    }

    // mark tests to run
    i = 0;
    while (liquid_autotest_registry[i] != NULL)
    {
        if (test < 0)
            liquid_autotest_registry[i]->status = LIQUID_AUTOTEST_SCHED;
        else
            liquid_autotest_registry[i]->status = (i==test) ? LIQUID_AUTOTEST_SCHED : LIQUID_AUTOTEST_SKIP;
        i++;
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
    return liquid_registry_print(liquid_autotest_registry);
}

