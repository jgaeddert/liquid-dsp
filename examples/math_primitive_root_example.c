char __docstr__[] =
"Demonstrates computing primitive root of a number using modular arithmetic.";

#include <stdio.h>
#include <stdlib.h>

#include "liquid.h"
#include "liquid.argparse.h"

int main(int argc, char*argv[])
{
    // define variables and parse command-line arguments
    liquid_argparse_init(__docstr__);
    liquid_argparse_parse(argc,argv);

    // maximum number
    unsigned int n = 140;
    unsigned int i;

    printf("primitive roots of prime numbers up to %u:\n", n);
    for (i=3; i<=n; i++) {
        if (!liquid_is_prime(i))
            continue;
        
        unsigned int root = liquid_primitive_root_prime(i);
        printf("  %4u : %4u\n", i, root);
    }

    return 0;
}
