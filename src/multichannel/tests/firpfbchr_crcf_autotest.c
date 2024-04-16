/*
 * Copyright (c) 2007 - 2024 Joseph Gaeddert
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

#include <assert.h>
#include "autotest/autotest.h"
#include "liquid.h"

void autotest_firpfbchr_crcf_config()
{
#if LIQUID_STRICT_EXIT
    AUTOTEST_WARN("skipping firpfbchr_crcf config test with strict exit enabled\n");
    return;
#endif
#if !LIQUID_SUPPRESS_ERROR_OUTPUT
    fprintf(stderr,"warning: ignore potential errors here; checking for invalid configurations\n");
#endif
    // design prototype filter
    unsigned int h_len = 2*64*12+1;
    float h[2*64*12+1];
    liquid_firdes_kaiser(h_len, 0.1f, 60.0f, 0.0f, h);

    // check invalid function calls
    CONTEND_ISNULL(firpfbchr_crcf_create( 0, 76, 12,    h)) // too few channels
    CONTEND_ISNULL(firpfbchr_crcf_create(64,  0, 12,    h)) // decimation rate too small
    CONTEND_ISNULL(firpfbchr_crcf_create(64, 76,  0,    h)) // filter delay too small
    CONTEND_ISNULL(firpfbchr_crcf_create(64, 76, 12, NULL)) // coefficients pointer set to NULL

    // kaiser
    CONTEND_ISNULL(firpfbchr_crcf_create_kaiser( 0, 76, 12, 60.0f)) // too few channels
    CONTEND_ISNULL(firpfbchr_crcf_create_kaiser(64,  0, 12, 60.0f)) // decimation rate too small
    CONTEND_ISNULL(firpfbchr_crcf_create_kaiser(64, 76,  0, 60.0f)) // filter delay too small
    CONTEND_ISNULL(firpfbchr_crcf_create_kaiser(64, 76, 12, -1.0f)) // stop-band suppression out of range

    //CONTEND_ISNULL(firpfbchr_crcf_copy(NULL))

    // create proper object and test configurations
    firpfbchr_crcf q = firpfbchr_crcf_create_kaiser(64, 76, 12, 60.0f);

    CONTEND_EQUALITY(LIQUID_OK, firpfbchr_crcf_print(q))

    firpfbchr_crcf_destroy(q);
}

