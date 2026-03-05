char __docstr__[] =
"This example exercises all logging formats within liquid";

#include <stdlib.h>
#include <stdio.h>
#include "liquid.h"
#include "liquid.argparse.h"

int main(int argc, char*argv[])
{
    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(bool, rawtime,   false, 'r', "LIQUID_LOG_RAWTIME",        NULL);
    liquid_argparse_add(bool, datetime,  false, 'D', "LIQUID_LOG_DATETIME",       NULL);
    liquid_argparse_add(bool, date,      false, 'd', "LIQUID_LOG_DATE",           NULL);
    liquid_argparse_add(bool, time,      false, 't', "LIQUID_LOG_TIME",           NULL);
    liquid_argparse_add(bool, utc,       false, 'U', "LIQUID_LOG_UTC",            NULL);
    liquid_argparse_add(bool, ms,        false, 'm', "LIQUID_LOG_MS",             NULL);
    liquid_argparse_add(bool, us,        false, 'u', "LIQUID_LOG_US",             NULL);
    liquid_argparse_add(bool, ns,        false, 'n', "LIQUID_LOG_NS",             NULL);
    liquid_argparse_add(bool, full,      false, 'f', "LIQUID_LOG_LEVEL_FULL",     NULL);
    liquid_argparse_add(bool, short_,    false, 's', "LIQUID_LOG_LEVEL_SHORT",    NULL);
    liquid_argparse_add(bool, one,       false, 'o', "LIQUID_LOG_LEVEL_ONE",      NULL);
    liquid_argparse_add(bool, number,    false, 'N', "LIQUID_LOG_LEVEL_NUMBER",   NULL);
    liquid_argparse_add(bool, brackets,  false, 'B', "LIQUID_LOG_LEVEL_BRACKETS", NULL);
    liquid_argparse_add(bool, filename,  false, 'F', "LIQUID_LOG_FILENAME",       NULL);
    liquid_argparse_add(bool, filename_s,false, 'S', "LIQUID_LOG_FILENAME_SHORT", NULL);
    liquid_argparse_add(bool, filename_t,false, 'T', "LIQUID_LOG_FILENAME_TRUNC", NULL);
    liquid_argparse_add(bool, line,      false, 'L', "LIQUID_LOG_LINE",           NULL);
    liquid_argparse_add(bool, color,     false, 'C', "LIQUID_LOG_COLOR",          NULL);
    liquid_argparse_parse(argc,argv);

    // build config
    int config =
        (rawtime    ? LIQUID_LOG_RAWTIME        : 0) |
        (datetime   ? LIQUID_LOG_DATETIME       : 0) |
        (date       ? LIQUID_LOG_DATE           : 0) |
        (time       ? LIQUID_LOG_TIME           : 0) |
        (utc        ? LIQUID_LOG_UTC            : 0) |
        (ms         ? LIQUID_LOG_MS             : 0) |
        (us         ? LIQUID_LOG_US             : 0) |
        (ns         ? LIQUID_LOG_NS             : 0) |
        (full       ? LIQUID_LOG_LEVEL_FULL     : 0) |
        (short_     ? LIQUID_LOG_LEVEL_SHORT    : 0) |
        (one        ? LIQUID_LOG_LEVEL_ONE      : 0) |
        (number     ? LIQUID_LOG_LEVEL_NUMBER   : 0) |
        (brackets   ? LIQUID_LOG_LEVEL_BRACKETS : 0) |
        (filename   ? LIQUID_LOG_FILENAME       : 0) |
        (filename_s ? LIQUID_LOG_FILENAME_SHORT : 0) |
        (filename_t ? LIQUID_LOG_FILENAME_TRUNC : 0) |
        (line       ? LIQUID_LOG_LINE           : 0) |
        (color      ? LIQUID_LOG_COLOR          : 0);

    //
    liquid_logger_set_level(NULL,LIQUID_INFO);
    liquid_logger_set_config(NULL,config);
    liquid_log_info("here is a formatted log message");

    return 0;
}

