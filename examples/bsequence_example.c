const char __docstr__[] =
"This example demonstrates the interface to the bsequence (binary"
" sequence) object.  The bsequence object acts like a buffer of bits"
" which are stored and manipulated efficiently in memory.";

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
    //liquid_argparse_add(char*, filename, "bsequence_example.m", 'o', "output filename", NULL);
    liquid_argparse_parse(argc,argv);

    // create and initialize binary sequence
    unsigned int n=16;
    bsequence q = bsequence_create(n);

    unsigned char v[4] = {0x35, 0x35};
    bsequence_init(q,v);

    bsequence_push(q,1);
    bsequence_push(q,1);
    bsequence_push(q,1);
    bsequence_push(q,1);

    bsequence_push(q,0);
    bsequence_push(q,1);

    bsequence_print(q);

    bsequence_circshift(q);
    bsequence_print(q);
    bsequence_circshift(q);
    bsequence_print(q);

    unsigned int b;
    unsigned int i;
    for (i=0; i<n; i++) {
        b = bsequence_index(q,i);
        printf("b[%3u] = %3u\n", i, b);
    }
    
    // clean up memory
    bsequence_destroy(q);

    return 0;
}

