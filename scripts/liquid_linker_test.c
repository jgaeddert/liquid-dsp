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

#include <stdio.h>
#include <stdlib.h>
#include <liquid/liquid.h>

// test linking to liquid after library has been installed

int main()
{
    // test liquid version number
    printf("checking installed liquid version numbers...\n");
    printf("  header  : %d\n", LIQUID_VERSION_NUMBER);
    printf("  library : %d\n", liquid_libversion_number());
    LIQUID_VALIDATE_LIBVERSION;

    // create object, print and return
    printf("creating test object...\n");
    resamp_crcf q = resamp_crcf_create(0.12345f, 12, 0.25f, 60.0f, 256);
    resamp_crcf_print(q);
    resamp_crcf_destroy(q);
    return 0;
}

