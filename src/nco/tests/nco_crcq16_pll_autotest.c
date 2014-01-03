/*
 * Copyright (c) 2007 - 2014 Joseph Gaeddert
 *
 * This file is part of liquid.
 *
 * liquid is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * liquid is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with liquid.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <complex.h>
#include "autotest/autotest.h"

#include "liquidfpm.h"
#include "liquid.h"

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
    nco_crcq16_pll_set_bandwidth(nco_rx, q16_float_to_fixed(_pll_bandwidth) );

    // run loop
    unsigned int i;
    q16_t phase_error;
    cq16_t r, v;
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
    }

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
    float tol = 0.02f;

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
    float tol = 0.02f;

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

