const char __docstr__[] =
"Demonstrates computing primitive root of a number using modular arithmetic.";

#include <stdio.h>
#include <stdlib.h>

#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char*argv[])
{
    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(unsigned, n, 140, 'n', "maximum number", NULL);
    liquid_argparse_parse(argc,argv);

    printf("primitive roots of prime numbers up to %u:\n", n);
    unsigned int i;
    for (i=3; i<=n; i++) {
        if (!liquid_is_prime(i))
            continue;
        
        unsigned int root = liquid_primitive_root_prime(i);
        printf("  %4u : %4u\n", i, root);
    }

    return 0;
}
