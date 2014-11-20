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
// autotest fftfilt data definitions
//

#ifndef __LIQUID_FFTFILT_AUTOTEST_H__
#define __LIQUID_FFTFILT_AUTOTEST_H__

// autotest helper functions:
//  _h      :   filter coefficients
//  _h_len  :   filter coefficients length
//  _x      :   input array
//  _x_len  :   input array length
//  _y      :   output array
//  _y_len  :   output array length
void fftfilt_rrrf_test(float *      _h,
                       unsigned int _h_len,
                       float *      _x,
                       unsigned int _x_len,
                       float *      _y,
                       unsigned int _y_len);

void fftfilt_crcf_test(float *         _h,
                       unsigned int    _h_len,
                       float complex * _x,
                       unsigned int    _x_len,
                       float complex * _y,
                       unsigned int    _y_len);

void fftfilt_cccf_test(float complex * _h,
                       unsigned int    _h_len,
                       float complex * _x,
                       unsigned int    _x_len,
                       float complex * _y,
                       unsigned int    _y_len);

// 
// autotest datasets
//

// rrrf
extern float         fftfilt_rrrf_data_h4x256_h[];
extern float         fftfilt_rrrf_data_h4x256_x[];
extern float         fftfilt_rrrf_data_h4x256_y[];

extern float         fftfilt_rrrf_data_h7x256_h[];
extern float         fftfilt_rrrf_data_h7x256_x[];
extern float         fftfilt_rrrf_data_h7x256_y[];

extern float         fftfilt_rrrf_data_h13x256_h[];
extern float         fftfilt_rrrf_data_h13x256_x[];
extern float         fftfilt_rrrf_data_h13x256_y[];

extern float         fftfilt_rrrf_data_h23x256_h[];
extern float         fftfilt_rrrf_data_h23x256_x[];
extern float         fftfilt_rrrf_data_h23x256_y[];

// crcf
extern float         fftfilt_crcf_data_h4x256_h[];
extern float complex fftfilt_crcf_data_h4x256_x[];
extern float complex fftfilt_crcf_data_h4x256_y[];

extern float         fftfilt_crcf_data_h7x256_h[];
extern float complex fftfilt_crcf_data_h7x256_x[];
extern float complex fftfilt_crcf_data_h7x256_y[];

extern float         fftfilt_crcf_data_h13x256_h[];
extern float complex fftfilt_crcf_data_h13x256_x[];
extern float complex fftfilt_crcf_data_h13x256_y[];

extern float         fftfilt_crcf_data_h23x256_h[];
extern float complex fftfilt_crcf_data_h23x256_x[];
extern float complex fftfilt_crcf_data_h23x256_y[];

// cccf
extern float complex fftfilt_cccf_data_h4x256_h[];
extern float complex fftfilt_cccf_data_h4x256_x[];
extern float complex fftfilt_cccf_data_h4x256_y[];

extern float complex fftfilt_cccf_data_h7x256_h[];
extern float complex fftfilt_cccf_data_h7x256_x[];
extern float complex fftfilt_cccf_data_h7x256_y[];

extern float complex fftfilt_cccf_data_h13x256_h[];
extern float complex fftfilt_cccf_data_h13x256_x[];
extern float complex fftfilt_cccf_data_h13x256_y[];

extern float complex fftfilt_cccf_data_h23x256_h[];
extern float complex fftfilt_cccf_data_h23x256_x[];
extern float complex fftfilt_cccf_data_h23x256_y[];

#endif // __LIQUID_FFTFILT_AUTOTEST_H__

