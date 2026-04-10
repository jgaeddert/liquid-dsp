const char __docstr__[] =
"This example demonstrates brute-force finding maximal-length sequence"
" (m-sequence) generator polynomials of a certain length.";

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char* argv[])
{
    // define variables and parse command-line arguments
    liquid_argparse_init(__docstr__);
    //liquid_argparse_add(char*, filename, "msequence_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(int, degree, 8, 'd', "degree", NULL);
    liquid_argparse_parse(argc,argv);

    unsigned int maxpoly = (1 << degree) - 1;
    unsigned int expected_sum = ((maxpoly + 1) / 2) * maxpoly;
    unsigned int i;
    unsigned int poly;
    for (poly = 0; poly <= maxpoly; ++poly) {
        unsigned int g = (poly << 1) + 1;
        msequence seq = msequence_create(degree, g, 1);
        unsigned int sum = 0;
        for (i = 0; i < maxpoly; ++i) {
            sum += msequence_get_state(seq);
            msequence_advance(seq);
        }
        if (sum == expected_sum) {
            printf("degree %d poly: %#06x\n", degree, g);
        }
        msequence_destroy(seq);
    }

    return 0;
}
