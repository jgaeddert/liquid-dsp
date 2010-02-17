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
// freqz : compute spectral response
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "liquid.doc.h"

// Compute spectral response
void liquid_doc_freqz(float * _b,
                      unsigned int _nb,
                      float * _a,
                      unsigned int _na,
                      unsigned int _nfft,
                      float complex * _H)
{
    unsigned int i;

    float complex x[_nfft];
    float complex B[_nfft];
    float complex A[_nfft];
    float complex X[_nfft];
    fftplan fft = fft_create_plan(_nfft,x,X,FFT_FORWARD,0);

    // numerator
    for (i=0; i<_nfft; i++)
        x[i] = i < _nb ? _b[i] : 0.0f;
    fft_execute(fft);
    memmove(B,X,_nfft*sizeof(float complex));

    // denominator
    for (i=0; i<_nfft; i++)
        x[i] = i < _na ? _a[i] : 0.0f;
    fft_execute(fft);
    memmove(A,X,_nfft*sizeof(float complex));

    fft_destroy_plan(fft);

    for (i=0; i<_nfft; i++)
        X[i] = B[i] / A[i];

    memmove(_H, X, _nfft*sizeof(float complex));
}

