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

void autotest_firfilt_crcf_kaiser()
{
    // design filter
    firfilt_crcf q = firfilt_crcf_create_kaiser(51, 0.2f, 60.0f, 0.0f);
    firfilt_crcf_set_scale(q, 0.4f);

    // verify resulting spectrum
    autotest_psd_s regions[] = {
      {.fmin=-0.5,   .fmax=-0.25,  .pmin= 0,   .pmax=-60,  .test_lo=0, .test_hi=1},
      {.fmin=-0.15,  .fmax=+0.15,  .pmin=-0.1, .pmax=+0.1, .test_lo=1, .test_hi=1},
      {.fmin= 0.25,  .fmax=+0.5,   .pmin= 0,   .pmax=-60,  .test_lo=0, .test_hi=1},
    };
    liquid_autotest_validate_psd_firfilt_crcf(q, 1200, regions, 3,
        liquid_autotest_verbose ? "autotest_firfilt_crcf_kaiser.m" : NULL);
    firfilt_crcf_destroy(q);
}

void autotest_firfilt_crcf_firdespm()
{
    // design filter
    firfilt_crcf q = firfilt_crcf_create_firdespm(51, 0.2f, 60.0f);
    firfilt_crcf_set_scale(q, 0.4f);

    // verify resulting spectrum
    autotest_psd_s regions[] = {
      {.fmin=-0.5,   .fmax=-0.25,  .pmin= 0,   .pmax=-60,  .test_lo=0, .test_hi=1},
      {.fmin=-0.15,  .fmax=+0.15,  .pmin=-0.1, .pmax=+0.1, .test_lo=1, .test_hi=1},
      {.fmin= 0.25,  .fmax=+0.5,   .pmin= 0,   .pmax=-60,  .test_lo=0, .test_hi=1},
    };
    liquid_autotest_validate_psd_firfilt_crcf(q, 1200, regions, 3,
        liquid_autotest_verbose ? "autotest_firfilt_crcf_firdespm.m" : NULL);
    firfilt_crcf_destroy(q);
}

void autotest_firfilt_config()
{
#if LIQUID_STRICT_EXIT
    AUTOTEST_WARN("skipping firfilt config test with strict exit enabled\n");
    return;
#endif
#if !LIQUID_SUPPRESS_ERROR_OUTPUT
    fprintf(stderr,"warning: ignore potential errors here; checking for invalid configurations\n");
#endif
    // no need to check every combination
    CONTEND_ISNULL(firfilt_crcf_create_kaiser(0, 0, 0, 0));
    CONTEND_ISNULL(firfilt_crcf_create_rnyquist(LIQUID_FIRFILT_UNKNOWN, 0, 0, 0, 4));
    CONTEND_ISNULL(firfilt_crcf_create_firdespm(0, 0, 0));
    CONTEND_ISNULL(firfilt_crcf_create_dc_blocker(0, 0));
    CONTEND_ISNULL(firfilt_crcf_create_notch(0, 0, 0));
    CONTEND_ISNULL(firfilt_cccf_create_notch(0, 0, 0));

    // create proper object and test configurations
    firfilt_crcf q = firfilt_crcf_create_kaiser(11, 0.2f, 60.0f, 0.0f);

    CONTEND_EQUALITY(LIQUID_OK, firfilt_crcf_print(q))

    float scale = 7.0f;
    CONTEND_EQUALITY(LIQUID_OK, firfilt_crcf_set_scale(q, 3.0f))
    CONTEND_EQUALITY(LIQUID_OK, firfilt_crcf_get_scale(q, &scale))
    CONTEND_EQUALITY(scale, 3.0f)
    CONTEND_EQUALITY(firfilt_crcf_get_length(q), 11)

    firfilt_crcf_destroy(q);
}

