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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <getopt.h>

#include "liquid.internal.h"
#include "liquid.autotest.h"

LIQUID_AUTOTEST(optim_rosenbrock,"","",0.1)
{
    _liquid_error_downgrade_enable();

    // optimum
    float v_ones[8] = {1,1,1,1,1,1,1,1};
    LIQUID_CHECK_DELTA( liquid_rosenbrock(NULL, v_ones, 8), 0.0f, 1e-6f )
    LIQUID_CHECK_DELTA( liquid_rosenbrock(NULL, v_ones, 1), 0.0f, 1e-6f )

    // very far from optimum
    float v_misc[8] = {0.3, 1.0, 4.5,-2.2, 6.7,-0.2, 1.1,-0.9,};
    LIQUID_CHECK( liquid_rosenbrock(NULL, v_misc, 8)> 1000.0f )

    // invalid configuration
    LIQUID_CHECK( liquid_rosenbrock(NULL, v_misc, 0) ==  0.0f )
    _liquid_error_downgrade_disable();
}

