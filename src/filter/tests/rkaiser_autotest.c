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

LIQUID_AUTOTEST(liquid_rkaiser_config,"description","",0.1)
{
    _liquid_error_downgrade_enable();

    LIQUID_CHECK(liquid_firdes_rkaiser(0, 12, 0.2f, 0, NULL) ==  LIQUID_EICONFIG); // k too small
    LIQUID_CHECK(liquid_firdes_rkaiser(2,  0, 0.2f, 0, NULL) ==  LIQUID_EICONFIG); // m too small
    LIQUID_CHECK(liquid_firdes_rkaiser(2, 12,-0.7f, 0, NULL) ==  LIQUID_EICONFIG); // beta too small
    LIQUID_CHECK(liquid_firdes_rkaiser(2, 12, 2.7f, 0, NULL) ==  LIQUID_EICONFIG); // beta too large
    LIQUID_CHECK(liquid_firdes_rkaiser(2, 12, 0.2f,-2, NULL) ==  LIQUID_EICONFIG); // dt too small
    LIQUID_CHECK(liquid_firdes_rkaiser(2, 12, 0.2f, 3, NULL) ==  LIQUID_EICONFIG); // dt too large

    LIQUID_CHECK(liquid_firdes_arkaiser(0, 12, 0.2f, 0, NULL) ==  LIQUID_EICONFIG); // k too small
    LIQUID_CHECK(liquid_firdes_arkaiser(2,  0, 0.2f, 0, NULL) ==  LIQUID_EICONFIG); // m too small
    LIQUID_CHECK(liquid_firdes_arkaiser(2, 12,-0.7f, 0, NULL) ==  LIQUID_EICONFIG); // beta too small
    LIQUID_CHECK(liquid_firdes_arkaiser(2, 12, 2.7f, 0, NULL) ==  LIQUID_EICONFIG); // beta too large
    LIQUID_CHECK(liquid_firdes_arkaiser(2, 12, 0.2f,-2, NULL) ==  LIQUID_EICONFIG); // dt too small
    LIQUID_CHECK(liquid_firdes_arkaiser(2, 12, 0.2f, 3, NULL) ==  LIQUID_EICONFIG); // dt too large

    _liquid_error_downgrade_disable();
}

