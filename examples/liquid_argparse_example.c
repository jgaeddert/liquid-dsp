char __docstr__[] =
"This example demonstrates liquid's simplified method for creating"
" and parsing arguments for command-line options.";

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include "liquid.h"
#include "liquid.argparse.h"

int callback(const char * _optarg, void * _ref)
{
    printf("custom callback: _optarg=%s\n", _optarg);
    if (strcmp(_optarg,"true")==0)
        *(int*)(_ref) = 1;
    else if (strcmp(_optarg,"false")==0)
        *(int*)(_ref) = 0;
    else {
        fprintf(stderr,"could not convert; expected 'true' or 'false'\n");
        return -1;
    }
    return 0;
}

int main(int argc, char*argv[])
{
    // create argument parser using macros to define variables and set values from command line
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(int,   verbose,     0,      'v', "enable verbosity",  NULL);
    liquid_argparse_add(float, sample_rate, 12.0f,  'r', "sample rate in Hz", NULL);
    liquid_argparse_add(char*, mod_scheme,  "qpsk", 'm', "modulation scheme", NULL);
    liquid_argparse_add(int,   custom,      -1,     'c', "custom option, valid values are {true,false}", callback);
    liquid_argparse_parse(argc,argv);

    // print values
    printf("verbose = %d\n", verbose);
    printf("sample rate = %f\n", sample_rate);
    printf("mod scheme = '%s'\n", mod_scheme);
    printf("custom = %d\n", custom);

    return 0;
}
