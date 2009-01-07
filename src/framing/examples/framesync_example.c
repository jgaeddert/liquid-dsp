//
//
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "../src/framing.h"
#include "../../sequence/src/sequence.h"// msequence
#include "../../random/src/random.h"    // awgn

int main() {
    // options
    unsigned int n=63;
    float snr_db = 10.0f;

    // create objects
    float snr = powf(10.0f, snr_db/10.0f);
    framesync fs = framesync_create_msequence(LIQUID_MSEQUENCE_N63);
    msequence ms = msequence_create(6);

    unsigned int i;
    float s, rxy;

    for (i=0; i<n; i++) {
        // advance
        s = msequence_advance(ms) ? 1.0f : -1.0f;
        s += randnf() / snr;

        // correlate
        rxy = framesync_correlate(fs,s);

        printf("%4u: %12.8f\n",i,rxy);
    }

    // clean it up
    framesync_destroy(fs);
    msequence_destroy(ms);

    printf("done.\n");
    return 0;
}

