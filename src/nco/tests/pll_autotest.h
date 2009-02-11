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
        r = nco_cexpf(_nco_tx);
        v = nco_cexpf(_nco_rx);

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
    nco nco_tx = nco_create();
    nco nco_rx = nco_create();
    pll pll_rx = pll_create();

    // initialize objects
    nco_set_phase(nco_tx, phase_offset);
    nco_set_frequency(nco_tx, frequency_offset);
    pll_set_bandwidth(pll_rx, pll_bandwidth);

    // run loop
    pll_run_loop(nco_tx, nco_rx, pll_rx, n);

    // run tests
    CONTEND_DELTA(nco_tx->theta, nco_rx->theta, tol);
    CONTEND_DELTA(nco_tx->d_theta, nco_rx->d_theta, tol);

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
    nco nco_tx = nco_create();
    nco nco_rx = nco_create();
    pll pll_rx = pll_create();

    // initialize objects
    nco_set_phase(nco_tx, phase_offset);
    nco_set_frequency(nco_tx, frequency_offset);
    pll_set_bandwidth(pll_rx, pll_bandwidth);

    // run loop
    pll_run_loop(nco_tx, nco_rx, pll_rx, n);

    // run tests
    CONTEND_DELTA(nco_tx->theta, nco_rx->theta, tol);
    CONTEND_DELTA(nco_tx->d_theta, nco_rx->d_theta, tol);

    // clean it up
    nco_destroy(nco_tx);
    nco_destroy(nco_rx);
    pll_destroy(pll_rx);
}

#endif // __LIQUID_NCO_AUTOTEST_H__

