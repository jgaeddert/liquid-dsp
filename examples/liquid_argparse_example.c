char __docstr__[] =
"This example demonstrates liquid's simplified method for creating"
" and parsing arguments for command-line options.";

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include "liquid.h"
#include "liquid.argparse.h"

int main(int argc, char*argv[])
{
    // ...
    liquid_argparse_init(__docstr__);
    // required?
    liquid_argparse_add(int,    verbose,    0,      'v', "enable verbosity", NULL);
    liquid_argparse_add(float,  sample_rate,12.0f,  'r', "sample rate in Hz", NULL);
    //liquid_argparse_add(string, mod_scheme, "qpsk", 'm', "modulation scheme", NULL);
    liquid_argparse_parse(argc,argv);

    printf("verbose = %d\n", verbose);

    return 0;
}
