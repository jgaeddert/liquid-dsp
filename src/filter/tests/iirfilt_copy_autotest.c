/*
 * Copyright (c) 2007 - 2022 Joseph Gaeddert
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
#include "liquid.internal.h"

void testbench_iirfilt_copy(liquid_iirdes_format _format)
{
    // create base object
    iirfilt_crcf q0 = iirfilt_crcf_create_prototype(
        LIQUID_IIRDES_ELLIP, LIQUID_IIRDES_LOWPASS, _format,
        9,0.2f,0.0f,0.1f,60.0f);

    // start running input through filter
    unsigned int i, num_samples = 80;
    float complex y0, y1;
    for (i=0; i<num_samples; i++) {
        float complex v = randnf() + _Complex_I*randnf();
        iirfilt_crcf_execute(q0, v, &y0);
    }

    // copy filter
    iirfilt_crcf q1 = iirfilt_crcf_copy(q0);

    // continue running through both filters
    for (i=0; i<num_samples; i++) {
        float complex v = randnf() + _Complex_I*randnf();
        iirfilt_crcf_execute(q0, v, &y0);
        iirfilt_crcf_execute(q1, v, &y1);

        // compare result
        CONTEND_EQUALITY(y0, y1);
    }

    // destroy filter objects
    iirfilt_crcf_destroy(q0);
    iirfilt_crcf_destroy(q1);
}

void autotest_iirfilt_copy_tf () { testbench_iirfilt_copy(LIQUID_IIRDES_TF ); }
void autotest_iirfilt_copy_sos() { testbench_iirfilt_copy(LIQUID_IIRDES_SOS); }

