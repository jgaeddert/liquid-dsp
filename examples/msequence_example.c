// This example demonstrates the property of maximal-length sequence
// (m-sequence) linear feedback shift registers (LFSR) where the state
// cycles through all permutations of integers from 1 to 2^m-1.
#include <stdlib.h>
#include <stdio.h>
#include <complex.h>
#include <math.h>

#include "liquid.h"

int main(int argc, char*argv[])
{
    // create and initialize m-sequence
    msequence q = msequence_create_default(5);
    msequence_print(q);

    // cycle through values and print state
    unsigned int i;
    for (i=0; i<msequence_get_length(q); i++) {
        printf("%u\n",msequence_get_state(q));
        msequence_advance(q);
    }

    msequence_destroy(q);
    return 0;
}

