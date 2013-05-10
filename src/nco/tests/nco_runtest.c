/*
 * Copyright (c) 2013 Joseph Gaeddert
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

#include "autotest/autotest.h"
#include "liquid.h"

// autotest helper function
//  _theta  :   input phase
//  _cos    :   expected output: cos(_theta)
//  _sin    :   expected output: sin(_theta)
//  _type   :   NCO type (e.g. LIQUID_NCO)
//  _tol    :   error tolerance
void nco_crcf_phase_test(float _theta,
                         float _cos,
                         float _sin,
                         int   _type,
                         float _tol)
{
    // create object
    nco_crcf nco = nco_crcf_create(_type);

    // set phase
    nco_crcf_set_phase(nco, _theta);

    // compute cosine and sine outputs
    float c = nco_crcf_cos(nco);
    float s = nco_crcf_sin(nco);

    // run tests
    CONTEND_DELTA( c, _cos, _tol );
    CONTEND_DELTA( s, _sin, _tol );

    // destroy object
    nco_crcf_destroy(nco);
}

