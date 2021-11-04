/*
 * Copyright (c) 2007 - 2021 Joseph Gaeddert
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
#include "autotest/autotest.h"
#include "liquid.h"

// autotest helper functions
void testbench_symtrack_cccf(unsigned int _k, unsigned int _m, float _beta, int _ms)
{
    int          ftype       = LIQUID_FIRFILT_ARKAISER;
    unsigned int num_symbols = 4000;    // number of data symbols
    //unsigned int hc_len      =   4;     // channel filter length
    float        noise_floor = -30.0f;  // noise floor [dB]
    float        SNRdB       = 30.0f;   // signal-to-noise ratio [dB]
    float        bandwidth   =  0.10f;  // loop filter bandwidth
    float        dphi        =  0.02f;  // carrier frequency offset [radians/sample]
    float        phi         =  2.1f;   // carrier phase offset [radians]

    // buffers
    unsigned int    buf_len = 800;  // buffer size
    float complex   buf_0[buf_len]; // sample buffer
    float complex   buf_1[buf_len]; // recovered symbols buffer

    // create stream generator
    symstreamcf gen = symstreamcf_create_linear(ftype,_k,_m,_beta,_ms);

    // create channel emulator and add impairments
    channel_cccf channel = channel_cccf_create();
    channel_cccf_add_awgn          (channel, noise_floor, SNRdB);
    channel_cccf_add_carrier_offset(channel, dphi, phi);
    //channel_cccf_add_multipath     (channel, NULL, hc_len);

    // create symbol tracking synchronizer
    symtrack_cccf symtrack = symtrack_cccf_create(ftype,_k,_m,_beta,_ms);
    symtrack_cccf_set_bandwidth(symtrack,bandwidth);
    //symtrack_cccf_set_eq_off(symtrack); // disable equalization
    if (liquid_autotest_verbose)
        symtrack_cccf_print(symtrack);

    unsigned int total_samples = 0;
    unsigned int total_symbols = 0;
    unsigned int num_symbols_evm = 0;
    float        evm = 0.0f;
    modemcf demod = modemcf_create(_ms); // for checking output EVM
    //FILE * fid = fopen("symtrack_test.dat","w");
    while (total_samples < num_symbols * _k)
    {
        // write samples to buffer
        symstreamcf_write_samples(gen, buf_0, buf_len);

        // apply channel
        channel_cccf_execute_block(channel, buf_0, buf_len, buf_0);

        // run resulting stream through synchronizer
        unsigned int num_symbols_sync;
        symtrack_cccf_execute_block(symtrack, buf_0, buf_len, buf_1, &num_symbols_sync);
        total_symbols += num_symbols_sync;
        total_samples += buf_len;

        // check output EVM
        if (total_samples > 0.80 * num_symbols * _k) {
            unsigned int i, sym;
            for (i=0; i<num_symbols_sync; i++) {
                modemcf_demodulate(demod, buf_1[i], &sym);
                float err = modemcf_get_demodulator_evm(demod);
                evm += err*err;
                //fprintf(fid,"%12.8f %12.8f\n", crealf(buf_1[i]), cimagf(buf_1[i]));
            }
            num_symbols_evm += num_symbols_sync;
        }
    }
    //fclose(fid);

    // destroy objects
    symstreamcf_destroy  (gen);
    channel_cccf_destroy (channel);
    symtrack_cccf_destroy(symtrack);
    modemcf_destroy(demod);

    // verify output constellation EVM is reasonably high
    evm = 10*log10f(evm / (float)num_symbols_evm);
    printf("EVM: %12.8f, %u\n", evm, num_symbols_evm);
    CONTEND_LESS_THAN(evm, -20.0f);
}

void autotest_symtrack_cccf_00() { testbench_symtrack_cccf( 2, 7,0.20f,LIQUID_MODEM_BPSK); }
void autotest_symtrack_cccf_01() { testbench_symtrack_cccf( 2, 7,0.20f,LIQUID_MODEM_QPSK); }

