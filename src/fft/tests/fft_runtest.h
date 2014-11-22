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
// autotest fft data definitions
//

#ifndef __LIQUID_FFT_DATA_H__
#define __LIQUID_FFT_DATA_H__

// autotest helper function
//  _x      :   fft input array
//  _test   :   expected fft output
//  _n      :   fft size
void fft_test(float complex * _x,
              float complex * _test,
              unsigned int    _n);

// 
// autotest datasets
//

//
extern float complex fft_test_x2[];
extern float complex fft_test_y2[];

extern float complex fft_test_x3[];
extern float complex fft_test_y3[];

extern float complex fft_test_x4[];
extern float complex fft_test_y4[];

extern float complex fft_test_x5[];
extern float complex fft_test_y5[];

extern float complex fft_test_x6[];
extern float complex fft_test_y6[];

extern float complex fft_test_x7[];
extern float complex fft_test_y7[];

extern float complex fft_test_x8[];
extern float complex fft_test_y8[];

extern float complex fft_test_x9[];
extern float complex fft_test_y9[];

//
extern float complex fft_test_x10[];
extern float complex fft_test_y10[];

extern float complex fft_test_x16[];
extern float complex fft_test_y16[];

extern float complex fft_test_x17[];
extern float complex fft_test_y17[];

extern float complex fft_test_x20[];
extern float complex fft_test_y20[];

extern float complex fft_test_x21[];
extern float complex fft_test_y21[];

extern float complex fft_test_x22[];
extern float complex fft_test_y22[];

extern float complex fft_test_x24[];
extern float complex fft_test_y24[];

extern float complex fft_test_x26[];
extern float complex fft_test_y26[];

extern float complex fft_test_x30[];
extern float complex fft_test_y30[];

extern float complex fft_test_x32[];
extern float complex fft_test_y32[];

extern float complex fft_test_x35[];
extern float complex fft_test_y35[];

extern float complex fft_test_x36[];
extern float complex fft_test_y36[];

extern float complex fft_test_x43[];
extern float complex fft_test_y43[];

extern float complex fft_test_x48[];
extern float complex fft_test_y48[];

extern float complex fft_test_x63[];
extern float complex fft_test_y63[];

extern float complex fft_test_x64[];
extern float complex fft_test_y64[];

extern float complex fft_test_x79[];
extern float complex fft_test_y79[];

extern float complex fft_test_x92[];
extern float complex fft_test_y92[];

extern float complex fft_test_x96[];
extern float complex fft_test_y96[];

//
extern float complex fft_test_x120[];
extern float complex fft_test_y120[];

extern float complex fft_test_x130[];
extern float complex fft_test_y130[];

extern float complex fft_test_x157[];
extern float complex fft_test_y157[];

extern float complex fft_test_x192[];
extern float complex fft_test_y192[];

extern float complex fft_test_x317[];
extern float complex fft_test_y317[];

extern float complex fft_test_x509[];
extern float complex fft_test_y509[];

// 8-point real even/odd dft data
extern float fftdata_r2r_x8[];
extern float fftdata_r2r_REDFT00_y8[];
extern float fftdata_r2r_REDFT10_y8[];
extern float fftdata_r2r_REDFT01_y8[];
extern float fftdata_r2r_REDFT11_y8[];
extern float fftdata_r2r_RODFT00_y8[];
extern float fftdata_r2r_RODFT10_y8[];
extern float fftdata_r2r_RODFT01_y8[];
extern float fftdata_r2r_RODFT11_y8[];

// 27-point real even/odd dft data
extern float fftdata_r2r_x27[];
extern float fftdata_r2r_REDFT00_y27[];
extern float fftdata_r2r_REDFT10_y27[];
extern float fftdata_r2r_REDFT01_y27[];
extern float fftdata_r2r_REDFT11_y27[];
extern float fftdata_r2r_RODFT00_y27[];
extern float fftdata_r2r_RODFT10_y27[];
extern float fftdata_r2r_RODFT01_y27[];
extern float fftdata_r2r_RODFT11_y27[];

// 32-point real even/odd dft data
extern float fftdata_r2r_x32[];
extern float fftdata_r2r_REDFT00_y32[];
extern float fftdata_r2r_REDFT10_y32[];
extern float fftdata_r2r_REDFT01_y32[];
extern float fftdata_r2r_REDFT11_y32[];
extern float fftdata_r2r_RODFT00_y32[];
extern float fftdata_r2r_RODFT10_y32[];
extern float fftdata_r2r_RODFT01_y32[];
extern float fftdata_r2r_RODFT11_y32[];

#endif // __LIQUID_FFT_DATA_H__
