/*
 * Copyright (c) 2007 - 2015 Joseph Gaeddert
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "autotest/autotest.h"
#include "liquid.h"

// test initialization of binary sequence on m-sequence
void autotest_bsequence_init_msequence() {
    // create and initialize m-sequence
    msequence ms = msequence_create_default(4);
    
    // create and initialize binary sequence on m-sequence
    bsequence bs;
    bs = bsequence_create( msequence_get_length(ms) );
    bsequence_init_msequence(bs, ms);
    if (liquid_autotest_verbose)
        bsequence_print(bs);

    CONTEND_EQUALITY( bsequence_get_length(bs), msequence_get_length(ms) )

    // clean up memory
    bsequence_destroy(bs);
    msequence_destroy(ms);
}

// helper function to test auto-correlation properties of m-sequence
void msequence_test_autocorrelation(unsigned int _m)
{
    // create and initialize m-sequence
    msequence ms = msequence_create_default(_m);
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
        rxy = 2*bsequence_correlate(bs1, bs2) - n;
        CONTEND_EQUALITY( rxy, -1 );
    }

    // clean up memory
    bsequence_destroy(bs1);
    bsequence_destroy(bs2);
    msequence_destroy(ms);
}

void autotest_msequence_m2()    {   msequence_test_autocorrelation(2);  }   // n = 3
void autotest_msequence_m3()    {   msequence_test_autocorrelation(3);  }   // n = 7
void autotest_msequence_m4()    {   msequence_test_autocorrelation(4);  }   // n = 15
void autotest_msequence_m5()    {   msequence_test_autocorrelation(5);  }   // n = 31
void autotest_msequence_m6()    {   msequence_test_autocorrelation(6);  }   // n = 63
void autotest_msequence_m7()    {   msequence_test_autocorrelation(7);  }   // n = 127
void autotest_msequence_m8()    {   msequence_test_autocorrelation(8);  }   // n = 255
void autotest_msequence_m9()    {   msequence_test_autocorrelation(9);  }   // n = 511
void autotest_msequence_m10()   {   msequence_test_autocorrelation(10); }   // n = 1023
void autotest_msequence_m11()   {   msequence_test_autocorrelation(11); }   // n = 2047
void autotest_msequence_m12()   {   msequence_test_autocorrelation(12); }   // n = 4095

