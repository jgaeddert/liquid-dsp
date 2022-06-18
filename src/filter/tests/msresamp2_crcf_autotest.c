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
#include "liquid.h"

// test copy method
void xautotest_msresamp2_copy()
{
    // create original resampler
    unsigned int num_stages = 4;
    msresamp2_crcf q0 = msresamp2_crcf_create(
        LIQUID_RESAMP_INTERP, num_stages, 0.4f, 0.0f, 60.0f);

    // allocate buffers for output
    unsigned int M = 1 << num_stages; // interpolation factor
    float complex v, y0[M], y1[M];

    // push samples through original object
    unsigned int i, num_samples = 35;
    for (i=0; i<num_samples; i++) {
        v = randnf() + _Complex_I*randnf();
        msresamp2_crcf_execute(q0, &v, y0);
    }

    // copy object
    msresamp2_crcf q1 = msresamp2_crcf_copy(q0);

    // run random samples through both filters and compare
    for (i=0; i<num_samples; i++) {
        v = randnf() + _Complex_I*randnf();
        msresamp2_crcf_execute(q0, &v, y0);
        msresamp2_crcf_execute(q1, &v, y1);
        CONTEND_SAME_DATA(y0, y1, M*sizeof(float complex));
    }

    // clean up allocated objects
    msresamp2_crcf_destroy(q0);
    msresamp2_crcf_destroy(q1);
}

