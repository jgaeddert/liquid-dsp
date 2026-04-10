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
#include "liquid.h"
#include "liquid_vla.h"

// AUTOTEST : test partitioning rational-rate resampler
void testbench_rresamp_crcf_part(liquid_autotest __q__,
                                    unsigned int _P, // output size
                                    unsigned int _Q, // input size
                                    unsigned int _m, // filter semi-length
                                    unsigned int _n) // number of blocks
{
    // semi-fixed options
    float tol = 1e-12f; // error tolerance (should be basically zero)
    float bw  = 0.5f;   // resampling filter bandwidth
    float As  = 60.0f;  // resampling filter stop-band attenuation [dB]

    // create two identical resampler objects
    rresamp_crcf q0 = rresamp_crcf_create_kaiser(_P,_Q,_m,bw,As);
    rresamp_crcf q1 = rresamp_crcf_create_kaiser(_P,_Q,_m,bw,As);

    // full input, output buffers
    LIQUID_VLA(liquid_float_complex, buf_in, 2*_Q*_n); // input buffer
    LIQUID_VLA(liquid_float_complex, buf_out_0, 2*_P*_n); // output, normal resampling operation
    LIQUID_VLA(liquid_float_complex, buf_out_1, 2*_P*_n); // output, partitioned into 2 blocks

    // generate input signal (pulse, but can really be anything)
    unsigned int i;
    for (i=0; i<2*_Q*_n; i++)
        buf_in[i] = liquid_hamming(i,2*_Q*_n) * cexpf(_Complex_I*2*M_PI*0.037f*i);

    // run resampler normally in one large block (2*_Q*n inputs, 2*_P*n outputs)
    rresamp_crcf_execute_block(q0, buf_in, 2*_n, buf_out_0);

    // reset and run with separate resamplers (e.g. in two threads)
    rresamp_crcf_reset(q0);
    // first block runs as normal
    rresamp_crcf_execute_block(q0, buf_in, _n, buf_out_1);
    // initialize second block with _Q*m samples to account for delay
    for (i=0; i<_m; i++)
        rresamp_crcf_write(q1, buf_in + _Q*_n - (_m-i)*_Q);
    // run remainder of second block as normal
    rresamp_crcf_execute_block(q1, buf_in + _Q*_n, _n, buf_out_1 + _P*_n);

    // clean up allocated objects
    rresamp_crcf_destroy(q0);
    rresamp_crcf_destroy(q1);

    // compare output buffers between normal and partitioned operation
    for (i=0; i<2*_P*_n; i++) {
        LIQUID_CHECK_DELTA( crealf(buf_out_0[i]), crealf(buf_out_1[i]), tol );
        LIQUID_CHECK_DELTA( cimagf(buf_out_0[i]), cimagf(buf_out_1[i]), tol );
    }
}

LIQUID_AUTOTEST(rresamp_crcf_part_P1_Q5,"description","",0.1) { testbench_rresamp_crcf_part(__q__, 1, 5, 15, 20); }
LIQUID_AUTOTEST(rresamp_crcf_part_P2_Q5,"description","",0.1) { testbench_rresamp_crcf_part(__q__, 2, 5, 15, 20); }
LIQUID_AUTOTEST(rresamp_crcf_part_P3_Q5,"description","",0.1) { testbench_rresamp_crcf_part(__q__, 3, 5, 15, 20); }
LIQUID_AUTOTEST(rresamp_crcf_part_P6_Q5,"description","",0.1) { testbench_rresamp_crcf_part(__q__, 6, 5, 15, 20); }
LIQUID_AUTOTEST(rresamp_crcf_part_P8_Q5,"description","",0.1) { testbench_rresamp_crcf_part(__q__, 8, 5, 15, 20); }
LIQUID_AUTOTEST(rresamp_crcf_part_P9_Q5,"description","",0.1) { testbench_rresamp_crcf_part(__q__, 9, 5, 15, 20); }

