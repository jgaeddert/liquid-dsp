char __docstr__[] =
"This example demonstrates logging capabilities with liquid.";
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "liquid.h"
#include "liquid.argparse.h"

// user-defined callback
int test_callback(liquid_log_event event, void * context, int config)
{
    printf("  custom callback invoked! (%s), config=0x%.8x, context=%s\n",
        event->time_str, config, (char*)context);
    return 0;
}

// user-defined lock function
int test_lock(int _lock, void * _context)
    { (void)_context; printf("%s\n", _lock ? "locking" : "unlocking"); return 0; }

// sweep log levels
void sweep_levels(liquid_logger _q)
{
    (void)_q;
    unsigned int sleeptime = 100000;
    usleep(sleeptime); liquid_log_trace("message at the level trace");
    usleep(sleeptime); liquid_log_debug("message at the level debug");
    usleep(sleeptime); liquid_log_info ("message at the level info");
    usleep(sleeptime); liquid_log_warn ("message at the level warn");
    usleep(sleeptime); liquid_log_error("message at the level error");
    usleep(sleeptime); liquid_log_fatal("message at the level fatal");
}

int main(int argc, char*argv[])
{
    liquid_logger_set_level(NULL,0);

    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    liquid_argparse_parse(argc,argv);

    printf("\nsetting default logging (without color)\n");
    liquid_logger_set_config(NULL, LIQUID_LOG_DEFAULT & ~LIQUID_LOG_COLOR);
    sweep_levels(NULL);

    printf("\nsetting compact logging\n");
    liquid_logger_set_config(NULL, LIQUID_LOG_COMPACT);
    sweep_levels(NULL);

    printf("\nsetting short logging (default)\n");
    liquid_logger_set_config(NULL, LIQUID_LOG_SHORT);
    sweep_levels(NULL);

    printf("\nsetting medium logging\n");
    liquid_logger_set_config(NULL, LIQUID_LOG_MEDIUM);
    sweep_levels(NULL);

    printf("\nsetting full logging\n");
    liquid_logger_set_config(NULL, LIQUID_LOG_FULL);
    sweep_levels(NULL);

    printf("\nsetting custom log format\n");
    liquid_logger_set_config(NULL, LIQUID_LOG_LEVEL_SHORT | LIQUID_LOG_LEVEL_BRACKETS | LIQUID_LOG_COLOR);
    sweep_levels(NULL);

    // test macro
    printf("\ntesting macro\n");
    liquid_log_trace("testing trace logging with narg=%u", 42);
    liquid_logger_print(NULL);

    // test logging with custom object
    printf("\ntesting custom log:\n");
    char fname[] = "logging_test.log";
    char context[] = "custom context";
    FILE * logfile = fopen(fname,"w");
    liquid_logger custom_log = liquid_logger_create();
    liquid_logger_set_level(custom_log, LIQUID_DEBUG);
    liquid_logger_set_config(custom_log, LIQUID_LOG_COMPACT | LIQUID_LOG_COLOR);
    liquid_logger_set_lock(custom_log, test_lock, NULL);
    liquid_logger_add_callback(custom_log, test_callback, context, 0);
    liquid_logger_add_file    (custom_log, logfile, -1);
    liquid_log(custom_log,LIQUID_ERROR,__FILE__,__LINE__,"could not allocate memory for %u bytes", 1024);
    liquid_logger_print(custom_log);
    liquid_logger_destroy(custom_log);
    fclose(logfile);
    printf("output log written to %s\n", fname);

    return 0;
}

