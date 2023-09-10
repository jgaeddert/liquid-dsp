// This example demonstrates logging capabilities with liquid,
// heavily influenced by: https://github.com/rxi/log.c
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "liquid.internal.h"

// user-defined callback
int test_callback(liquid_log_event event, void * context)
    { printf("  custom callback invoked! (%s)\n", event->time_str); return 0; }

// user-defined lock function
int test_lock(int _lock, void * _context)
    { printf("%s\n", _lock ? "locking" : "unlocking"); return 0; }

// sweep log levels
void sweep_levels(liquid_logger _q)
{
    int level;
    for (level=0; level<=LIQUID_FATAL; level++) {
        liquid_log(_q,level,__FILE__,__LINE__,"message with (%d) value", level);
        usleep(100000); // sleep to exercise different timestamps
    }
}

int main(int argc, char*argv[])
{
    liquid_log(NULL,LIQUID_INFO,__FILE__,__LINE__,"setting default logging (without color)");
    liquid_logger_set_config(NULL, LIQUID_LOG_DEFAULT);
    sweep_levels(NULL);

    liquid_log(NULL,LIQUID_INFO,__FILE__,__LINE__,"setting default logging (with color)");
    liquid_logger_set_config(NULL, LIQUID_LOG_DEFAULT | LIQUID_LOG_COLOR);
    sweep_levels(NULL);

    liquid_log(NULL,LIQUID_INFO,__FILE__,__LINE__,"setting concise logging");
    liquid_logger_set_config(NULL, LIQUID_LOG_CONCISE | LIQUID_LOG_COLOR);
    sweep_levels(NULL);

    liquid_log(NULL,LIQUID_INFO,__FILE__,__LINE__,"setting compact logging");
    liquid_logger_set_config(NULL, LIQUID_LOG_COMPACT | LIQUID_LOG_COLOR);
    sweep_levels(NULL);

    // test macro
    liquid_log_trace("testing trace logging with narg=%u", 42);
    liquid_logger_print(NULL);

    // test logging with custom object
    printf("\ntesting custom log:\n");
    char fname[] = "autotest/logs/logging_test.log";
    FILE * logfile = fopen(fname,"w");
    liquid_logger custom_log = liquid_logger_create();
    liquid_logger_set_level(custom_log, LIQUID_DEBUG);
    liquid_logger_set_lock(custom_log, test_lock, NULL);
    liquid_logger_add_callback(custom_log, test_callback, NULL, 0);
    liquid_logger_add_file    (custom_log, logfile, -1);
    liquid_log(custom_log,LIQUID_ERROR,__FILE__,__LINE__,"could not allocate memory for %u bytes", 1024);
    liquid_logger_print(custom_log);
    liquid_logger_destroy(custom_log);
    fclose(logfile);
    printf("output log written to %s\n", fname);

    return 0;
}

