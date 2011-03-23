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

#ifndef __LIQUID_DOC_H__
#define __LIQUID_DOC_H__

#include <stdio.h>
#include <complex.h>
#include <stdbool.h>
#include "liquid.h"

// default gnuplot colors
#define LIQUID_DOC_COLOR_GRID   "#cccccc"   // very light gray
#define LIQUID_DOC_COLOR_GRAY   "#999999"   // light gray
#define LIQUID_DOC_COLOR_BLUE   "#004080"   // blue
#define LIQUID_DOC_COLOR_GREEN  "#008040"   // green
#define LIQUID_DOC_COLOR_RED    "#800000"   // dark red
#define LIQUID_DOC_COLOR_PURPLE "#400040"   // dark puple

// power spectral density window
typedef enum {
    LIQUID_DOC_PSDWINDOW_NONE=0,
    LIQUID_DOC_PSDWINDOW_HANN,
    LIQUID_DOC_PSDWINDOW_HAMMING
} liquid_doc_psdwindow;

// compute power spectral density (complex float input)
void liquid_doc_compute_psdcf(float complex * _x,
                              unsigned int _n,
                              float complex * _X,
                              unsigned int _nfft,
                              liquid_doc_psdwindow _wtype,
                              bool _normalize);

// compute power spectral density (float input)
void liquid_doc_compute_psdf(float * _x,
                             unsigned int _n,
                             float complex * _X,
                             unsigned int _nfft,
                             liquid_doc_psdwindow _wtype,
                             bool _normalize);


// Compute spectral response
void liquid_doc_freqz(float * _b,
                      unsigned int _nb,
                      float * _a,
                      unsigned int _na,
                      unsigned int _nfft,
                      float complex * _H);

#endif // __LIQUID_DOC_H__

