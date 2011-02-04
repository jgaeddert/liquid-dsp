/*
 * Copyright (c) 2007, 2008, 2009, 2010, 2011 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010, 2011 Virginia Polytechnic
 *                                      Institute & State University
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
// fft.common.c
//

#include "liquid.internal.h"

// perform n-point FFT allocating plan internally
//  _n      :   fft size
//  _x      :   input array [size: _n x 1]
//  _y      :   output array [size: _n x 1]
//  _dir    :   fft direction: {FFT_FORWARD, FFT_REVERSE}
//  _method :   fft method
void FFT(_run)(unsigned int _n,
               TC * _x,
               TC * _y,
               int _dir,
               int _method)
{
    // create plan
    FFT(plan) plan = FFT(_create_plan)(_n, _x, _y, _dir, _method);

    // execute fft
    FFT(_execute)(plan);

    // destroy plan
    FFT(_destroy_plan)(plan);
}

// perform _n-point fft shift
void FFT(_shift)(TC *_x, unsigned int _n)
{
    unsigned int i, n2;
    if (_n%2)
        n2 = (_n-1)/2;
    else
        n2 = _n/2;

    TC tmp;
    for (i=0; i<n2; i++) {
        tmp = _x[i];
        _x[i] = _x[i+n2];
        _x[i+n2] = tmp;
    }
}

