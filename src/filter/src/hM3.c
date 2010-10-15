/*
 * Copyright (c) 2010 Joseph Gaeddert
 * Copyright (c) 2010 Virginia Polytechnic Institute & State University
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
//  [harris:2005] f. harris, C. Dick, S. Seshagiri, K. Moerder, "An
//      Improved Square-Root Nyquist Shaping Filter," Proceedings of
//      the Software-Defined Radio Forum, 2005
//

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "liquid.internal.h"

// design_hM3_filter()
//
// Design root-Nyquist harris-Moerder filter using Parks-McClellan
// algorithm
//
//  _k      :   filter over-sampling rate (samples/symbol)
//  _m      :   filter delay (symbols)
//  _beta   :   filter excess bandwidth factor (0,1)
//  _dt     :   filter fractional sample delay
//  _h      :   resulting filter [size: 2*_k*_m+1]
void design_hM3_filter(unsigned int _k,
                       unsigned int _m,
                       float _beta,
                       float _dt,
                       float * _h)
{
    if ( _k < 2 ) {
        printf("error: design_hM3_filter(): k must be greater than 1\n");
        exit(0);
    } else if ( _m < 1 ) {
        printf("error: design_hM3_filter(): m must be greater than 0\n");
        exit(0);
    } else if ( (_beta < 0.0f) || (_beta > 1.0f) ) {
        printf("error: design_hM3_filter(): beta must be in [0,1]\n");
        exit(0);
    } else;

    unsigned int n=2*_k*_m+1;       // filter length

    //
    float fc = 1.0 / (float)(2*_k); // filter cutoff
    float fp = fc*(1.0 - _beta);    // pass-band
    float fs = fc*(1.0 + _beta);    // stop-band

    // root nyquist
    unsigned int num_bands = 3;
    float bands[6]   = {0.0f, fp, fc, fc, fs, 0.5f};
    float des[3]     = {1.0f, 1.0f/sqrtf(2.0f), 0.0f};
    float weights[3] = {1.0f, 1.0f, 1.0f};

    liquid_firdespm_btype btype = LIQUID_FIRDESPM_BANDPASS;
    liquid_firdespm_wtype wtype[3] = {LIQUID_FIRDESPM_FLATWEIGHT,
                                      LIQUID_FIRDESPM_FLATWEIGHT,
                                      LIQUID_FIRDESPM_EXPWEIGHT};

    //unsigned int i;
    float h[n];
    firdespm_run(n,bands,des,weights,num_bands,btype,wtype,h);
    // copy results
    memmove(_h, h, n*sizeof(float));

    float isi_max;
    float isi_mse;
    liquid_filter_isi(h,_k,_m,&isi_mse,&isi_max);

    // iterate...
    float isi_mse_min = isi_mse;
    unsigned int p, pmax=100;
    for (p=0; p<pmax; p++) {
        // increase pass-band edge
        fp = fc*(1.0 - _beta * p / (float)(pmax) );
        bands[1] = fp;

        // execute filter design
        firdespm_run(n,bands,des,weights,num_bands,btype,wtype,h);

        // compute inter-symbol interference (MSE, max)
        liquid_filter_isi(h,_k,_m,&isi_mse,&isi_max);

        printf("  isi mse : %20.8e (min: %20.8e)\n", isi_mse, isi_mse_min);
        if (isi_mse > isi_mse_min) {
            // search complete
            break;
        } else {
            isi_mse_min = isi_mse;
            // copy results
            memmove(_h, h, n*sizeof(float));
        }
    };

    // normalize
    float e2 = 0.0f;
    unsigned int i;
    for (i=0; i<n; i++) e2 += _h[i]*_h[i];
    for (i=0; i<n; i++) _h[i] *= sqrtf(_k/e2);
}

