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

// autotest helper function: measure delay assuming impulse
// - set gain to 1 for a single sample
// - set gain to 0 for remaining samples
//   -> this generates one symbol for the modulation scheme
// - take fft of resulting pulse
// - observe phase slope across pass-band
void testbench_symstreamrcf_delay(liquid_autotest __q__,
                                  float           _bw,
                                  unsigned int    _m)
{
    // create object and get expected delay
    int          ftype  = LIQUID_FIRFILT_ARKAISER;
    float        beta   = 0.30f;
    int          ms     = LIQUID_MODEM_QPSK;
    symstreamrcf gen    = symstreamrcf_create_linear(ftype,_bw,_m,beta,ms);
    float        delay  = symstreamrcf_get_delay(gen);
    float        tol    = 0.05; // error tolerance

    // compute buffer length based on delay
    unsigned int  nfft = 2*(120 + (unsigned int)(delay/sqrtf(_bw)));
    LIQUID_VLA(liquid_float_complex, buf_time, nfft);
    LIQUID_VLA(liquid_float_complex, buf_freq, nfft);

    // write samples to buffer
    symstreamrcf_write_samples(gen, buf_time, 1);
    symstreamrcf_set_gain(gen, 0.0f);
    symstreamrcf_write_samples(gen, buf_time+1, nfft-1);

    // destroy objects
    symstreamrcf_destroy(gen);

    // take forward transform
    fft_run(nfft, buf_time, buf_freq, LIQUID_FFT_FORWARD, 0);

    // measure phase slope across pass-band
    unsigned int m = 0.4 * _bw * nfft; // use 0.4 to account for filter roll-off
    liquid_float_complex p = 0.0f;
    int i;
    for (i=-(int)m; i<(int)m; i++)
        p += buf_freq[(nfft+i)%nfft] * conjf(buf_freq[(nfft+i+1)%nfft]);
    float delay_meas = cargf(p) * nfft / (2*M_PI);

    liquid_log_debug("expected delay: %.6f, measured: %.6f, error: %.6f (tol= %.3f)",
                delay, delay_meas, delay-delay_meas,tol);

    // verify delay is relatively close to expected
    LIQUID_CHECK_DELTA(delay, delay_meas, tol);
}

LIQUID_AUTOTEST(symstreamrcf_delay_00,"","",0.1) { testbench_symstreamrcf_delay(__q__, 0.500f, 4); }
LIQUID_AUTOTEST(symstreamrcf_delay_01,"","",0.1) { testbench_symstreamrcf_delay(__q__, 0.500f, 5); }
LIQUID_AUTOTEST(symstreamrcf_delay_02,"","",0.1) { testbench_symstreamrcf_delay(__q__, 0.500f, 6); }
LIQUID_AUTOTEST(symstreamrcf_delay_03,"","",0.1) { testbench_symstreamrcf_delay(__q__, 0.500f, 7); }
LIQUID_AUTOTEST(symstreamrcf_delay_04,"","",0.1) { testbench_symstreamrcf_delay(__q__, 0.500f, 8); }
LIQUID_AUTOTEST(symstreamrcf_delay_05,"","",0.1) { testbench_symstreamrcf_delay(__q__, 0.500f, 9); }
LIQUID_AUTOTEST(symstreamrcf_delay_06,"","",0.1) { testbench_symstreamrcf_delay(__q__, 0.500f,10); }
LIQUID_AUTOTEST(symstreamrcf_delay_07,"","",0.1) { testbench_symstreamrcf_delay(__q__, 0.500f,14); }
LIQUID_AUTOTEST(symstreamrcf_delay_08,"","",0.1) { testbench_symstreamrcf_delay(__q__, 0.500f,20); }
LIQUID_AUTOTEST(symstreamrcf_delay_09,"","",0.1) { testbench_symstreamrcf_delay(__q__, 0.500f,31); }

LIQUID_AUTOTEST(symstreamrcf_delay_10,"","",0.1) { testbench_symstreamrcf_delay(__q__, 0.800f,12); }
LIQUID_AUTOTEST(symstreamrcf_delay_11,"","",0.1) { testbench_symstreamrcf_delay(__q__, 0.700f,12); }
LIQUID_AUTOTEST(symstreamrcf_delay_12,"","",0.1) { testbench_symstreamrcf_delay(__q__, 0.600f,12); }
LIQUID_AUTOTEST(symstreamrcf_delay_13,"","",0.1) { testbench_symstreamrcf_delay(__q__, 0.500f,12); }
LIQUID_AUTOTEST(symstreamrcf_delay_14,"","",0.1) { testbench_symstreamrcf_delay(__q__, 0.400f,12); }
LIQUID_AUTOTEST(symstreamrcf_delay_15,"","",0.1) { testbench_symstreamrcf_delay(__q__, 0.300f,12); }
LIQUID_AUTOTEST(symstreamrcf_delay_16,"","",0.1) { testbench_symstreamrcf_delay(__q__, 0.200f,12); }
LIQUID_AUTOTEST(symstreamrcf_delay_17,"","",0.1) { testbench_symstreamrcf_delay(__q__, 0.100f,12); }
LIQUID_AUTOTEST(symstreamrcf_delay_18,"","",0.1) { testbench_symstreamrcf_delay(__q__, 0.050f,12); }
LIQUID_AUTOTEST(symstreamrcf_delay_19,"","",0.1) { testbench_symstreamrcf_delay(__q__, 0.025f,12); }

