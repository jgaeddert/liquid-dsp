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
#include "liquid.h"

void autotest_firpfb_impulse_response()
{
    // Initialize variables
    float tol=1e-4f;

    // k=2, m=3, beta=0.3, npfb=4;
    // h=rrcos(k*npfb,m,beta);
    float h[48] = {
     -0.033116,  -0.024181,  -0.006284,   0.018261, 
      0.045016,   0.068033,   0.080919,   0.078177, 
      0.056597,   0.016403,  -0.038106,  -0.098610, 
     -0.153600,  -0.189940,  -0.194900,  -0.158390, 
     -0.075002,   0.054511,   0.222690,   0.415800, 
      0.615340,   0.800390,   0.950380,   1.048100, 
      1.082000,   1.048100,   0.950380,   0.800390, 
      0.615340,   0.415800,   0.222690,   0.054511, 
     -0.075002,  -0.158390,  -0.194900,  -0.189940, 
     -0.153600,  -0.098610,  -0.038106,   0.016403, 
      0.056597,   0.078177,   0.080919,   0.068033, 
      0.045016,   0.018261,  -0.006284,  -0.024181
    };

    // filter input
    float noise[12] = {
      0.438310,   1.001900,   0.200600,   0.790040, 
      1.134200,   1.592200,  -0.702980,  -0.937560, 
     -0.511270,  -1.684700,   0.328940,  -0.387780
    };

    // expected filter outputs
    float test[4] = {
        2.05558467194397f,
        1.56922189602661f,
        0.998479744645138,
        0.386125857849177
    };

    // Load filter coefficients externally
    firpfb_rrrf f = firpfb_rrrf_create(4, h, 48);
    
    unsigned int i;
    for (i=0; i<12; i++)
        firpfb_rrrf_push(f,noise[i]);

    float y;
    for (i=0; i<4; i++) {
        firpfb_rrrf_execute(f,i,&y);
        CONTEND_DELTA(test[i],y,tol);
    }
    
    firpfb_rrrf_destroy(f);
}

