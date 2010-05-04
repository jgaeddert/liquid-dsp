//
// scramble_example.c
//
// Data-scrambling example.  Physical layer synchronization of
// received waveforms relies on independent and identically
// distributed underlying data symbols.  If the message sequence,
// however, is '00000....' and the modulation scheme is BPSK,
// the synchronizer probably won't be able to recover the symbol
// timing.  It is imperative to increase the entropy of the data
// for this to happen.  The data scrambler routine attempts to
// 'whiten' the data sequence with a bit mask in order to achieve
// maximum entropy.  This example demonstrates the interface.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "liquid.h"

int main() {
    unsigned int n=8;   // number of data bytes

    unsigned char x[n]; // input data
    unsigned char y[n]; // scrambled data
    unsigned char z[n]; // unscrambled data

    unsigned int i;

    // generate random data
    for (i=0; i<n; i++)
        x[i] = rand() & 0xff;

    // scramble input
    memmove(y,x,n);
    scramble_data(y,n);

    // unscramble result
    memmove(z,y,n);
    unscramble_data(z,n);

    // print results
    printf("i\tx\ty\tz\n");
    printf("--\t--\t--\t--\n");
    for (i=0; i<n; i++)
        printf("%u\t%x\t%x\t%x\n", i, x[i], y[i], z[i]);

    printf("done.\n");
    return 0;
}

