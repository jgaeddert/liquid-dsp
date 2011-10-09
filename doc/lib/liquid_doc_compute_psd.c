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
//
//

#include <stdlib.h>
#include <stdio.h>
#include "liquid.doc.h"

void liquid_doc_compute_psdcf(float complex * _x,
                              unsigned int _n,
                              float complex * _X,
                              unsigned int _nfft,
                              liquid_doc_psdwindow _wtype,
                              int _normalize)
{
    unsigned int i;

    // compute window and norm
    float w[_n];
    float wnorm=0.0f;
    for (i=0; i<_n; i++) {
        switch (_wtype) {
        case LIQUID_DOC_PSDWINDOW_NONE:     w[i] = 1.0f;            break;
        case LIQUID_DOC_PSDWINDOW_HANN:     w[i] = hann(i,_n);      break;
        case LIQUID_DOC_PSDWINDOW_HAMMING:  w[i] = hamming(i,_n);   break;
            break;
        default:
            fprintf(stderr,"error: liquid_doc_compute_psd(), invalid window type\n");
            exit(1);
        }
        wnorm += w[i];
    }
    wnorm /= (float)(_n);

    float complex x[_nfft];
    fftplan fft = fft_create_plan(_nfft,x,_X,FFT_FORWARD,0);
    for (i=0; i<_nfft; i++) {
        x[i] = i < _n ? _x[i] * w[i] / wnorm : 0.0f;

    }
    fft_execute(fft);
    fft_destroy_plan(fft);

    // normalize spectrum by maximum
    if (_normalize) {
        float X_max = 0.0f;
        for (i=0; i<_nfft; i++)
            X_max = cabsf(_X[i]) > X_max ? cabsf(_X[i]) : X_max;

        for (i=0; i<_nfft; i++)
            _X[i] /= X_max;
    }
}

void liquid_doc_compute_psdf(float * _x,
                             unsigned int _n,
                             float complex * _X,
                             unsigned int _nfft,
                             liquid_doc_psdwindow _wtype,
                             int _normalize)
{
    float complex xcf[_n];
    unsigned int i;
    for (i=0; i<_n; i++)
        xcf[i] = _x[i];

    liquid_doc_compute_psdcf(xcf,_n,_X,_nfft,_wtype,_normalize);
}

