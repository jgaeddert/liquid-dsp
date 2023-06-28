/*
 * Copyright (c) 2007 - 2023 Joseph Gaeddert
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

#include <stdlib.h>
#include <stdio.h>

#include "autotest/autotest.h"
#include "liquid.internal.h"

void autotest_fec_config()
{
#if LIQUID_STRICT_EXIT
    AUTOTEST_WARN("skipping fec_hamming3126 config test with strict exit enabled\n");
    return;
#endif
#if !LIQUID_SUPPRESS_ERROR_OUTPUT
    fprintf(stderr,"warning: ignore potential errors here; checking for invalid configurations\n");
#endif
    // symbols too large
    CONTEND_EQUALITY(fec_golay2412_encode_symbol(1u<<12), 0)
    CONTEND_EQUALITY(fec_golay2412_decode_symbol(1u<<24), 0)

    CONTEND_EQUALITY(fec_hamming3126_encode_symbol(1u<<26), 0)
    CONTEND_EQUALITY(fec_hamming3126_decode_symbol(1u<<31), 0)

    CONTEND_EQUALITY(fec_hamming1511_encode_symbol(1u<<11), 0)
    CONTEND_EQUALITY(fec_hamming1511_decode_symbol(1u<<15), 0)

    CONTEND_EQUALITY(fec_hamming128_encode_symbol(1u<<8), 0)
    CONTEND_EQUALITY(fec_hamming128_decode_symbol(1u<<12), 0)
}

