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
    //liquid_logger_set_config(NULL, LIQUID_LOG_FULL | LIQUID_LOG_COLOR);

    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(int,  test_id,       -1, 't', "run a specific test", NULL);
    //liquid_argparse_add(int,  hammer_id,     -1, 'H', "[TODO] hammer on a specific test", NULL);
    //liquid_argparse_add(int,  hammer_count, 100, 'c', "[TODO] number of times to run hammer test", NULL);
    liquid_argparse_add(int,  random_seed,   -1, 'R', "specify random seed value", NULL);
    liquid_argparse_add(bool, list,       false, 'l', "list tests and exit", NULL);
    liquid_argparse_add(bool, stop_fail,  false, 'x', "stop on fail", NULL);
    liquid_argparse_add(char*,search,        "", 's', "run tests with search string in name", NULL);
    liquid_argparse_add(char*,json,          "", 'o', "output JSON file", NULL);
    liquid_argparse_add(char*,logfile,       "", 'g', "output logfile", NULL);
    liquid_argparse_add(bool, status,  false, 'P', "print full status of all at the end", NULL);
    liquid_argparse_parse(argc,argv);

    if (strcmp(logfile,""))
        liquid_logger_add_filename(NULL,logfile,LIQUID_INFO);

    if (random_seed < 0)
        random_seed = time(NULL);
    srand(random_seed);

    int i = 0;
    if (list) {
        while (liquid_autotest_registry[i] != NULL)
        {
            liquid_autotest_print_info(liquid_autotest_registry[i],i);
            i++;
        }
        return LIQUID_OK;
    }

    // create autotest registry object
    liquid_registry registry = liquid_registry_create(liquid_autotest_registry);

    // schedule tests to run (default: all)
    if (test_id >= 0) {
        liquid_registry_schedule_one(registry, test_id);
    } else if (strlen(search) > 0) {
        liquid_registry_schedule_search(registry, search);
    }

    // run tests, stopping on failure if requested
    liquid_registry_execute(registry, stop_fail);

    // print summary
    liquid_registry_print_status(registry);

    // print status of executed tests
    if (status)
        liquid_registry_print_status(registry);

    // print summary
    int rc = liquid_registry_print_summary(registry);

    // export JSON if requested
    if (strcmp(json,""))
    {
        // try to open output file for writing
        FILE * fid = fopen(json,"w");
        if (fid == NULL)
            return liquid_error(LIQUID_EIO,"could not open '%s' for writing", json);

        // print header
        time_t now;
        time(&now);
        char timestamp[80];
        strftime(timestamp,80,"%c",localtime(&now));
        fprintf(fid,"{\n");
        fprintf(fid,"  \"build-info\" : {},\n");
        fprintf(fid,"  \"timestamp\" : \"%s\",\n", timestamp);
        fprintf(fid,"  \"command-line\" : \"");
        for (i=0; i<argc; i++)
            fprintf(fid," %s", argv[i]);
        fprintf(fid,"\",\n");
        fprintf(fid,"  \"rseed\" : %u,\n", random_seed);
        fprintf(fid,"  \"stop-on-fail\" : %s,\n", stop_fail ? "true" : "false");

        // print registry results
        liquid_registry_json(registry, fid);

        // finalize JSON output
        fprintf(fid,"}\n");
        fclose(fid);
        liquid_log_info("output JSON results written to %s", json);
    }

    // destroy registry
    liquid_registry_destroy(registry);

    return rc;
}

