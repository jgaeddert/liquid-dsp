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

float filter_autocorr(float * _h,
                      unsigned int _h_len,
                      int _lag)
{
    // auto-correlation is even symmetric
    _lag = abs(_lag);

    // validate input
    if (_lag >= _h_len)
        return 0.0f;

    // initialize auto-correlation to zero
    float rxx=0.0f;

    // compute auto-correlation
    unsigned int i;
    for (i=_lag; i<_h_len; i++) {
        rxx += _h[i] * _h[i-_lag];
    }

    return rxx;
}

void filter_compute_isi(float * _h,
                        unsigned int _k,
                        unsigned int _m,
                        float * _mse,
                        float * _max)
{
    unsigned int h_len = 2*_k*_m+1;

    // compute zero-lag auto-correlation
    //float rxx0 = filter_autocorr(_h,h_len,0);

    unsigned int i;
    float isi_mse = 0.0f;
    float isi_max = 0.0f;
    float e;
    for (i=1; i<_m; i++) {
        e = filter_autocorr(_h,h_len,i*_k);
        e = fabsf(e);

        isi_mse += e*e;
        
        if (i==1 || e > isi_max)
            isi_max = e;
    }

    *_mse = isi_mse / (float)(_m-1);
    *_max = isi_max;
}

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

    unsigned int n=2*_k*_m+1;           // filter length
    float del = 0.5f*_beta/(float)(_k); // transition bandwidth
    float As = 14.26f*del*n + 7.95f;    // sidelobe attenuation
    //As = 60.0f;
    printf("As = %12.8f\n", As);
    //exit(1);

    float fc = 1.0f / (float)(_k);  // filter cutoff

    float h[n];
    fir_kaiser_window(n,fc,As,_dt,h);
    float e2 = 0.0f;
    for (i=0; i<n; i++) e2 += h[i]*h[i];
    for (i=0; i<n; i++) h[i] /= sqrtf(e2*_k);
    // copy results
    memmove(_h, h, n*sizeof(float));

    float isi_max;
    float isi_mse;
    filter_compute_isi(h,_k,_m,&isi_mse,&isi_max);

    // iterate...
    float isi_mse_min = isi_mse;
    unsigned int p, pmax=1000;
    FILE * fid = fopen(LIQUID_RKAISER_DEBUG_FILENAME,"w");
    fprintf(fid,"clear all;\n");
    for (p=0; p<pmax; p++) {
        // increase band edges
        float df = 0.5f * _beta * p / (float)(_k*pmax);

        // execute filter design
        fir_kaiser_window(n,fc+df,As,_dt,h);
        e2 = 0.0f;
        for (i=0; i<n; i++) e2 += h[i]*h[i];
        for (i=0; i<n; i++) h[i] /= sqrtf(e2*_k);

        // compute inter-symbol interference (MSE, max)
        filter_compute_isi(h,_k,_m,&isi_mse,&isi_max);

        printf("  %4u : isi mse : %20.8e (min: %20.8e)\n", p, isi_mse, isi_mse_min);
        if (isi_mse > isi_mse_min) {
            // search complete
            //break;
        } else {
            isi_mse_min = isi_mse;
            // copy results
            memmove(_h, h, n*sizeof(float));
        }
        fprintf(fid,"fc(%5u) = %20.8e; isi_mse(%5u) = %20.8e;\n", p+1, fc+df, p+1, isi_mse);
    };

    //fprintf(fid,"plot(fc, 10*log10(isi_mse));\n");
    fprintf(fid,"plot(fc, isi_mse);\n");
    fprintf(fid,"xlabel('cutoff frequency');\n");
    fprintf(fid,"ylabel('ISI, MSE [dB]');\n");
    fprintf(fid,"grid on;\n");
    fclose(fid);
    printf("internal debug results written to %s\n", LIQUID_RKAISER_DEBUG_FILENAME);

}

