/*
 * Copyright (c) 2007, 2008, 2009, 2010, 2011, 2012, 2013 Joseph Gaeddert
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

#include <stdlib.h>
#include <complex.h>
#include "autotest/autotest.h"
#include "liquid.h"

// autotest helper function
//  _type       :   NCO type (e.g. LIQUID_NCO)
//  _phase      :   initial phase
//  _frequency  :   initial frequency
//  _num_samples:   number of samples to test
//  _tol        :   error tolerance
void nco_crcf_frequency_test(int          _type,
                             float        _phase,
                             float        _frequency,
                             unsigned int _num_samples,
                             float        _tol)
{
    // create object
    nco_crcf nco = nco_crcf_create(_type);

    // set phase and frequency
    nco_crcf_set_phase(nco, _phase);
    nco_crcf_set_frequency(nco, _frequency);

    // run trials
    unsigned int i;
    for (i=0; i<_num_samples; i++) {
        // compute complex output
        float complex y_test;
        nco_crcf_cexpf(nco, &y_test);

        // compute expected output
        float complex y = cexpf(_Complex_I*(_phase + i*_frequency));

        // run tests
        CONTEND_DELTA( crealf(y_test), crealf(y), _tol );
        CONTEND_DELTA( cimagf(y_test), cimagf(y), _tol );

        // step oscillator
        nco_crcf_step(nco);
    }

    // destroy object
    nco_crcf_destroy(nco);
}


// test floating point precision nco phase
void autotest_nco_crcf_frequency()
{
    // error tolerance (higher for NCO)
    float tol = 0.04f;

    // test frequencies with irrational values
    nco_crcf_frequency_test(LIQUID_NCO, 0.0f,  M_SQRT2,    256, tol);
    nco_crcf_frequency_test(LIQUID_NCO, 0.0f, -M_SQRT1_2,  256, tol);
    nco_crcf_frequency_test(LIQUID_NCO, 0.0f,  M_2_SQRTPI, 256, tol);
    nco_crcf_frequency_test(LIQUID_NCO, 0.0f,  M_E,        256, tol);
    nco_crcf_frequency_test(LIQUID_NCO, 0.0f, -M_LOG2E,    256, tol);
}

// test floating point precision vco phase
void autotest_vco_crcf_frequency()
{
    // error tolerance
    float tol = 0.0001f;

    // test frequencies with irrational values
    nco_crcf_frequency_test(LIQUID_VCO, 0.0f,  M_SQRT2,    256, tol);
    nco_crcf_frequency_test(LIQUID_VCO, 0.0f, -M_SQRT1_2,  256, tol);
    nco_crcf_frequency_test(LIQUID_VCO, 0.0f,  M_2_SQRTPI, 256, tol);
    nco_crcf_frequency_test(LIQUID_VCO, 0.0f,  M_E,        256, tol);
    nco_crcf_frequency_test(LIQUID_VCO, 0.0f, -M_LOG2E,    256, tol);
}

