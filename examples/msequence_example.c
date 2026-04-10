const char __docstr__[] =
"This example demonstrates the property of maximal-length sequence"
" (m-sequence) linear feedback shift registers (LFSR) where the state"
" cycles through all permutations of integers from 1 to 2^m-1.";

#include <stdlib.h>
#include <stdio.h>
#ifndef _MSC_VER
#include <complex.h>
#endif
#include <math.h>

#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char* argv[])
{
    // define variables and parse command-line arguments
    liquid_argparse_init(__docstr__);
    //liquid_argparse_add(char*, filename, "msequence_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(unsigned, m, 5, 'm', "shift register length, m=2^m-1", NULL);
    liquid_argparse_parse(argc,argv);

    // create and initialize m-sequence
    msequence q = msequence_create_default(m);
    msequence_print(q);

    // cycle through values and print state
    unsigned int i;
    for (i=0; i<msequence_get_length(q); i++) {
        // verify we never hit the '1' state except on the first iteration
        if (i > 0 && msequence_get_state(q)==1) {
            printf("invalid state!\n");
            break;
        }
        printf("%u\n",msequence_get_state(q));
        msequence_advance(q);
    }

    // ensure final state is 1 (circled all the way back around)
    printf("final state (should be 1): %u\n", msequence_get_state(q));

    msequence_destroy(q);
    return 0;
}

