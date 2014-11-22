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
#include "liquid.h"

//
// test phase-locked loop
//  _type           :   NCO type (e.g. LIQUID_NCO)
//  _phase_offset   :   initial phase offset
//  _freq_offset    :   initial frequency offset
//  _pll_bandwidth  :   bandwidth of phase-locked loop
//  _num_iterations :   number of iterations to run
//  _tol            :   error tolerance
void nco_crcf_pll_test(int          _type,
                       float        _phase_offset,
                       float        _freq_offset,
                       float        _pll_bandwidth,
                       unsigned int _num_iterations,
                       float        _tol)
{
    // objects
    nco_crcf nco_tx = nco_crcf_create(_type);
    nco_crcf nco_rx = nco_crcf_create(_type);

    // initialize objects
    nco_crcf_set_phase(nco_tx, _phase_offset);
    nco_crcf_set_frequency(nco_tx, _freq_offset);
    nco_crcf_pll_set_bandwidth(nco_rx, _pll_bandwidth);

    // run loop
    unsigned int i;
    float phase_error;
    float complex r, v;
    for (i=0; i<_num_iterations; i++) {
        // received complex signal
        nco_crcf_cexpf(nco_tx,&r);
        nco_crcf_cexpf(nco_rx,&v);

        // error estimation
        phase_error = cargf(r*conjf(v));

        // update pll
        nco_crcf_pll_step(nco_rx, phase_error);

        // update nco objects
        nco_crcf_step(nco_tx);
        nco_crcf_step(nco_rx);
    }

    // ensure phase of oscillators is locked
    float nco_tx_phase = nco_crcf_get_phase(nco_tx);
    float nco_rx_phase = nco_crcf_get_phase(nco_rx);
    CONTEND_DELTA(nco_tx_phase, nco_rx_phase, _tol);

    // ensure frequency of oscillators is locked
    float nco_tx_freq = nco_crcf_get_frequency(nco_tx);
    float nco_rx_freq = nco_crcf_get_frequency(nco_rx);
    CONTEND_DELTA(nco_tx_freq, nco_rx_freq, _tol);

    // clean it up
    nco_crcf_destroy(nco_tx);
    nco_crcf_destroy(nco_rx);
}

//
// AUTOTEST: test frequency and phase offsets
//
void autotest_vco_crcf_pll_phase()
{
    float tol = 0.02f;

    // test various phase offsets
    nco_crcf_pll_test(LIQUID_NCO, -M_PI/1.1f,  0.0f, 0.1f, 256, tol);
    nco_crcf_pll_test(LIQUID_NCO, -M_PI/2.0f,  0.0f, 0.1f, 256, tol);
    nco_crcf_pll_test(LIQUID_NCO, -M_PI/4.0f,  0.0f, 0.1f, 256, tol);
    nco_crcf_pll_test(LIQUID_NCO, -M_PI/8.0f,  0.0f, 0.1f, 256, tol);
    nco_crcf_pll_test(LIQUID_NCO,  M_PI/8.0f,  0.0f, 0.1f, 256, tol);
    nco_crcf_pll_test(LIQUID_NCO,  M_PI/4.0f,  0.0f, 0.1f, 256, tol);
    nco_crcf_pll_test(LIQUID_NCO,  M_PI/2.0f,  0.0f, 0.1f, 256, tol);
    nco_crcf_pll_test(LIQUID_NCO,  M_PI/1.1f,  0.0f, 0.1f, 256, tol);
    
    // test various frequency offsets
    nco_crcf_pll_test(LIQUID_NCO,  0.0f,      -1.6f, 0.1f, 256, tol);
    nco_crcf_pll_test(LIQUID_NCO,  0.0f,      -0.8f, 0.1f, 256, tol);
    nco_crcf_pll_test(LIQUID_NCO,  0.0f,      -0.4f, 0.1f, 256, tol);
    nco_crcf_pll_test(LIQUID_NCO,  0.0f,      -0.2f, 0.1f, 256, tol);
    nco_crcf_pll_test(LIQUID_NCO,  0.0f,       0.2f, 0.1f, 256, tol);
    nco_crcf_pll_test(LIQUID_NCO,  0.0f,       0.4f, 0.1f, 256, tol);
    nco_crcf_pll_test(LIQUID_NCO,  0.0f,       0.8f, 0.1f, 256, tol);
    nco_crcf_pll_test(LIQUID_NCO,  0.0f,       1.6f, 0.1f, 256, tol);
}

//
// AUTOTEST: test frequency and phase offsets
//
void autotest_nco_crcf_pll_phase()
{
    float tol = 1e-3f;


    // test various phase offsets
    nco_crcf_pll_test(LIQUID_VCO, -M_PI/1.1f,  0.0f, 0.1f, 256, tol);
    nco_crcf_pll_test(LIQUID_VCO, -M_PI/2.0f,  0.0f, 0.1f, 256, tol);
    nco_crcf_pll_test(LIQUID_VCO, -M_PI/4.0f,  0.0f, 0.1f, 256, tol);
    nco_crcf_pll_test(LIQUID_VCO, -M_PI/8.0f,  0.0f, 0.1f, 256, tol);
    nco_crcf_pll_test(LIQUID_VCO,  M_PI/8.0f,  0.0f, 0.1f, 256, tol);
    nco_crcf_pll_test(LIQUID_VCO,  M_PI/4.0f,  0.0f, 0.1f, 256, tol);
    nco_crcf_pll_test(LIQUID_VCO,  M_PI/2.0f,  0.0f, 0.1f, 256, tol);
    nco_crcf_pll_test(LIQUID_VCO,  M_PI/1.1f,  0.0f, 0.1f, 256, tol);
    
    // test various frequency offsets
    nco_crcf_pll_test(LIQUID_VCO,  0.0f,      -1.6f, 0.1f, 256, tol);
    nco_crcf_pll_test(LIQUID_VCO,  0.0f,      -0.8f, 0.1f, 256, tol);
    nco_crcf_pll_test(LIQUID_VCO,  0.0f,      -0.4f, 0.1f, 256, tol);
    nco_crcf_pll_test(LIQUID_VCO,  0.0f,      -0.2f, 0.1f, 256, tol);
    nco_crcf_pll_test(LIQUID_VCO,  0.0f,       0.2f, 0.1f, 256, tol);
    nco_crcf_pll_test(LIQUID_VCO,  0.0f,       0.4f, 0.1f, 256, tol);
    nco_crcf_pll_test(LIQUID_VCO,  0.0f,       0.8f, 0.1f, 256, tol);
    nco_crcf_pll_test(LIQUID_VCO,  0.0f,       1.6f, 0.1f, 256, tol);
}

