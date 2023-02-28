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
void autotest_iirinterp_copy()
{
    // create base object
    iirinterp_crcf q0 = iirinterp_crcf_create_default(3, 7);
    //iirinterp_crcf_set_scale(q0, 0.12345f);

    // run samples through filter
    unsigned int i;
    float complex buf_0[3], buf_1[3];
    for (i=0; i<20; i++) {
        float complex v = randnf() + _Complex_I*randnf();
        iirinterp_crcf_execute(q0, v, buf_0);
    }

    // copy object
    iirinterp_crcf q1 = iirinterp_crcf_copy(q0);

    // run samples through both filters in parallel
    for (i=0; i<60; i++) {
        float complex v = randnf() + _Complex_I*randnf();
        iirinterp_crcf_execute(q0, v, buf_0);
        iirinterp_crcf_execute(q1, v, buf_1);

        CONTEND_SAME_DATA( buf_0, buf_1, 3*sizeof(float complex) );
    }

    // destroy objects
    iirinterp_crcf_destroy(q0);
    iirinterp_crcf_destroy(q1);
}

#if 0
void xautotest_iirinterp_rrrf_common()
{
    iirinterp_rrrf interp = iirinterp_rrrf_create_kaiser(17, 4, 60.0f);
    CONTEND_EQUALITY(iirinterp_rrrf_get_interp_rate(interp), 17);
    iirinterp_rrrf_destroy(interp);
}

void xautotest_iirinterp_crcf_common()
{
    iirinterp_crcf interp = iirinterp_crcf_create_kaiser(7, 4, 60.0f);
    CONTEND_EQUALITY(iirinterp_crcf_get_interp_rate(interp), 7);
    iirinterp_crcf_destroy(interp);
}
#endif

