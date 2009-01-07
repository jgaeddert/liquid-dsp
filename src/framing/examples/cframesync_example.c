//
//
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>

#include "../src/framing.h"
#include "../../sequence/src/sequence.h"// msequence
#include "../../random/src/random.h"    // awgn

int main() {
    // options
    unsigned int n=63;
    float phi = 0.3f;       // channel phase offset
    float snr_db = 10.0f;

    // create objects
    float snr = powf(10.0f, snr_db/10.0f); // TODO: scale snr for complex signal?
    cframesync fs = cframesync_create_msequence(LIQUID_MSEQUENCE_N63);
    msequence ms = msequence_create(6);

    unsigned int i;
    float complex s, rxy;

    for (i=0; i<n; i++) {
        // advance
        s = msequence_advance(ms) ? 1.0f : -1.0f;
        s *= cexpf(_Complex_I*phi); // phasor rotation
        s += crandnf() / snr;

        // correlate
        rxy = cframesync_correlate(fs,s);

        printf("%4u: mag: %12.8f, angle: %5.2f\n",i,cabsf(rxy),cargf(rxy));
    }

    // clean it up
    cframesync_destroy(fs);
    msequence_destroy(ms);

    printf("done.\n");
    return 0;
}

