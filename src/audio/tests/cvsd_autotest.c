/*
 * Copyright (c) 2007 - 2014 Joseph Gaeddert
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

#include "autotest/autotest.h"
#include "liquid.internal.h"

//
// AUTOTEST: check RMSE for CVSD
//
void autotest_cvsd_rmse_sine() {
    unsigned int n=256;
    unsigned int nbits=3;
    float zeta=1.5f;
    float alpha=0.90f;

    // create cvsd codecs
    cvsd cvsd_encoder = cvsd_create(nbits,zeta,alpha);
    cvsd cvsd_decoder = cvsd_create(nbits,zeta,alpha);

    float phi=0.0f;
    float dphi=0.1f;
    unsigned int i;
    unsigned char b;
    float x,y;
    float rmse=0.0f;
    for (i=0; i<n; i++) {
        x = 0.5f*sinf(phi);
        b = cvsd_encode(cvsd_encoder, x); 
        y = cvsd_decode(cvsd_decoder, b); 

        rmse += (x-y)*(x-y);
        phi += dphi;
    }   

    rmse = 10*log10f(rmse/n);
    if (liquid_autotest_verbose)
        printf("cvsd rmse : %8.2f dB\n", rmse);
    CONTEND_LESS_THAN(rmse, -20.0f);

    // destroy cvsd codecs
    cvsd_destroy(cvsd_encoder);
    cvsd_destroy(cvsd_decoder);
}

