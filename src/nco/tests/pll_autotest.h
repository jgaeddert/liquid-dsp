/*
 * Copyright (c) 2007, 2009 Joseph Gaeddert
 * Copyright (c) 2007, 2009 Virginia Polytechnic Institute & State University
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

#ifndef __LIQUID_NCO_AUTOTEST_H__
#define __LIQUID_NCO_AUTOTEST_H__

#include <complex.h>
#include "autotest/autotest.h"
#include "liquid.h"

//
// Helper function: run loop
//
void pll_run_loop(nco _nco_tx, nco _nco_rx, pll _pll_rx, unsigned int _n)
{
    // run loop
    unsigned int i;
    float phase_error;
    float complex r, v;
    for (i=0; i<_n; i++) {
        // received complex signal
        nco_cexpf(_nco_tx,&r);
        nco_cexpf(_nco_rx,&v);

        // error estimation
        phase_error = cargf(r*conjf(v));

        // update NCO objects
        nco_step(_nco_tx);
        pll_step(_pll_rx, _nco_rx, phase_error);
        nco_step(_nco_rx);
    }
}

//
// test simple nco lock, phase offset
//
void autotest_pll_phase_offset() {
    // parameters
    float phase_offset = M_PI/2;
    float frequency_offset = 0.0f;
    float pll_bandwidth = 1e-2f;
    unsigned int n=256;     // number of iterations
    float tol = 1e-4f;      // error tolerance

    // objects
    nco nco_tx = nco_create(LIQUID_VCO);
    nco nco_rx = nco_create(LIQUID_VCO);
    pll pll_rx = pll_create();

    // initialize objects
    nco_set_phase(nco_tx, phase_offset);
    nco_set_frequency(nco_tx, frequency_offset);
    pll_set_bandwidth(pll_rx, pll_bandwidth);

    // run loop
    pll_run_loop(nco_tx, nco_rx, pll_rx, n);

    // run tests
    float nco_tx_phase = nco_get_phase(nco_tx);
    float nco_rx_phase = nco_get_phase(nco_rx);
    CONTEND_DELTA(nco_tx_phase, nco_rx_phase, tol);

    float nco_tx_freq = nco_get_frequency(nco_tx);
    float nco_rx_freq = nco_get_frequency(nco_rx);
    CONTEND_DELTA(nco_tx_freq, nco_rx_freq, tol);

    // clean it up
    nco_destroy(nco_tx);
    nco_destroy(nco_rx);
    pll_destroy(pll_rx);
}

//
// test simple nco lock, frequency offset
//
void autotest_pll_frequency_offset() {
    // parameters
    float phase_offset = 0.0f;
    float frequency_offset = 0.5f;
    float pll_bandwidth = 1e-2f;
    unsigned int n=256;     // number of iterations
    float tol = 1e-4f;      // error tolerance

    // objects
    nco nco_tx = nco_create(LIQUID_VCO);
    nco nco_rx = nco_create(LIQUID_VCO);
    pll pll_rx = pll_create();

    // initialize objects
    nco_set_phase(nco_tx, phase_offset);
    nco_set_frequency(nco_tx, frequency_offset);
    pll_set_bandwidth(pll_rx, pll_bandwidth);

    // run loop
    pll_run_loop(nco_tx, nco_rx, pll_rx, n);

    // run tests
    float nco_tx_phase = nco_get_phase(nco_tx);
    float nco_rx_phase = nco_get_phase(nco_rx);
    CONTEND_DELTA(nco_tx_phase, nco_rx_phase, tol);

    float nco_tx_freq = nco_get_frequency(nco_tx);
    float nco_rx_freq = nco_get_frequency(nco_rx);
    CONTEND_DELTA(nco_tx_freq, nco_rx_freq, tol);

    // clean it up
    nco_destroy(nco_tx);
    nco_destroy(nco_rx);
    pll_destroy(pll_rx);
}

#endif // __LIQUID_NCO_AUTOTEST_H__

