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

#include <stdlib.h>

#include "liquid.autotest.h"
#include "liquid.h"

// interleave/deinterleave
void testbench_interleaver_hard(liquid_autotest __q__, unsigned int _n)
{
    unsigned int i;
    LIQUID_VLA(unsigned char, x, _n);
    LIQUID_VLA(unsigned char, y, _n);
    LIQUID_VLA(unsigned char, z, _n);

    for (i=0; i<_n; i++)
        x[i] = rand() & 0xFF;

    // create interleaver object
    interleaver q = interleaver_create(_n);

    interleaver_encode(q,x,y);
    interleaver_decode(q,y,z);

    LIQUID_CHECK_ARRAY(x, z, _n);

    // destroy interleaver object
    interleaver_destroy(q);
}

// interleave/deinterleave (soft)
void testbench_interleaver_soft(liquid_autotest __q__, unsigned int _n)
{
    unsigned int i;
    LIQUID_VLA(unsigned char, x, 8*_n);
    LIQUID_VLA(unsigned char, y, 8*_n);
    LIQUID_VLA(unsigned char, z, 8*_n);

    for (i=0; i<8*_n; i++)
        x[i] = rand() & 0xFF;

    // create interleaver object
    interleaver q = interleaver_create(_n);

    interleaver_encode_soft(q,x,y);
    interleaver_decode_soft(q,y,z);

    LIQUID_CHECK_ARRAY(x, z, 8*_n);
    
    // destroy interleaver object
    interleaver_destroy(q);
}

LIQUID_AUTOTEST(interleaver_hard_8,"","",0.1)      { testbench_interleaver_hard(__q__, 8   ); }
LIQUID_AUTOTEST(interleaver_hard_16,"","",0.1)     { testbench_interleaver_hard(__q__, 16  ); }
LIQUID_AUTOTEST(interleaver_hard_64,"","",0.1)     { testbench_interleaver_hard(__q__, 64  ); }
LIQUID_AUTOTEST(interleaver_hard_256,"","",0.1)    { testbench_interleaver_hard(__q__, 256 ); }

LIQUID_AUTOTEST(interleaver_soft_8,"","",0.1)      { testbench_interleaver_soft(__q__, 8   ); }
LIQUID_AUTOTEST(interleaver_soft_16,"","",0.1)     { testbench_interleaver_soft(__q__, 16  ); }
LIQUID_AUTOTEST(interleaver_soft_64,"","",0.1)     { testbench_interleaver_soft(__q__, 64  ); }
LIQUID_AUTOTEST(interleaver_soft_256,"","",0.1)    { testbench_interleaver_soft(__q__, 256 ); }

