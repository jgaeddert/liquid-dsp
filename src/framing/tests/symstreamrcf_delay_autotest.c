/*
 * Copyright (c) 2007 - 2021 Joseph Gaeddert
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
#include "autotest/autotest.h"
#include "liquid.h"

// autotest helper functions
void testbench_symstreamrcf_delay(float        _bw,
                                  unsigned int _m)
{
    // create object and get expected delay
    int          ftype  = LIQUID_FIRFILT_ARKAISER;
    float        beta   = 0.30f;
    int          ms     = LIQUID_MODEM_QPSK;
    symstreamrcf gen    = symstreamrcf_create_linear(ftype,_bw,_m,beta,ms);
    float        delay  = symstreamrcf_get_delay(gen);
    float        tol    = 2.0f + 1.0f/_bw; // error tolerance (fairly wide due to random signal)

    unsigned int i;
    for (i=0; i<1000 + (unsigned int)(delay); i++) {
        // generate a single sample
        float complex sample;
        symstreamrcf_write_samples(gen, &sample, 1);

        // check to see if value exceeds 1
        if (cabsf(sample) > 1.0f)
            break;
    }
    if (liquid_autotest_verbose)
        printf("expected delay: %.3f, approximate delay: %u, tol: %.3f\n", delay, i, tol);

    // verify delay is relatively close to expected
    CONTEND_DELTA((float)delay, (float)i, tol);

    // destroy objects
    symstreamrcf_destroy(gen);
}

void autotest_symstreamrcf_delay_00() { testbench_symstreamrcf_delay(0.500f, 4); }
void autotest_symstreamrcf_delay_01() { testbench_symstreamrcf_delay(0.500f, 5); }
void autotest_symstreamrcf_delay_02() { testbench_symstreamrcf_delay(0.500f, 6); }
void autotest_symstreamrcf_delay_03() { testbench_symstreamrcf_delay(0.500f, 7); }
void autotest_symstreamrcf_delay_04() { testbench_symstreamrcf_delay(0.500f, 8); }
void autotest_symstreamrcf_delay_05() { testbench_symstreamrcf_delay(0.500f, 9); }
void autotest_symstreamrcf_delay_06() { testbench_symstreamrcf_delay(0.500f,10); }
void autotest_symstreamrcf_delay_07() { testbench_symstreamrcf_delay(0.500f,14); }
void autotest_symstreamrcf_delay_08() { testbench_symstreamrcf_delay(0.500f,20); }
void autotest_symstreamrcf_delay_09() { testbench_symstreamrcf_delay(0.500f,31); }

void autotest_symstreamrcf_delay_10() { testbench_symstreamrcf_delay(0.800f,12); }
void autotest_symstreamrcf_delay_11() { testbench_symstreamrcf_delay(0.700f,12); }
void autotest_symstreamrcf_delay_12() { testbench_symstreamrcf_delay(0.600f,12); }
void autotest_symstreamrcf_delay_13() { testbench_symstreamrcf_delay(0.500f,12); }
void autotest_symstreamrcf_delay_14() { testbench_symstreamrcf_delay(0.400f,12); }
void autotest_symstreamrcf_delay_15() { testbench_symstreamrcf_delay(0.300f,12); }
void autotest_symstreamrcf_delay_16() { testbench_symstreamrcf_delay(0.200f,12); }
void autotest_symstreamrcf_delay_17() { testbench_symstreamrcf_delay(0.100f,12); }
void autotest_symstreamrcf_delay_18() { testbench_symstreamrcf_delay(0.050f,12); }
void autotest_symstreamrcf_delay_19() { testbench_symstreamrcf_delay(0.025f,12); }


