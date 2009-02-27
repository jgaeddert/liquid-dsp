//
//
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.h"

int main() {
    // options
    unsigned int n=63;
    float snr_db = 10.0f;

    // create objects
    float snr = powf(10.0f, snr_db/10.0f);
    pnsync_rrrf fs = pnsync_rrrf_create_msequence(LIQUID_MSEQUENCE_N63);
    msequence ms = msequence_create(6);

    unsigned int i;
    float s, rxy;

    for (i=0; i<n; i++) {
        // advance
        s = msequence_advance(ms) ? 1.0f : -1.0f;
        s += randnf() / snr;

        // correlate
        pnsync_rrrf_correlate(fs,s,&rxy);

        printf("%4u: %12.8f\n",i,rxy);
    }

    // clean it up
    pnsync_rrrf_destroy(fs);
    msequence_destroy(ms);

    printf("done.\n");
    return 0;
}

