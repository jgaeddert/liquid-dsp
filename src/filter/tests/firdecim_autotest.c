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

void autotest_firdecim_config()
{
#if LIQUID_STRICT_EXIT
    AUTOTEST_WARN("skipping firdecim config test with strict exit enabled\n");
    return;
#endif
#if !LIQUID_SUPPRESS_ERROR_OUTPUT
    fprintf(stderr,"warning: ignore potential errors here; checking for invalid configurations\n");
#endif
    // design filter
    unsigned int M     =  4;
    unsigned int m     = 12;
    unsigned int h_len =  2*M*m+1;
    float        h[h_len];
    int          wtype = LIQUID_WINDOW_HAMMING;
    liquid_firdes_windowf(wtype, h_len, 0.2f, 0, h);

    // check that estimate methods return zero for invalid configs
    CONTEND_ISNULL(firdecim_crcf_create(0, h, h_len)); // M cannot be 0
    CONTEND_ISNULL(firdecim_crcf_create(M, h,     0)); // h_len cannot be 0

    CONTEND_ISNULL(firdecim_crcf_create_kaiser(1, 12, 60.0f)); // M too small
    CONTEND_ISNULL(firdecim_crcf_create_kaiser(4,  0, 60.0f)); // m too small
    CONTEND_ISNULL(firdecim_crcf_create_kaiser(4, 12, -2.0f)); // As too small

    CONTEND_ISNULL(firdecim_crcf_create_prototype(LIQUID_FIRFILT_UNKNOWN, 4, 12, 0.3f, 0.0f));
    CONTEND_ISNULL(firdecim_crcf_create_prototype(LIQUID_FIRFILT_RCOS,    1, 12, 0.3f, 0.0f));
    CONTEND_ISNULL(firdecim_crcf_create_prototype(LIQUID_FIRFILT_RCOS,    4,  0, 0.3f, 0.0f));
    CONTEND_ISNULL(firdecim_crcf_create_prototype(LIQUID_FIRFILT_RCOS,    4, 12, 7.2f, 0.0f));
    CONTEND_ISNULL(firdecim_crcf_create_prototype(LIQUID_FIRFILT_RCOS,    4, 12, 0.3f, 4.0f));

    // create valid object and test configuration
    firdecim_crcf decim = firdecim_crcf_create_kaiser(M, m, 60.0f);
    CONTEND_EQUALITY(firdecim_crcf_print(decim), LIQUID_OK);
    CONTEND_EQUALITY(firdecim_crcf_set_scale(decim, 8.0f), LIQUID_OK);
    float scale = 1.0f;
    CONTEND_EQUALITY(firdecim_crcf_get_scale(decim, &scale), LIQUID_OK);
    CONTEND_EQUALITY(scale, 8.0f);

    firdecim_crcf_destroy(decim);
}

