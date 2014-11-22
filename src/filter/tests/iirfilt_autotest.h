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

//
// autotest iirfilt data definitions
//

#ifndef __LIQUID_IIRFILT_H__
#define __LIQUID_IIRFILT_H__

// autotest helper functions:
//  _b      :   filter coefficients (numerator)
//  _a      :   filter coefficients (denominator)
//  _h_len  :   filter coefficients length
//  _x      :   input array
//  _x_len  :   input array length
//  _y      :   output array
//  _y_len  :   output array length
void iirfilt_rrrf_test(float *      _b,
                       float *      _a,
                       unsigned int _h_len,
                       float *      _x,
                       unsigned int _x_len,
                       float *      _y,
                       unsigned int _y_len);

void iirfilt_crcf_test(float *         _b,
                       float *         _a,
                       unsigned int    _h_len,
                       float complex * _x,
                       unsigned int    _x_len,
                       float complex * _y,
                       unsigned int    _y_len);

void iirfilt_cccf_test(float complex * _b,
                       float complex * _a,
                       unsigned int    _h_len,
                       float complex * _x,
                       unsigned int    _x_len,
                       float complex * _y,
                       unsigned int    _y_len);

// 
// autotest datasets
//

// rrrf
extern float         iirfilt_rrrf_data_h3x64_b[];
extern float         iirfilt_rrrf_data_h3x64_a[];
extern float         iirfilt_rrrf_data_h3x64_x[];
extern float         iirfilt_rrrf_data_h3x64_y[];

extern float         iirfilt_rrrf_data_h5x64_b[];
extern float         iirfilt_rrrf_data_h5x64_a[];
extern float         iirfilt_rrrf_data_h5x64_x[];
extern float         iirfilt_rrrf_data_h5x64_y[];

extern float         iirfilt_rrrf_data_h7x64_b[];
extern float         iirfilt_rrrf_data_h7x64_a[];
extern float         iirfilt_rrrf_data_h7x64_x[];
extern float         iirfilt_rrrf_data_h7x64_y[];

// crcf
extern float         iirfilt_crcf_data_h3x64_b[];
extern float         iirfilt_crcf_data_h3x64_a[];
extern float complex iirfilt_crcf_data_h3x64_x[];
extern float complex iirfilt_crcf_data_h3x64_y[];

extern float         iirfilt_crcf_data_h5x64_b[];
extern float         iirfilt_crcf_data_h5x64_a[];
extern float complex iirfilt_crcf_data_h5x64_x[];
extern float complex iirfilt_crcf_data_h5x64_y[];

extern float         iirfilt_crcf_data_h7x64_b[];
extern float         iirfilt_crcf_data_h7x64_a[];
extern float complex iirfilt_crcf_data_h7x64_x[];
extern float complex iirfilt_crcf_data_h7x64_y[];

// cccf
extern float complex iirfilt_cccf_data_h3x64_b[];
extern float complex iirfilt_cccf_data_h3x64_a[];
extern float complex iirfilt_cccf_data_h3x64_x[];
extern float complex iirfilt_cccf_data_h3x64_y[];

extern float complex iirfilt_cccf_data_h5x64_b[];
extern float complex iirfilt_cccf_data_h5x64_a[];
extern float complex iirfilt_cccf_data_h5x64_x[];
extern float complex iirfilt_cccf_data_h5x64_y[];

extern float complex iirfilt_cccf_data_h7x64_b[];
extern float complex iirfilt_cccf_data_h7x64_a[];
extern float complex iirfilt_cccf_data_h7x64_x[];
extern float complex iirfilt_cccf_data_h7x64_y[];

#endif // __LIQUID_IIRFILT_H__

