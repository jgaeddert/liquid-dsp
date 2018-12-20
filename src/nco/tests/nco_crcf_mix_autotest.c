/*
 * Copyright (c) 2007 - 2018 Joseph Gaeddert
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

#include <stdlib.h>
#include <complex.h>
#include "autotest/autotest.h"
#include "liquid.h"

void autotest_nco_crcf_mix_block_up()
{
    // options
    unsigned int buf_len = 4096;
    float        phase   = 0.7123f;
    float        freq    = 0; //0.1324f;
    float        tol     = 1e-2f;

    // create object
    nco_crcf nco = nco_crcf_create(LIQUID_NCO);
    nco_crcf_set_phase    (nco, phase);
    nco_crcf_set_frequency(nco, freq);

    // generate signal
    float complex buf_0[buf_len];
    float complex buf_1[buf_len];
    unsigned int i;
    for (i=0; i<buf_len; i++)
        buf_0[i] = cexpf(_Complex_I*2*M_PI*randf());

    // mix signal
    nco_crcf_mix_block_up(nco, buf_0, buf_1, buf_len);

    // compare result to expected
    float phi = phase;
    for (i=0; i<buf_len; i++) {
        float complex v = buf_0[i] * cexpf(_Complex_I*phi);
        CONTEND_DELTA( crealf(buf_1[i]), crealf(v), tol);
        CONTEND_DELTA( cimagf(buf_1[i]), cimagf(v), tol);
        phi += freq;
    }

    // destroy object
    nco_crcf_destroy(nco);
}

