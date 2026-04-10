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
#include <string.h>
#include <math.h>

#include "liquid.autotest.h"
#include "liquid.internal.h"

// compute basic entropy metric
float liquid_scramble_test_entropy(unsigned char * _x,
                                   unsigned int _n)
{
    unsigned int i;

    // count ones
    unsigned int num_ones=0;
    for (i=0; i<_n; i++)
        num_ones += liquid_c_ones[_x[i]];

    // compute probabilities (add small value to prevent
    // possible log(0))
    float p1 = (float)num_ones / (float)(8*_n) + 1e-12f;
    float p0 = 1.0 - p1;
    return -p0*log2f(p0) - p1*log2f(p1);
}

// helper function to keep code base small
void testbench_scramble(liquid_autotest __q__, unsigned int _n)
{
    LIQUID_VLA(unsigned char, x, _n);    // input data
    LIQUID_VLA(unsigned char, y, _n);    // scrambled data
    LIQUID_VLA(unsigned char, z, _n);    // unscrambled data

    unsigned int i;

    // initialize data array
    for (i=0; i<_n; i++)
        x[i] = 0x00;

    // scramble input
    memmove(y,x,_n);
    scramble_data(y,_n);

    // unscramble result
    memmove(z,y,_n);
    unscramble_data(z,_n);

    // ensure data are equivalent
    LIQUID_CHECK_ARRAY(x,z,_n*sizeof(unsigned char));

    // compute entropy metric
    float H = liquid_scramble_test_entropy(y,_n);
    LIQUID_CHECK( H > 0.8f );
}


// test unscrambling of soft bits (helper function to keep code base small)
void testbench_scramble_soft(liquid_autotest __q__, unsigned int _n)
{
    LIQUID_VLA(unsigned char, msg_org, _n);      // input data
    LIQUID_VLA(unsigned char, msg_enc, _n);      // scrambled data 
    LIQUID_VLA(unsigned char, msg_soft, 8*_n);   // scrambled data (soft bits)
    LIQUID_VLA(unsigned char, msg_dec, _n);      // unscrambled data

    unsigned int i;

    // initialize data array (random)
    for (i=0; i<_n; i++)
        msg_org[i] = rand() & 0xff;

    // scramble input
    memmove(msg_enc, msg_org, _n);
    scramble_data(msg_enc,_n);

    // convert to soft bits
    for (i=0; i<_n; i++)
        liquid_unpack_soft_bits(msg_enc[i], 8, &msg_soft[8*i]);

    // unscramble result
    unscramble_data_soft(msg_soft, _n);

    // unpack soft bits
    for (i=0; i<_n; i++) {
        unsigned int sym_out;
        liquid_pack_soft_bits(&msg_soft[8*i], 8, &sym_out);
        msg_dec[i] = sym_out;
    }

    // ensure data are equivalent
    LIQUID_CHECK_ARRAY(msg_org, msg_dec, _n);
}

// simple data scrambling
LIQUID_AUTOTEST(scramble_n16,"","",0.1)     { testbench_scramble(__q__,16);  };
LIQUID_AUTOTEST(scramble_n64,"","",0.1)     { testbench_scramble(__q__,64);  };
LIQUID_AUTOTEST(scramble_n256,"","",0.1)    { testbench_scramble(__q__,256); };

LIQUID_AUTOTEST(scramble_n11,"","",0.1)     { testbench_scramble(__q__,11);  };
LIQUID_AUTOTEST(scramble_n33,"","",0.1)     { testbench_scramble(__q__,33);  };
LIQUID_AUTOTEST(scramble_n277,"","",0.1)    { testbench_scramble(__q__,277); };

// soft data scrambling
LIQUID_AUTOTEST(scramble_soft_n16,"","",0.1)     { testbench_scramble_soft(__q__,16);  };
LIQUID_AUTOTEST(scramble_soft_n64,"","",0.1)     { testbench_scramble_soft(__q__,64);  };
LIQUID_AUTOTEST(scramble_soft_n256,"","",0.1)    { testbench_scramble_soft(__q__,256); };

LIQUID_AUTOTEST(scramble_soft_n11,"","",0.1)     { testbench_scramble_soft(__q__,11);  };
LIQUID_AUTOTEST(scramble_soft_n33,"","",0.1)     { testbench_scramble_soft(__q__,33);  };
LIQUID_AUTOTEST(scramble_soft_n277,"","",0.1)    { testbench_scramble_soft(__q__,277); };


