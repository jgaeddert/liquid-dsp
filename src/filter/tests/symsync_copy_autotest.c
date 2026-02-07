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

LIQUID_AUTOTEST(symsync_copy,"test copying object", "", 0.1)
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
    LIQUID_CHECK(nw_0 ==  nw_1);

    // check output sample values
    LIQUID_CHECK_ARRAY(buf_0, buf_1, nw_0*sizeof(float complex));

    // check other internal properties
    LIQUID_CHECK(symsync_crcf_get_tau(q0) == symsync_crcf_get_tau(q1));

    // destroy objects
    symsync_crcf_destroy(q0);
    symsync_crcf_destroy(q1);
}

LIQUID_AUTOTEST(symsync_config,"test errors and invalid configuration", "", 0.1)
{
    _liquid_error_downgrade_enable();

    // test copying/creating invalid objects
    LIQUID_CHECK(NULL == symsync_crcf_copy(NULL) );

    LIQUID_CHECK(NULL == symsync_crcf_create(0, 12, NULL, 48) ); // k is too small
    LIQUID_CHECK(NULL == symsync_crcf_create(2,  0, NULL, 48) ); // M is too small
    LIQUID_CHECK(NULL == symsync_crcf_create(2, 12, NULL,  0) ); // h_len is too small
    LIQUID_CHECK(NULL == symsync_crcf_create(2, 12, NULL, 47) ); // h_len is not divisible by M

    LIQUID_CHECK(NULL == symsync_crcf_create_rnyquist(LIQUID_FIRFILT_RRC, 0, 12, 0.2, 48) ); // k is too small
    LIQUID_CHECK(NULL == symsync_crcf_create_rnyquist(LIQUID_FIRFILT_RRC, 2,  0, 0.2, 48) ); // m is too small
    LIQUID_CHECK(NULL == symsync_crcf_create_rnyquist(LIQUID_FIRFILT_RRC, 2, 12, 7.2, 48) ); // beta is too large
    LIQUID_CHECK(NULL == symsync_crcf_create_rnyquist(LIQUID_FIRFILT_RRC, 2, 12, 0.2,  0) ); // M is too small

    LIQUID_CHECK(NULL == symsync_crcf_create_kaiser(0, 12, 0.2, 48) ); // k is too small
    LIQUID_CHECK(NULL == symsync_crcf_create_kaiser(2,  0, 0.2, 48) ); // m is too small
    LIQUID_CHECK(NULL == symsync_crcf_create_kaiser(2, 12, 7.2, 48) ); // beta is too large
    LIQUID_CHECK(NULL == symsync_crcf_create_kaiser(2, 12, 0.2,  0) ); // M is too small

    // create valid object
    symsync_crcf q = symsync_crcf_create_kaiser(2, 12, 0.2, 48);
    LIQUID_CHECK( LIQUID_OK ==  symsync_crcf_print(q) );

    // check lock state
    LIQUID_CHECK(  symsync_crcf_lock(q) ==  LIQUID_OK );
    LIQUID_CHECK(  symsync_crcf_is_locked(q) );
    LIQUID_CHECK(  symsync_crcf_unlock(q) ==  LIQUID_OK );
    LIQUID_CHECK( !symsync_crcf_is_locked(q) );

    // check invalid properties
    LIQUID_CHECK( LIQUID_EICONFIG == symsync_crcf_set_output_rate(q, 0) );
    LIQUID_CHECK( LIQUID_EICONFIG == symsync_crcf_set_lf_bw(q, -1) );

    // destroy object
    symsync_crcf_destroy(q);
}

