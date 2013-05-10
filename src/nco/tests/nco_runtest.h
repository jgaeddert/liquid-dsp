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

//
// autotest firfilt data definitions
//

#ifndef __LIQUID_NCO_H__
#define __LIQUID_NCO_H__

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
                         float _tol);

#endif // __LIQUID_NCO_H__

