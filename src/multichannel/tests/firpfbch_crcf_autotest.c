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

#include <assert.h>
#include "liquid.autotest.h"
#include "liquid.internal.h"

LIQUID_AUTOTEST(firpfbch_crcf_config,"firpfbch config","",0.1)
{
    _liquid_error_downgrade_enable();
    // check invalid function calls
    LIQUID_CHECK(NULL ==firpfbch_crcf_create(             77, 76, 12, NULL)) // invalid type
    LIQUID_CHECK(NULL ==firpfbch_crcf_create(LIQUID_ANALYZER,  0, 12, NULL)) // invalid number of channels
    LIQUID_CHECK(NULL ==firpfbch_crcf_create(LIQUID_ANALYZER, 76,  0, NULL)) // invalid filter semi-length

    LIQUID_CHECK(NULL ==firpfbch_crcf_create_kaiser(             77, 76, 12, 60.0f)) // invalid type
    LIQUID_CHECK(NULL ==firpfbch_crcf_create_kaiser(LIQUID_ANALYZER,  0, 12, 60.0f)) // invalid number of channels
    LIQUID_CHECK(NULL ==firpfbch_crcf_create_kaiser(LIQUID_ANALYZER, 76,  0, 60.0f)) // invalid filter semi-length

    LIQUID_CHECK(NULL ==firpfbch_crcf_create_rnyquist(             77, 76, 12, 0.2f, LIQUID_FIRFILT_ARKAISER)) // invalid type
    LIQUID_CHECK(NULL ==firpfbch_crcf_create_rnyquist(LIQUID_ANALYZER,  0, 12, 0.2f, LIQUID_FIRFILT_ARKAISER)) // invalid number of channels
    LIQUID_CHECK(NULL ==firpfbch_crcf_create_rnyquist(LIQUID_ANALYZER, 76,  0, 0.2f, LIQUID_FIRFILT_ARKAISER)) // invalid filter semi-length
    LIQUID_CHECK(NULL ==firpfbch_crcf_create_rnyquist(LIQUID_ANALYZER, 76, 12, 77.f, LIQUID_FIRFILT_ARKAISER)) // invalid filter excess bandwidth
    LIQUID_CHECK(NULL ==firpfbch_crcf_create_rnyquist(LIQUID_ANALYZER, 76, 12, 0.2f, LIQUID_FIRFILT_UNKNOWN )) // invalid filter type

    //LIQUID_CHECK(NULL ==firpfbch_crcf_copy(NULL))

    // create proper object and test configurations
    firpfbch_crcf q = firpfbch_crcf_create_kaiser(LIQUID_ANALYZER, 76, 12, 60.0f);

    LIQUID_CHECK(LIQUID_OK == firpfbch_crcf_print(q))

    firpfbch_crcf_destroy(q);
    _liquid_error_downgrade_disable();
}

