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
// autotest firdecim data definitions
//

#ifndef __LIQUID_FIRDECIM_H__
#define __LIQUID_FIRDECIM_H__

// autotest helper functions:
//  _M      :   decimation factor
//  _h      :   filter coefficients
//  _h_len  :   filter coefficients length
//  _x      :   input array
//  _x_len  :   input array length
//  _y      :   output array
//  _y_len  :   output array length
void firdecim_rrrf_test(unsigned int _M,
                        float *      _h,
                        unsigned int _h_len,
                        float *      _x,
                        unsigned int _x_len,
                        float *      _y,
                        unsigned int _y_len);

void firdecim_crcf_test(unsigned int    _M,
                        float *         _h,
                        unsigned int    _h_len,
                        float complex * _x,
                        unsigned int    _x_len,
                        float complex * _y,
                        unsigned int    _y_len);

void firdecim_cccf_test(unsigned int    _M,
                        float complex * _h,
                        unsigned int    _h_len,
                        float complex * _x,
                        unsigned int    _x_len,
                        float complex * _y,
                        unsigned int    _y_len);

// 
// autotest datasets
//

// rrrf
extern float         firdecim_rrrf_data_M2h4x20_h[];
extern float         firdecim_rrrf_data_M2h4x20_x[];
extern float         firdecim_rrrf_data_M2h4x20_y[];

extern float         firdecim_rrrf_data_M3h7x30_h[];
extern float         firdecim_rrrf_data_M3h7x30_x[];
extern float         firdecim_rrrf_data_M3h7x30_y[];

extern float         firdecim_rrrf_data_M4h13x40_h[];
extern float         firdecim_rrrf_data_M4h13x40_x[];
extern float         firdecim_rrrf_data_M4h13x40_y[];

extern float         firdecim_rrrf_data_M5h23x50_h[];
extern float         firdecim_rrrf_data_M5h23x50_x[];
extern float         firdecim_rrrf_data_M5h23x50_y[];

// crcf
extern float         firdecim_crcf_data_M2h4x20_h[];
extern float complex firdecim_crcf_data_M2h4x20_x[];
extern float complex firdecim_crcf_data_M2h4x20_y[];

extern float         firdecim_crcf_data_M3h7x30_h[];
extern float complex firdecim_crcf_data_M3h7x30_x[];
extern float complex firdecim_crcf_data_M3h7x30_y[];

extern float         firdecim_crcf_data_M4h13x40_h[];
extern float complex firdecim_crcf_data_M4h13x40_x[];
extern float complex firdecim_crcf_data_M4h13x40_y[];

extern float         firdecim_crcf_data_M5h23x50_h[];
extern float complex firdecim_crcf_data_M5h23x50_x[];
extern float complex firdecim_crcf_data_M5h23x50_y[];

// cccf
extern float complex firdecim_cccf_data_M2h4x20_h[];
extern float complex firdecim_cccf_data_M2h4x20_x[];
extern float complex firdecim_cccf_data_M2h4x20_y[];

extern float complex firdecim_cccf_data_M3h7x30_h[];
extern float complex firdecim_cccf_data_M3h7x30_x[];
extern float complex firdecim_cccf_data_M3h7x30_y[];

extern float complex firdecim_cccf_data_M4h13x40_h[];
extern float complex firdecim_cccf_data_M4h13x40_x[];
extern float complex firdecim_cccf_data_M4h13x40_y[];

extern float complex firdecim_cccf_data_M5h23x50_h[];
extern float complex firdecim_cccf_data_M5h23x50_x[];
extern float complex firdecim_cccf_data_M5h23x50_y[];

#endif // __LIQUID_FIRDECIM_H__

