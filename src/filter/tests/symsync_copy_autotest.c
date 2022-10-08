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

// test copying object
void autotest_symsync_copy()
{
    // create base object
    symsync_crcf q0 = symsync_crcf_create_rnyquist(
            LIQUID_FIRFILT_ARKAISER, 5, 7, 0.25, 64);
    symsync_crcf_set_lf_bw(q0,0.02f);

    // run samples through filter
    // NOTE: we don't care that the input is noise; just that both objects
    //       produce the same output
    unsigned int i, nw_0, nw_1, buf_len = 640;
    float complex buf  [buf_len];
    float complex buf_0[buf_len];
    float complex buf_1[buf_len];
    for (i=0; i<buf_len; i++)
        buf[i] = randnf() + _Complex_I*randnf();

    symsync_crcf_execute(q0, buf, buf_len, buf_0, &nw_0);

    // copy object
    symsync_crcf q1 = symsync_crcf_copy(q0);

    // run samples through both filters and check equality
    for (i=0; i<buf_len; i++)
        buf[i] = randnf() + _Complex_I*randnf();

    symsync_crcf_execute(q0, buf, buf_len, buf_0, &nw_0);
    symsync_crcf_execute(q1, buf, buf_len, buf_1, &nw_1);

    // check that the same number of samples were written
    CONTEND_EQUALITY(nw_0, nw_1);

    // check output sample values
    CONTEND_SAME_DATA(buf_0, buf_1, nw_0*sizeof(float complex));

    // check other internal properties
    CONTEND_EQUALITY(symsync_crcf_get_tau(q0),symsync_crcf_get_tau(q1));

    // destroy objects
    symsync_crcf_destroy(q0);
    symsync_crcf_destroy(q1);
}

