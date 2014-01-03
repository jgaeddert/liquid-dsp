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
// autotest firfilt data definitions
//

#ifndef __LIQUID_FIRFILT_H__
#define __LIQUID_FIRFILT_H__

// autotest helper functions:
//  _h      :   filter coefficients
//  _h_len  :   filter coefficients length
//  _x      :   input array
//  _x_len  :   input array length
//  _y      :   output array
//  _y_len  :   output array length
void firfilt_rrrf_test(float *      _h,
                       unsigned int _h_len,
                       float *      _x,
                       unsigned int _x_len,
                       float *      _y,
                       unsigned int _y_len);

void firfilt_crcf_test(float *         _h,
                       unsigned int    _h_len,
                       float complex * _x,
                       unsigned int    _x_len,
                       float complex * _y,
                       unsigned int    _y_len);

void firfilt_cccf_test(float complex * _h,
                       unsigned int    _h_len,
                       float complex * _x,
                       unsigned int    _x_len,
                       float complex * _y,
                       unsigned int    _y_len);

// 
// autotest datasets
//

// rrrf
extern float         firfilt_rrrf_data_h4x8_h[];
extern float         firfilt_rrrf_data_h4x8_x[];
extern float         firfilt_rrrf_data_h4x8_y[];

extern float         firfilt_rrrf_data_h7x16_h[];
extern float         firfilt_rrrf_data_h7x16_x[];
extern float         firfilt_rrrf_data_h7x16_y[];

extern float         firfilt_rrrf_data_h13x32_h[];
extern float         firfilt_rrrf_data_h13x32_x[];
extern float         firfilt_rrrf_data_h13x32_y[];

extern float         firfilt_rrrf_data_h23x64_h[];
extern float         firfilt_rrrf_data_h23x64_x[];
extern float         firfilt_rrrf_data_h23x64_y[];

// crcf
extern float         firfilt_crcf_data_h4x8_h[];
extern float complex firfilt_crcf_data_h4x8_x[];
extern float complex firfilt_crcf_data_h4x8_y[];

extern float         firfilt_crcf_data_h7x16_h[];
extern float complex firfilt_crcf_data_h7x16_x[];
extern float complex firfilt_crcf_data_h7x16_y[];

extern float         firfilt_crcf_data_h13x32_h[];
extern float complex firfilt_crcf_data_h13x32_x[];
extern float complex firfilt_crcf_data_h13x32_y[];

extern float         firfilt_crcf_data_h23x64_h[];
extern float complex firfilt_crcf_data_h23x64_x[];
extern float complex firfilt_crcf_data_h23x64_y[];

// cccf
extern float complex firfilt_cccf_data_h4x8_h[];
extern float complex firfilt_cccf_data_h4x8_x[];
extern float complex firfilt_cccf_data_h4x8_y[];

extern float complex firfilt_cccf_data_h7x16_h[];
extern float complex firfilt_cccf_data_h7x16_x[];
extern float complex firfilt_cccf_data_h7x16_y[];

extern float complex firfilt_cccf_data_h13x32_h[];
extern float complex firfilt_cccf_data_h13x32_x[];
extern float complex firfilt_cccf_data_h13x32_y[];

extern float complex firfilt_cccf_data_h23x64_h[];
extern float complex firfilt_cccf_data_h23x64_x[];
extern float complex firfilt_cccf_data_h23x64_y[];

#endif // __LIQUID_FIRFILT_H__

