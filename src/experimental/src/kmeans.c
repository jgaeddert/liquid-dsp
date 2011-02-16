/*
 * Copyright (c) 2007, 2009, 2011 Joseph Gaeddert
 * Copyright (c) 2007, 2009, 2011 Virginia Polytechnic
 *                                Institute & State University
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
// K-means
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.experimental.h"

#define DEBUG_KMEANS 0

//
void kmeans(float complex * _x,
            unsigned int _n,
            float complex * _s,
            unsigned int _k)
{
    //
    float complex s_hat[_k];    // centroid estimate array
    unsigned int  n_hat[_k];    // number of associated samples
    float d, dmin=0;            // (minimum) distance values
    unsigned int kmin=0;        // index of associated centroid

    unsigned int t, i, j;

    // iterate over number of trials
    // TODO: rather than iterating of a set number of trials find stopping criteria
    for (t=0; t<2*_k; t++) {
        // reset centroid estimation arrays
        for (i=0; i<_k; i++) {
            s_hat[i] = 0;
            n_hat[i] = 0;
        }

        // run search over each input
        for (i=0; i<_n; i++) {
            // compute distance from each centroid
            for (j=0; j<_k; j++) {
                d = cabsf(_x[i] - _s[j]);
                if (j==0 || d<dmin) {
                    dmin = d;
                    kmin = j;
                }
            }

            // add sample to associated centroid estimate
            n_hat[kmin]++;
            s_hat[kmin] += _x[i];
        }

        // update centroid estimate
        for (j=0; j<_k; j++) {
            if (n_hat[j] == 0) {
                // TODO: figure out what to do here (no associated samples)
                //_s[j] = 0;
                crandnf(_s+j);
            } else {
                // compute centroid (sample mean)
                _s[j] = s_hat[j] / n_hat[j];
            }
        }
    }
}

