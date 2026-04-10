const char __docstr__[] =
"This example demonstrates the circular buffer object on"
" floating-point data.";

#include <stdio.h>

#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char* argv[])
{
    // define variables and parse command-line arguments
    liquid_argparse_init(__docstr__);
    liquid_argparse_parse(argc,argv);

    float v[] = {1, 2, 3, 4, 5, 6, 7, 8};
    float *r; // reader
    unsigned int num_requested = 3;
    unsigned int num_read;

    cbufferf cb = cbufferf_create(10);

    cbufferf_write(cb, v, 4);
    cbufferf_read(cb, num_requested, &r, &num_read);
    printf("cbufferf: requested %u elements, read %u elements\n",
            num_requested, num_read);

    unsigned int i;
    for (i=0; i<num_read; i++)
        printf("  %u : %f\n", i, r[i]);

    // release 2 elements from the buffer
    cbufferf_release(cb, 2);

    // write 8 elements
    cbufferf_write(cb, v, 8);

    // print
    cbufferf_print(cb);

    // destroy object
    cbufferf_destroy(cb);

    printf("done.\n");
    return 0;
}


