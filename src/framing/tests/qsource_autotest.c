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

LIQUID_AUTOTEST(qsourcecf_config,"qsource configurations","",0.1)
{
    _liquid_error_downgrade_enable();
    LIQUID_CHECK(NULL == qsourcecf_create( 0, 12, 60, 0.0f, 0.2f, 10.0f)); // too few subcarriers
    LIQUID_CHECK(NULL == qsourcecf_create(17, 12, 60, 0.0f, 0.2f, 10.0f)); // odd-numbered subcarriers
    LIQUID_CHECK(NULL == qsourcecf_create(64,  0, 60, 0.0f, 0.2f, 10.0f)); // filter semi-length too small
    LIQUID_CHECK(NULL == qsourcecf_create(64, 12, 60,+0.6f, 0.2f, 10.0f)); // center frequency out of range
    LIQUID_CHECK(NULL == qsourcecf_create(64, 12, 60,-0.6f, 0.2f, 10.0f)); // center frequency out of range
    LIQUID_CHECK(NULL == qsourcecf_create(64, 12, 60, 0.0f,-0.1f, 10.0f)); // bandwidth out of range
    LIQUID_CHECK(NULL == qsourcecf_create(64, 12, 60, 0.0f, 1.1f, 10.0f)); // bandwidth out of range
    LIQUID_CHECK(NULL == qsourcecf_copy(NULL));
    _liquid_error_downgrade_disable();
}

