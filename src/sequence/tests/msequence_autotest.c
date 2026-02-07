/*
 * Copyright (c) 2007 - 2026 Joseph Gaeddert
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

#include "liquid.autotest.h"
#include "liquid.internal.h"

LIQUID_AUTOTEST(bsequence_init_msequence,"test initialization of binary sequence on m-sequence","",0.1) {
    // create and initialize m-sequence
    msequence ms = msequence_create_default(4);

    // create and initialize binary sequence on m-sequence
    bsequence bs;
    bs = bsequence_create( msequence_get_length(ms) );
    bsequence_init_msequence(bs, ms);

    LIQUID_CHECK( bsequence_get_length(bs) ==  msequence_get_length(ms) )

    // clean up memory
    bsequence_destroy(bs);
    msequence_destroy(ms);
}

// helper function to test auto-correlation properties of m-sequence
void testbench_msequence_autocorrelation(liquid_autotest __q__, unsigned int _m)
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
    LIQUID_CHECK( bsequence_get_length(bs1) ==  n )
    LIQUID_CHECK( bsequence_get_length(bs2) ==  n )

    // when sequences are aligned, autocorrelation is equal to length
    int rxy;
    rxy = bsequence_correlate(bs1, bs2);
    LIQUID_CHECK( rxy ==  (int)n )

    // when sequences are misaligned, autocorrelation is equal to -1
    unsigned int i;
    for (i=0; i<n-1; i++) {
        bsequence_push(bs2, msequence_advance(ms));
        rxy = 2*bsequence_correlate(bs1, bs2) - n;
        LIQUID_CHECK( rxy ==  -1 );
    }

    // clean up memory
    bsequence_destroy(bs1);
    bsequence_destroy(bs2);
    msequence_destroy(ms);
}

LIQUID_AUTOTEST(msequence_xcorr_m2,"","",0.1)  { testbench_msequence_autocorrelation(__q__,2);  }   // n = 3
LIQUID_AUTOTEST(msequence_xcorr_m3,"","",0.1)  { testbench_msequence_autocorrelation(__q__,3);  }   // n = 7
LIQUID_AUTOTEST(msequence_xcorr_m4,"","",0.1)  { testbench_msequence_autocorrelation(__q__,4);  }   // n = 15
LIQUID_AUTOTEST(msequence_xcorr_m5,"","",0.1)  { testbench_msequence_autocorrelation(__q__,5);  }   // n = 31
LIQUID_AUTOTEST(msequence_xcorr_m6,"","",0.1)  { testbench_msequence_autocorrelation(__q__,6);  }   // n = 63
LIQUID_AUTOTEST(msequence_xcorr_m7,"","",0.1)  { testbench_msequence_autocorrelation(__q__,7);  }   // n = 127
LIQUID_AUTOTEST(msequence_xcorr_m8,"","",0.1)  { testbench_msequence_autocorrelation(__q__,8);  }   // n = 255
LIQUID_AUTOTEST(msequence_xcorr_m9,"","",0.1)  { testbench_msequence_autocorrelation(__q__,9);  }   // n = 511
LIQUID_AUTOTEST(msequence_xcorr_m10,"","",0.1) { testbench_msequence_autocorrelation(__q__,10); }   // n = 1023
LIQUID_AUTOTEST(msequence_xcorr_m11,"","",0.1) { testbench_msequence_autocorrelation(__q__,11); }   // n = 2047
LIQUID_AUTOTEST(msequence_xcorr_m12,"","",0.1) { testbench_msequence_autocorrelation(__q__,12); }   // n = 4095

// helper function to test cyclic period of sequences
void testbench_msequence_period(liquid_autotest __q__, unsigned int _m)
{
    // create and initialize m-sequence
    msequence q = msequence_create_default(_m);

    // measure period and compare to expected
    unsigned int n = (1U << _m) - 1;
    unsigned int p = msequence_measure_period(q);
    LIQUID_CHECK(p == n)

    // clean up objects
    msequence_destroy(q);
}

LIQUID_AUTOTEST(msequence_period_m2, "","", 0.01) { testbench_msequence_period(__q__,2);  }
LIQUID_AUTOTEST(msequence_period_m3, "","", 0.01) { testbench_msequence_period(__q__,3);  }
LIQUID_AUTOTEST(msequence_period_m4, "","", 0.01) { testbench_msequence_period(__q__,4);  }
LIQUID_AUTOTEST(msequence_period_m5, "","", 0.01) { testbench_msequence_period(__q__,5);  }
LIQUID_AUTOTEST(msequence_period_m6, "","", 0.01) { testbench_msequence_period(__q__,6);  }
LIQUID_AUTOTEST(msequence_period_m7, "","", 0.01) { testbench_msequence_period(__q__,7);  }
LIQUID_AUTOTEST(msequence_period_m8, "","", 0.01) { testbench_msequence_period(__q__,8);  }
LIQUID_AUTOTEST(msequence_period_m9, "","", 0.01) { testbench_msequence_period(__q__,9);  }
LIQUID_AUTOTEST(msequence_period_m10,"","", 0.01) { testbench_msequence_period(__q__,10); }
LIQUID_AUTOTEST(msequence_period_m11,"","", 0.01) { testbench_msequence_period(__q__,11); }
LIQUID_AUTOTEST(msequence_period_m12,"","", 0.01) { testbench_msequence_period(__q__,12); }
LIQUID_AUTOTEST(msequence_period_m13,"","", 0.01) { testbench_msequence_period(__q__,13); }
LIQUID_AUTOTEST(msequence_period_m14,"","", 0.01) { testbench_msequence_period(__q__,14); }
LIQUID_AUTOTEST(msequence_period_m15,"","", 0.01) { testbench_msequence_period(__q__,15); }
LIQUID_AUTOTEST(msequence_period_m16,"","", 0.01) { testbench_msequence_period(__q__,16); }
LIQUID_AUTOTEST(msequence_period_m17,"","", 0.01) { testbench_msequence_period(__q__,17); }
LIQUID_AUTOTEST(msequence_period_m18,"","", 0.01) { testbench_msequence_period(__q__,18); }
LIQUID_AUTOTEST(msequence_period_m19,"","", 0.01) { testbench_msequence_period(__q__,19); }
LIQUID_AUTOTEST(msequence_period_m20,"","", 0.01) { testbench_msequence_period(__q__,20); }
LIQUID_AUTOTEST(msequence_period_m21,"","", 0.01) { testbench_msequence_period(__q__,21); }
LIQUID_AUTOTEST(msequence_period_m22,"","", 0.01) { testbench_msequence_period(__q__,22); }
LIQUID_AUTOTEST(msequence_period_m23,"","", 0.03) { testbench_msequence_period(__q__,23); }
LIQUID_AUTOTEST(msequence_period_m24,"","", 0.07) { testbench_msequence_period(__q__,24); }
LIQUID_AUTOTEST(msequence_period_m25,"","", 0.16) { testbench_msequence_period(__q__,25); }
LIQUID_AUTOTEST(msequence_period_m26,"","", 0.30) { testbench_msequence_period(__q__,26); }
LIQUID_AUTOTEST(msequence_period_m27,"","", 0.60) { testbench_msequence_period(__q__,27); }
LIQUID_AUTOTEST(msequence_period_m28,"","", 1.20) { testbench_msequence_period(__q__,28); }
LIQUID_AUTOTEST(msequence_period_m29,"","", 2.42) { testbench_msequence_period(__q__,29); }
LIQUID_AUTOTEST(msequence_period_m30,"","", 4.85) { testbench_msequence_period(__q__,30); }
LIQUID_AUTOTEST(msequence_period_m31,"","", 9.68) { testbench_msequence_period(__q__,31); }

LIQUID_AUTOTEST(msequence_config,"","",0.1)
{
    _liquid_error_downgrade_enable();
    // check invalid configurations
    LIQUID_CHECK(NULL ==msequence_create        (100, 0, 0))
    LIQUID_CHECK(NULL ==msequence_create_default( 32)) // too long
    LIQUID_CHECK(NULL ==msequence_create_genpoly(  0))

    // create proper object and test configurations
    msequence q = msequence_create_genpoly(LIQUID_MSEQUENCE_GENPOLY_M11);

    LIQUID_CHECK(LIQUID_OK == msequence_print(q))
    LIQUID_CHECK(1 ==          msequence_get_state(q))
    LIQUID_CHECK(LIQUID_OK == msequence_set_state(q, 0x8a))
    LIQUID_CHECK(0x8a ==       msequence_get_state(q))

    msequence_destroy(q);
    _liquid_error_downgrade_disable();
}

