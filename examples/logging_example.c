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

int main(int argc, char*argv[])
{
    int level;
    for (level=0; level<=LIQUID_FATAL; level++) {
        liquid_log(NULL,level,__FILE__,__LINE__,"message with (%d) value", level);
        usleep(100000);
    }

    // test macro
    liquid_log_trace("testing trace logging with narg=%u", 42);
    liquid_logger_print(NULL);

    // test logging with custom object
    printf("\ntesting custom log:\n");
    char fname[] = "autotest/logs/logging_test.log";
    FILE * logfile = fopen(fname,"w");
    liquid_logger custom_log = liquid_logger_create();
    liquid_logger_set_level(custom_log, LIQUID_DEBUG);
    liquid_logger_add_callback(custom_log, test_callback, NULL, 0);
    liquid_logger_add_file    (custom_log, logfile, -1);
    liquid_log(custom_log,LIQUID_ERROR,__FILE__,__LINE__,"could not allocate memory for %u bytes", 1024);
    liquid_logger_print(custom_log);
    liquid_logger_destroy(custom_log);
    fclose(logfile);
    printf("output log written to %s\n", fname);

    return 0;
}

