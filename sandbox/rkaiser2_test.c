/*
 * Copyright (c) 2011 Joseph Gaeddert
 * Copyright (c) 2011 Virginia Polytechnic Institute & State University
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

#define DEBUG 0
#define OUTPUT_FILENAME "rkaiser2_test.m"

// Design frequency-shifted root-Nyquist filter based on
// the Kaiser-windowed sinc.
//
//  _k      :   filter over-sampling rate (samples/symbol)
//  _m      :   filter delay (symbols)
//  _beta   :   filter excess bandwidth factor (0,1)
//  _dt     :   filter fractional sample delay
//  _h      :   resulting filter [size: 2*_k*_m+1]
void design_rkaiser_filter2(unsigned int _k,
                            unsigned int _m,
                            float _beta,
                            float _dt,
                            float * _h);

// gradient search utility
float gs_utility(void * _userdata,
                 float * _h,
                 unsigned int _n);

int main() {
    // options
    unsigned int k=2;
    unsigned int m=3;
    float beta = 0.3;
    float dt = 0.0;

    // derived values
    unsigned int h_len = 2*k*m+1;
    float h1[h_len];
    float h2[h_len];

    design_rkaiser_filter(k,m,beta,dt,h1);
    design_rkaiser_filter2(k,m,beta,dt,h2);

    // print filters

    // export results
    FILE * fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all\n");
    fprintf(fid,"close all\n");
    fprintf(fid,"\n");
    fprintf(fid,"n  = %u;\n", h_len);
    fprintf(fid,"h1 = zeros(1,n);\n");
    fprintf(fid,"h2 = zeros(1,n);\n");

    unsigned int i;
    for (i=0; i<h_len; i++) {
        fprintf(fid,"  h1(%3u) = %12.4e;\n", i+1, h1[i]);
        fprintf(fid,"  h2(%3u) = %12.4e;\n", i+1, h2[i]);
    }
    fprintf(fid,"\n");
    fprintf(fid,"nfft=1024;\n");
    fprintf(fid,"f = [0:(nfft-1)]/nfft - 0.5;\n");
    fprintf(fid,"H1 = 20*log10(abs(fftshift(fft(h1,nfft))));\n");
    fprintf(fid,"H2 = 20*log10(abs(fftshift(fft(h2,nfft))));\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(f,H1, f,H2);\n");
    fprintf(fid,"axis([-0.5 0.5 -100 20]);\n");

    fclose(fid);
    printf("results written to '%s'\n", OUTPUT_FILENAME);

    printf("done.\n");
    return 0;
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
void design_rkaiser_filter2(unsigned int _k,
                            unsigned int _m,
                            float _beta,
                            float _dt,
                            float * _h)
{
    // validate input
    if (_k < 2) {
        fprintf(stderr,"error: design_rkaiser_filter(), k must be at least 2\n");
        exit(1);
    } else if (_m < 1) {
        fprintf(stderr,"error: design_rkaiser_filter(), m must be at least 1\n");
        exit(1);
    } else if (_beta <= 0.0f || _beta >= 1.0f) {
        fprintf(stderr,"error: design_rkaiser_filter(), beta must be in (0,1)\n");
        exit(1);
    } else if (_dt < -1.0f || _dt > 1.0f) {
        fprintf(stderr,"error: design_rkaiser_filter(), dt must be in [-1,1]\n");
        exit(1);
    }

    // TODO : set options in userdata object

    // 
    float fc = 0.5*(1.0 + _beta)/(float)(_k);
    float As = 40.0f;
    float v[2] = {fc, As};

    // create gradsearch object
    gradsearch gs = gradsearch_create_advanced(NULL,
            v, 2,       // vector optimizer
            1e-6f,      // delta: gradient step size
            0.002f,     // gamma: vector step size
            0.1f,       // alpha: momentum parameter
            0.999f,     // mu:    decremental gamma parameter
            gs_utility,
            LIQUID_OPTIM_MINIMIZE);

    // run search
    unsigned int i;
    unsigned int num_iterations = 50;
    float utility;
    for (i=0; i<num_iterations; i++) {
        utility = gs_utility(NULL,v,2);

        gradsearch_step(gs);

        gradsearch_print(gs);
    }

    // destroy gradient search object
    gradsearch_destroy(gs);

    // re-design filter and return
    unsigned int h_len = 2*_k*_m + 1;
    fc = v[0];
    As = v[1];
    firdes_kaiser_window(h_len,fc,As,_dt,_h);

    // normalize coefficients
    float e2 = 0.0f;
    for (i=0; i<h_len; i++) e2 += _h[i]*_h[i];
    for (i=0; i<h_len; i++) _h[i] *= sqrtf(_k/e2);
}

// gradient search utility
float gs_utility(void * _userdata,
                 float * _v,
                 unsigned int _n)
{
    // TODO : get options from _userdata object
    float w0 = 0.5;
    float w1 = 0.5;
    unsigned int k=2;
    unsigned int m=3;
    float beta = 0.3;
    float dt = 0.0;
    unsigned int nfft=512;

    // parameters
    float fc = _v[0];
    float As = _v[1];

    // derived values
    unsigned int h_len = 2*k*m+1;

    // utilities
    float u0 = 0.0;
    float u1 = 0.0;

    // compute filter
    float h[h_len];
    firdes_kaiser_window(h_len,fc,As,dt,h);

    // normalize coefficients
    float e2 = 0.0f;
    unsigned int i;
    for (i=0; i<h_len; i++) e2 += h[i]*h[i];
    for (i=0; i<h_len; i++) h[i] *= sqrtf(k/e2);

    // compute filter ISI
    float isi_max;
    float isi_rms;
    liquid_filter_isi(h,k,m,&isi_rms,&isi_max);
    u0 = 20*log10f(isi_rms);

    // compute relative out-of-band energy
    float e = liquid_filter_energy(h, h_len, 0.5*(1.0+beta)/(float)k, nfft);
    u1 = 20*log10f(e);

#if DEBUG
    printf("  ISI (rms) : %12.8f, energy : %12.8f\n", u0, u1);
#endif

    // combine results
    return w0*u0 + w1*u1;
}

