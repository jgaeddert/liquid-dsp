//
// msequence_example.c
//
// This example demonstrates the auto-correlation properties of a
// maximal-length sequence (m-sequence).  An m-sequence of a
// certain length is used to generate two binary sequences
// (buffers) which are then cross-correlated.  The resulting
// correlation produces -1 for all values except at index zero,
// where the sequences align.
// SEE ALSO: bsequence_example.c
//

#include <stdio.h>
#include <complex.h>
#include <math.h>

#include "liquid.h"

//#define OUTPUT_FILENAME "msequence_example.m"

int main() {
    // options
    unsigned int m=5;   // shift register length, n=2^m - 1

    // create and initialize m-sequence
    msequence ms = msequence_create(m);
    msequence_print(ms);
    unsigned int n = msequence_get_length(ms);
    signed int rxx[n];   // auto-correlation
    
    // create and initialize first binary sequence on m-sequence
    bsequence bs1 = bsequence_create(n);
    bsequence_init_msequence(bs1, ms);

    // create and initialize second binary sequence on same m-sequence
    bsequence bs2 = bsequence_create(n);
    bsequence_init_msequence(bs2, ms);

    // when sequences are aligned, autocorrelation is equal to length
    unsigned int k=0;
    rxx[k++] = 2*bsequence_correlate(bs1, bs2) - n;

    // when sequences are misaligned, autocorrelation is equal to -1
    unsigned int i;
    for (i=0; i<n-1; i++) {
        bsequence_push(bs2, msequence_advance(ms));
        rxx[k++] = 2*bsequence_correlate(bs1, bs2)-n;
    }

    // print results
    for (i=0; i<n; i++)
        printf("rxx(%3u) = %3d\n", i, rxx[i]);

    // clean up memory
    bsequence_destroy(bs1);
    bsequence_destroy(bs2);
    msequence_destroy(ms);

    return 0;
}

