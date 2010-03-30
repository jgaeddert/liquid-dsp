/*
 * Copyright (c) 2007, 2009 Joseph Gaeddert
 * Copyright (c) 2007, 2009 Virginia Polytechnic Institute & State University
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
// Design root-Nyquist Kiser filter
//

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "liquid.internal.h"

#define LIQUID_RKAISER_DEBUG_FILENAME "rkaiser_internal_debug.m"

// rkaiser_autocorr()
//
// Compute auto-correlation of filter at a specific lag.
//
//  _h      :   filter coefficients [size: _h_len]
//  _h_len  :   filter length
//  _lag    :   auto-correlation lag (samples)
float rkaiser_autocorr(float * _h,
                       unsigned int _h_len,
                       int _lag)
{
    // auto-correlation is even symmetric
    _lag = abs(_lag);

    // lag outside of filter length is zero
    if (_lag >= _h_len) return 0.0f;

    // compute auto-correlation
    float rxx=0.0f; // initialize auto-correlation to zero
    unsigned int i;
    for (i=_lag; i<_h_len; i++)
        rxx += _h[i] * _h[i-_lag];

    return rxx;
}

// rkaiser_compute_isi()
//
// Compute inter-symbol interference (ISI)--both MSE and
// maximum--for the filter _h.
//
//  _h      :   filter coefficients [size: 2*_k*_m+1]
//  _k      :   filter over-sampling rate (samples/symbol)
//  _m      :   filter delay (symbols)
//  _mse    :   output mean-squared ISI
//  _max    :   maximum ISI
void rkaiser_compute_isi(float * _h,
                         unsigned int _k,
                         unsigned int _m,
                         float * _mse,
                         float * _max)
{
    unsigned int h_len = 2*_k*_m+1;

    // compute zero-lag auto-correlation
    float rxx0 = rkaiser_autocorr(_h,h_len,0);
    //printf("rxx0 = %12.8f\n", rxx0);
    //exit(1);

    unsigned int i;
    float isi_mse = 0.0f;
    float isi_max = 0.0f;
    float e;
    for (i=1; i<=2*_m; i++) {
        e = rkaiser_autocorr(_h,h_len,i*_k) / rxx0;
        e = fabsf(e);

        isi_mse += e*e;
        
        if (i==1 || e > isi_max)
            isi_max = e;
    }

    *_mse = isi_mse / (float)(2*_m);
    *_max = isi_max;
}

// design_rkaiser_filter()
//
// Design frequency-shifted root-Nyquist filter based on
// the Kaiser-windowed sinc.
//
//  _k      :   filter over-sampling rate (samples/symbol)
//  _m      :   filter delay (symbols)
//  _beta   :   filter excess bandwidth factor (0,1)
//  _dt     :   filter fractional sample delay
//  _h      :   resulting filter [size: 2*_k*_m+1]
void design_rkaiser_filter(unsigned int _k,
                           unsigned int _m,
                           float _beta,
                           float _dt,
                           float * _h)
{
    unsigned int h_len;

    if ( _k < 1 ) {
        printf("error: design_rkaiser_filter(): k must be greater than 0\n");
        exit(0);
    } else if ( _m < 1 ) {
        printf("error: design_rkaiser_filter(): m must be greater than 0\n");
        exit(0);
    } else if ( (_beta < 0.0f) || (_beta > 1.0f) ) {
        printf("error: design_rkaiser_filter(): beta must be in [0,1]\n");
        exit(0);
    } else;

    unsigned int i;
    float kf = (float)_k;

    unsigned int n=2*_k*_m+1;           // filter length
    float del = _beta/kf;               // transition bandwidth
    float As = 14.26f*del*n + 7.95f;    // sidelobe attenuation
    float fc = 1.0f / kf;               // filter cutoff
    float h[n];                         // temporary coefficients array

    // bandwidth adjustment array (3 points makes a parabola)
    float x[3] = {
        0.0f,
        0.2f * _beta / kf,
        0.4f * _beta / kf};

    // evaluate performance (ISI) of each bandwidth adjustment
    float isi_max;
    float isi_mse;
    float y[3];
    for (i=0; i<3; i++) {
        // re-compute transition band and As
        del = (_beta / kf) - x[i];
        As  = 14.26f*del*n + 7.95f;

        // compute filter, isi
        fir_kaiser_window(n,fc+x[i],As,_dt,h);
        rkaiser_compute_isi(h,_k,_m,&isi_mse,&isi_max);
        y[i] = isi_mse;
    }

    // run parabolic search to find bandwidth adjustment x_hat which
    // minimizes the inter-symbol interference of the filter
    unsigned int p, pmax=10;
    float t0, t1;
    float x_hat = x[1];
    float y_hat;
    float y_prime=0;
    unsigned int imax;
    for (p=0; p<pmax; p++) {
        // numerator
        t0 = y[0] * (x[1]*x[1] - x[2]*x[2]) +
             y[1] * (x[2]*x[2] - x[0]*x[0]) +
             y[2] * (x[0]*x[0] - x[1]*x[1]);

        // denominator
        t1 = y[0] * (x[1] - x[2]) +
             y[1] * (x[2] - x[0]) +
             y[2] * (x[0] - x[1]);

        // break if denominator is sufficiently small
        if (fabsf(t1) < 1e-12f) break;

        // compute new estimate
        x_hat = 0.5f * t0 / t1;

        // re-compute transition band and As
        del = (_beta / kf) - x_hat;
        As  = 14.26f*del*n + 7.95f;

        // execute filter design
        fir_kaiser_window(n,fc+x_hat,As,_dt,h);

        // compute inter-symbol interference (MSE, max)
        rkaiser_compute_isi(h,_k,_m,&isi_mse,&isi_max);
        y_hat = isi_mse;

        // search index of maximum
        if      (y[0] > y[1] && y[0] > y[2])    imax = 0;
        else if (y[1] > y[0] && y[1] > y[2])    imax = 1;
        else                                    imax = 2;

        // replace old estimate
        x[imax] = x_hat;
        y[imax] = y_hat;

        //printf("  %4u : x_hat=%12.8f, y_hat=%20.8e\n", p+1, x_hat, y_hat);
    };

    // compute optimum filter and normalize
    fir_kaiser_window(n,fc+x_hat,As,_dt,_h);
    float e2 = 0.0f;
    for (i=0; i<n; i++) e2 += _h[i]*_h[i];
    for (i=0; i<n; i++) _h[i] *= sqrtf(_k/e2);
}

