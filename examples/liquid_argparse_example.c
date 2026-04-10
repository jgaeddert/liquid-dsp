const char __docstr__[] =
"This example demonstrates liquid's simplified method for creating"
" and parsing arguments for command-line options.";

#include "liquid.h"
#include "liquid_vla.h"
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
    liquid_argparse_add(char*, filename, "test.out",'o', "output filename",      NULL);
    liquid_argparse_add(bool,  verbose,     false,  'v', "enable verbose mode",  NULL);
    liquid_argparse_add(int,   iterations,  0,      'i', "number of iterations", NULL);
    liquid_argparse_add(float, sample_rate, 12.0f,  'r', "sample rate in Hz",    NULL);
    liquid_argparse_add(double,center_freq, 2450e6, 'f', "center freq in Hz",    NULL);
    liquid_argparse_add(char*, mod_scheme,  "qpsk", 'm', "modulation scheme",    liquid_argparse_modem);
    liquid_argparse_add(int,   custom,      -1,     'c', "custom option, valid values are {true,false}", callback);
    liquid_argparse_parse(argc,argv);

    // print values
    printf("filename    = %s\n", filename);
    printf("verbose     = %s\n", verbose ? "true" : "false");
    printf("iterations  = %d\n", iterations);
    printf("sample rate = %f\n", sample_rate);
    printf("center freq = %g\n", center_freq);
    printf("mod scheme  = '%s'\n", mod_scheme);
    printf("custom      = %d\n", custom);

    return 0;
}
