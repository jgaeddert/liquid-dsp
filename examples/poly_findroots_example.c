const char __docstr__[] = "Test polynomial root-finding algorithm";

#include <stdio.h>
#include <math.h>
#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char* argv[])
{
    // define variables and parse command-line arguments
    liquid_argparse_init(__docstr__);
    liquid_argparse_parse(argc,argv);

    unsigned int n=6;   // polynomial length (order+1)
    unsigned int i;

    // generate polynomial
    float p[6] = {6,11,-33,-33,11,6};
    LIQUID_VLA(liquid_float_complex, roots, n-1);

    // print polynomial
    printf("polynomial:\n");
    for (i=0; i<n; i++)
        printf("  p[%3u] = %12.4f\n", i, p[i]);

    // compute roots of polynomial
    polyf_findroots(p,n,roots);
    printf("roots:\n");
    for (i=0; i<n-1; i++) {
        printf("  r[%3u] = %8.5f + j*%8.5f\n",
            i, crealf(roots[i]), cimagf(roots[i]));
    }

    return 0;
}
