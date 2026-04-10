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

#include <stdio.h>
#include <stdlib.h>
#include "liquid.autotest.h"
#include "liquid.h"

// autotest helper function
void testbench_symstreamcf_delay(liquid_autotest __q__,
                                 unsigned int    _k,
                                 unsigned int    _m)
{
    // create object and get expected delay
    int          ftype  = LIQUID_FIRFILT_ARKAISER;
    float        beta   = 0.30f;
    int          ms     = LIQUID_MODEM_QPSK;
    symstreamcf  gen    = symstreamcf_create_linear(ftype,_k,_m,beta,ms);
    unsigned int delay  = symstreamcf_get_delay(gen);
    float        tol    = 2.0f + _k; // error tolerance (fairly wide due to random signal)

    unsigned int i;
    for (i=0; i<1000 + delay; i++) {
        // generate a single sample
        liquid_float_complex sample;
        symstreamcf_write_samples(gen, &sample, 1);

        // check to see if value exceeds 1
        if (cabsf(sample) > 0.9f)
            break;
    }

    liquid_log_debug("expected delay: %u, approximate delay: %u, tol: %.0f", delay, i, tol);

    // verify delay is relatively close to expected
    LIQUID_CHECK_DELTA((float)delay, (float)i, tol);

    // destroy objects
    symstreamcf_destroy(gen);
}

LIQUID_AUTOTEST(symstreamcf_delay_00,"","",0.1) { testbench_symstreamcf_delay(__q__,  2, 4); }
LIQUID_AUTOTEST(symstreamcf_delay_01,"","",0.1) { testbench_symstreamcf_delay(__q__,  2, 5); }
LIQUID_AUTOTEST(symstreamcf_delay_02,"","",0.1) { testbench_symstreamcf_delay(__q__,  2, 6); }
LIQUID_AUTOTEST(symstreamcf_delay_03,"","",0.1) { testbench_symstreamcf_delay(__q__,  2, 7); }
LIQUID_AUTOTEST(symstreamcf_delay_04,"","",0.1) { testbench_symstreamcf_delay(__q__,  2, 8); }
LIQUID_AUTOTEST(symstreamcf_delay_05,"","",0.1) { testbench_symstreamcf_delay(__q__,  2, 9); }
LIQUID_AUTOTEST(symstreamcf_delay_06,"","",0.1) { testbench_symstreamcf_delay(__q__,  2,10); }
LIQUID_AUTOTEST(symstreamcf_delay_07,"","",0.1) { testbench_symstreamcf_delay(__q__,  2,14); }
LIQUID_AUTOTEST(symstreamcf_delay_08,"","",0.1) { testbench_symstreamcf_delay(__q__,  2,20); }
LIQUID_AUTOTEST(symstreamcf_delay_09,"","",0.1) { testbench_symstreamcf_delay(__q__,  2,31); }

LIQUID_AUTOTEST(symstreamcf_delay_10,"","",0.1) { testbench_symstreamcf_delay(__q__,  3,12); }
LIQUID_AUTOTEST(symstreamcf_delay_11,"","",0.1) { testbench_symstreamcf_delay(__q__,  4,12); }
LIQUID_AUTOTEST(symstreamcf_delay_12,"","",0.1) { testbench_symstreamcf_delay(__q__,  5,12); }
LIQUID_AUTOTEST(symstreamcf_delay_13,"","",0.1) { testbench_symstreamcf_delay(__q__,  6,12); }
LIQUID_AUTOTEST(symstreamcf_delay_14,"","",0.1) { testbench_symstreamcf_delay(__q__,  7,12); }
LIQUID_AUTOTEST(symstreamcf_delay_15,"","",0.1) { testbench_symstreamcf_delay(__q__,  8,12); }
LIQUID_AUTOTEST(symstreamcf_delay_16,"","",0.1) { testbench_symstreamcf_delay(__q__,  9,12); }
LIQUID_AUTOTEST(symstreamcf_delay_17,"","",0.1) { testbench_symstreamcf_delay(__q__, 10,12); }
LIQUID_AUTOTEST(symstreamcf_delay_18,"","",0.1) { testbench_symstreamcf_delay(__q__, 11,12); }
LIQUID_AUTOTEST(symstreamcf_delay_19,"","",0.1) { testbench_symstreamcf_delay(__q__, 12,12); }

