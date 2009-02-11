#ifndef __MSEQUENCE_AUTOTEST_H__
#define __MSEQUENCE_AUTOTEST_H__

#include "autotest/autotest.h"
#include "liquid.h"

// test initialization of binary sequence on m-sequence
void autotest_bsequence_init_msequence() {
    // create and initialize m-sequence
    msequence ms = msequence_create(4);
    
    // create and initialize binary sequence on m-sequence
    bsequence bs;
    bs = bsequence_create( msequence_get_length(ms) );
    bsequence_init_msequence(bs, ms);
    bsequence_print(bs);

    CONTEND_EQUALITY( bsequence_get_length(bs), msequence_get_length(ms) )

    // clean up memory
    bsequence_destroy(bs);
    msequence_destroy(ms);
}

// test auto-correlation properties of m-sequence
void autotest_msequence_rxy() {
    // create and initialize m-sequence
    msequence ms = msequence_create(4);
    unsigned int n = msequence_get_length(ms);
    
    // create and initialize first binary sequence on m-sequence
    bsequence bs1 = bsequence_create(n);
    bsequence_init_msequence(bs1, ms);

    // create and initialize second binary sequence on same m-sequence
    bsequence bs2 = bsequence_create(n);
    bsequence_init_msequence(bs2, ms);

    // ensure sequences are the same length
    CONTEND_EQUALITY( bsequence_get_length(bs1), n )
    CONTEND_EQUALITY( bsequence_get_length(bs2), n )

    // when sequences are aligned, autocorrelation is equal to length
    signed int rxy;
    rxy = bsequence_correlate(bs1, bs2);
    CONTEND_EQUALITY( rxy, n )

    // when sequences are misaligned, autocorrelation is equal to -1
    unsigned int i;
    for (i=0; i<n-1; i++) {
        bsequence_push(bs2, msequence_advance(ms));
        rxy = bsequence_correlate(bs1, bs2);
        CONTEND_EQUALITY( rxy, -1 );
    }

    // clean up memory
    bsequence_destroy(bs1);
    bsequence_destroy(bs2);
    msequence_destroy(ms);
}

#endif 

