/*
 * Copyright (c) 2007 - 2015 Joseph Gaeddert
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
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

