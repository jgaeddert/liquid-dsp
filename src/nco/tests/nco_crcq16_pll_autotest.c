/*
 * Copyright (c) 2007 - 2025 Joseph Gaeddert
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

#include <complex.h>
#include "autotest/autotest.h"

#include "liquidfpm.h"
#include "liquid.h"

#define DEBUG_NCO_CRCQ16_PLL_TEST 0

//
// test phase-locked loop
//  _type           :   NCO type (e.g. LIQUID_NCO)
//  _phase_offset   :   initial phase offset
//  _freq_offset    :   initial frequency offset
//  _pll_bandwidth  :   bandwidth of phase-locked loop
//  _num_iterations :   number of iterations to run
//  _tol            :   error tolerance
void nco_crcq16_pll_test(int          _type,
                         float        _phase_offset,
                         float        _freq_offset,
                         float        _pll_bandwidth,
                         unsigned int _num_iterations,
                         float        _tol)
{
    // objects
    nco_crcq16 nco_tx = nco_crcq16_create(_type);
    nco_crcq16 nco_rx = nco_crcq16_create(_type);

    // initialize objects
    nco_crcq16_set_phase(nco_tx,         q16_angle_float_to_fixed(_phase_offset) );
    nco_crcq16_set_frequency(nco_tx,     q16_angle_float_to_fixed(_freq_offset) );
    nco_crcq16_pll_set_bandwidth(nco_rx, _pll_bandwidth);

    // run loop
    unsigned int i;
    q16_t phase_error;
    cq16_t r, v;
#if DEBUG_NCO_CRCQ16_PLL_TEST
    const char filename[] = "nco_crcq16_pll_test.m";
    FILE * fid = fopen(filename,"w");
    fprintf(fid,"n=%u; r=zeros(1,n); v=zeros(1,n);\n", _num_iterations);
#endif
    for (i=0; i<_num_iterations; i++) {
        // received complex signal
        nco_crcq16_cexpf(nco_tx,&r);
        nco_crcq16_cexpf(nco_rx,&v);

        // phase error estimation
        phase_error = cq16_carg(cq16_mul(r, cq16_conj(v)));

        // update pll
        nco_crcq16_pll_step(nco_rx, phase_error);

        // update nco objects
        nco_crcq16_step(nco_tx);
        nco_crcq16_step(nco_rx);
#if DEBUG_NCO_CRCQ16_PLL_TEST
        fprintf(fid,"p(%3u)=%12.8f; r(%3u)=%12.8f+%12.8fj; v(%3u)=%12.8f+%12.8fj;\n",
                i+1, q16_fixed_to_float(phase_error),
                i+1, q16_fixed_to_float(r.real), q16_fixed_to_float(r.real),
                i+1, q16_fixed_to_float(v.real), q16_fixed_to_float(v.real));
#endif
    }
#if DEBUG_NCO_CRCQ16_PLL_TEST
    fprintf(fid,"t=0:(n-1); figure; plot(t,real(r),t,real(v)); xlabel('time'); ylabel('phase error'); grid on;\n");
    fclose(fid);
    printf("debug results written to %s\n", filename);
#endif

    // ensure phase of oscillators is locked
    float nco_tx_phase = q16_angle_fixed_to_float( nco_crcq16_get_phase(nco_tx) );
    float nco_rx_phase = q16_angle_fixed_to_float( nco_crcq16_get_phase(nco_rx) );
    CONTEND_DELTA(nco_tx_phase, nco_rx_phase, _tol);

    // ensure frequency of oscillators is locked
    float nco_tx_freq = q16_angle_fixed_to_float( nco_crcq16_get_frequency(nco_tx) );
    float nco_rx_freq = q16_angle_fixed_to_float( nco_crcq16_get_frequency(nco_rx) );
    CONTEND_DELTA(nco_tx_freq, nco_rx_freq, _tol);

    // clean it up
    nco_crcq16_destroy(nco_tx);
    nco_crcq16_destroy(nco_rx);
}

//
// AUTOTEST: test frequency and phase offsets
//
void autotest_vco_crcq16_pll_phase()
{
    float tol = 0.03f;

    // test various phase offsets
    nco_crcq16_pll_test(LIQUID_NCO, -M_PI/1.1f,  0.0f, 0.1f, 256, tol);
    nco_crcq16_pll_test(LIQUID_NCO, -M_PI/2.0f,  0.0f, 0.1f, 256, tol);
    nco_crcq16_pll_test(LIQUID_NCO, -M_PI/4.0f,  0.0f, 0.1f, 256, tol);
    nco_crcq16_pll_test(LIQUID_NCO, -M_PI/8.0f,  0.0f, 0.1f, 256, tol);
    nco_crcq16_pll_test(LIQUID_NCO,  M_PI/8.0f,  0.0f, 0.1f, 256, tol);
    nco_crcq16_pll_test(LIQUID_NCO,  M_PI/4.0f,  0.0f, 0.1f, 256, tol);
    nco_crcq16_pll_test(LIQUID_NCO,  M_PI/2.0f,  0.0f, 0.1f, 256, tol);
    nco_crcq16_pll_test(LIQUID_NCO,  M_PI/1.1f,  0.0f, 0.1f, 256, tol);
    
    // test various frequency offsets
    nco_crcq16_pll_test(LIQUID_NCO,  0.0f,      -1.6f, 0.1f, 256, tol);
    nco_crcq16_pll_test(LIQUID_NCO,  0.0f,      -0.8f, 0.1f, 256, tol);
    nco_crcq16_pll_test(LIQUID_NCO,  0.0f,      -0.4f, 0.1f, 256, tol);
    nco_crcq16_pll_test(LIQUID_NCO,  0.0f,      -0.2f, 0.1f, 256, tol);
    nco_crcq16_pll_test(LIQUID_NCO,  0.0f,       0.2f, 0.1f, 256, tol);
    nco_crcq16_pll_test(LIQUID_NCO,  0.0f,       0.4f, 0.1f, 256, tol);
    nco_crcq16_pll_test(LIQUID_NCO,  0.0f,       0.8f, 0.1f, 256, tol);
    nco_crcq16_pll_test(LIQUID_NCO,  0.0f,       1.6f, 0.1f, 256, tol);
}

//
// AUTOTEST: test frequency and phase offsets
//
void autotest_nco_crcq16_pll_phase()
{
    float tol = 0.03f;

    // test various phase offsets
    nco_crcq16_pll_test(LIQUID_VCO, -M_PI/1.1f,  0.0f, 0.1f, 256, tol);
    nco_crcq16_pll_test(LIQUID_VCO, -M_PI/2.0f,  0.0f, 0.1f, 256, tol);
    nco_crcq16_pll_test(LIQUID_VCO, -M_PI/4.0f,  0.0f, 0.1f, 256, tol);
    nco_crcq16_pll_test(LIQUID_VCO, -M_PI/8.0f,  0.0f, 0.1f, 256, tol);
    nco_crcq16_pll_test(LIQUID_VCO,  M_PI/8.0f,  0.0f, 0.1f, 256, tol);
    nco_crcq16_pll_test(LIQUID_VCO,  M_PI/4.0f,  0.0f, 0.1f, 256, tol);
    nco_crcq16_pll_test(LIQUID_VCO,  M_PI/2.0f,  0.0f, 0.1f, 256, tol);
    nco_crcq16_pll_test(LIQUID_VCO,  M_PI/1.1f,  0.0f, 0.1f, 256, tol);
    
    // test various frequency offsets
    nco_crcq16_pll_test(LIQUID_VCO,  0.0f,      -1.6f, 0.1f, 256, tol);
    nco_crcq16_pll_test(LIQUID_VCO,  0.0f,      -0.8f, 0.1f, 256, tol);
    nco_crcq16_pll_test(LIQUID_VCO,  0.0f,      -0.4f, 0.1f, 256, tol);
    nco_crcq16_pll_test(LIQUID_VCO,  0.0f,      -0.2f, 0.1f, 256, tol);
    nco_crcq16_pll_test(LIQUID_VCO,  0.0f,       0.2f, 0.1f, 256, tol);
    nco_crcq16_pll_test(LIQUID_VCO,  0.0f,       0.4f, 0.1f, 256, tol);
    nco_crcq16_pll_test(LIQUID_VCO,  0.0f,       0.8f, 0.1f, 256, tol);
    nco_crcq16_pll_test(LIQUID_VCO,  0.0f,       1.6f, 0.1f, 256, tol);
}

