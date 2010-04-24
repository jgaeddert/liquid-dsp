/*
 * Copyright (c) 2007, 2008, 2009, 2010 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010 Virginia Polytechnic
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
// Design root-Nyquist Kaiser filter
//
// References
//  [Vaidyanathan:1993] Vaidyanathan, P. P., "Multirate Systems and
//      Filter Banks," 1993, Prentice Hall, Section 3.2.1
//

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "liquid.internal.h"

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
    // simply call internal method, ignoring gamma
    float gamma;
    design_rkaiser_filter_internal(_k,_m,_beta,_dt,_h,&gamma);
}

// design_rkaiser_filter_internal()
//
// Design frequency-shifted root-Nyquist filter based on
// the Kaiser-windowed sinc.
//
//  _k      :   filter over-sampling rate (samples/symbol)
//  _m      :   filter delay (symbols)
//  _beta   :   filter excess bandwidth factor (0,1)
//  _dt     :   filter fractional sample delay
//  _h      :   resulting filter [size: 2*_k*_m+1]
//  _gamma  :   transition bandwidth adjustment, 0 < _gamma < 1
void design_rkaiser_filter_internal(unsigned int _k,
                                    unsigned int _m,
                                    float _beta,
                                    float _dt,
                                    float * _h,
                                    float * _gamma)
{
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

    unsigned int n=2*_k*_m+1;   // filter length
    float del;                  // transition bandwidth
    float As;                   // sidelobe attenuation
    float fc;                   // filter cutoff
    float h[n];                 // temporary coefficients array

    // bandwidth adjustment array (3 points makes a parabola)
    float p0 = (_m == 1) ? -0.12861f : 0.011*logf(_m-1.8f) - 0.046f;
    float p1 = (_m == 1) ?  0.90364f : 0.0064437f*_m + 0.863301f;
    float gamma_hat = p1*_beta + p0;  // initial estimate
    if (gamma_hat < _beta*0.10f) gamma_hat = 0.10f*_beta;
    if (gamma_hat > _beta*0.95f) gamma_hat = 0.95f*_beta;
    float x[3] = {
        gamma_hat*0.9f,
        gamma_hat,
        _beta*0.98f};

    // evaluate performance (ISI) of each bandwidth adjustment
    float isi_max;
    float isi_mse;
    float y[3];
    for (i=0; i<3; i++) {
        // re-compute transition band, As, and cutoff frequency
        del = x[i] / kf;
        As  = 14.26f*del*n + 7.95f;
        fc  = (1 + _beta - x[i])/kf;

        // compute filter, isi
        fir_kaiser_window(n,fc,As,_dt,h);
        liquid_filter_isi(h,_k,_m,&isi_mse,&isi_max);
        y[i] = isi_mse;
    }

    // run parabolic search to find bandwidth adjustment x_hat which
    // minimizes the inter-symbol interference of the filter
    unsigned int p, pmax=10;
    float t0, t1;
    float x_hat = x[1];
    float y_hat;
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
        if (x_hat < 0) {
            printf("gamma too small; exiting prematurely\n");
            x_hat = 0.01f;
            break;
        } else if (x_hat > _beta) {
            printf("gamma too large; exiting prematurely\n");
            x_hat = _beta;
            break;
        }

        // re-compute transition band, As, and cutoff frequency
        del = x_hat / kf;
        As  = 14.26f*del*n + 7.95f;
        fc  = (1 + _beta - x_hat)/kf;

        // execute filter design
        fir_kaiser_window(n,fc,As,_dt,h);

        // compute inter-symbol interference (MSE, max)
        liquid_filter_isi(h,_k,_m,&isi_mse,&isi_max);
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
        del = x_hat / kf;
        As  = 14.26f*del*n + 7.95f;
        fc  = (1 + _beta - x_hat)/kf;
    fir_kaiser_window(n,fc,As,_dt,_h);
    float e2 = 0.0f;
    for (i=0; i<n; i++) e2 += _h[i]*_h[i];
    for (i=0; i<n; i++) _h[i] *= sqrtf(_k/e2);

    // save trasition bandwidth adjustment
    *_gamma = x_hat;
}

