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
    // simply call internal method and ignore output gamma value
    float gamma;
    design_rkaiser_filter_internal(_k,_m,_beta,_dt,_h,&gamma);
}

// design_arkaiser_filter()
//
// Design frequency-shifted root-Nyquist filter based on
// the Kaiser-windowed sinc using approximation for rho.
//
//  _k      :   filter over-sampling rate (samples/symbol)
//  _m      :   filter delay (symbols)
//  _beta   :   filter excess bandwidth factor (0,1)
//  _dt     :   filter fractional sample delay
//  _h      :   resulting filter [size: 2*_k*_m+1]
void design_arkaiser_filter(unsigned int _k,
                            unsigned int _m,
                            float _beta,
                            float _dt,
                            float * _h)
{
    // compute bandwidth adjustment estimate
    float rho_hat = rkaiser_approximate_rho(_m,_beta);
    float gamma_hat = rho_hat*_beta;

    unsigned int n=2*_k*_m+1;                       // filter length
    float del = gamma_hat / (float)_k;              // transition bandwidth
    float As  = 14.26f*del*n + 7.95f;               // sidelobe attenuation
    float fc  = (1 + _beta - gamma_hat)/(float)_k;  // filter cutoff

    // compute filter coefficients
    fir_kaiser_window(n,fc,As,_dt,_h);

    // normalize coefficients
    float e2 = 0.0f;
    unsigned int i;
    for (i=0; i<n; i++) e2 += _h[i]*_h[i];
    for (i=0; i<n; i++) _h[i] *= sqrtf(_k/e2);
}

// Find approximate bandwidth adjustment factor rho based on
// filter delay and desired excess bandwdith factor.
//
//  _m      :   filter delay (symbols)
//  _beta   :   filter excess bandwidth factor (0,1)
float rkaiser_approximate_rho(unsigned int _m,
                              float _beta)
{
    if ( _m < 1 ) {
        fprintf(stderr,"error: rkaiser_approximate_rho(): m must be greater than 0\n");
        exit(0);
    } else if ( (_beta < 0.0f) || (_beta > 1.0f) ) {
        fprintf(stderr,"error: rkaiser_approximate_rho(): beta must be in [0,1]\n");
        exit(0);
    } else;

    // compute bandwidth adjustment estimate
    float c0=0.0f, c1=0.0f, c2=0.0f;
    switch (_m) {
    case 1:  c0=0.78583556; c1=0.05439958; c2=0.37818679; break;
    case 2:  c0=0.82194722; c1=0.06170731; c2=0.16362774; break;
    case 3:  c0=0.84686762; c1=0.07475776; c2=0.05263769; break;
    case 4:  c0=0.86538726; c1=0.07374587; c2=0.03491642; break;
    case 5:  c0=0.87861007; c1=0.06981039; c2=0.03553645; break;
    case 6:  c0=0.88901162; c1=0.06708569; c2=0.03459680; break;
    default:
             c0 = 0.057918*logf(_m) + 0.784313;
             c1 = _m <= 3 ?
                     0.0099427*_m + 0.0447250 :
                    -0.0026685*_m + 0.0835030;
             c2 = 0.03373 + expf((-0.30382*_m*_m -0.19451*_m -0.56171));
    }
    // ensure no invalid log taken
    if (c2 >= _beta)
        c2 = 0.999f*_beta;

    float rho_hat = c0 + c1*logf(_beta - c2);

    // ensure estimate is in [0,1]
    if (rho_hat < 0.0f) {
        rho_hat = 0.0f;
    } else if (rho_hat > 1.0f) {
        rho_hat = 1.0f;
    }

    return rho_hat;
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

    // compute bandwidth adjustment estimate
    float rho_hat = rkaiser_approximate_rho(_m,_beta);
    float gamma_hat = rho_hat*_beta;

    // bandwidth adjustment array (3 points makes a parabola)
    float x[3] = {
        gamma_hat*0.9f,
        gamma_hat,
        gamma_hat*1.1f};

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

