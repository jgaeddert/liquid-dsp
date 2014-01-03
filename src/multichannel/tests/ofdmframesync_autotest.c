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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <complex.h>
#include <math.h>
#include <assert.h>

#include "autotest/autotest.h"
#include "liquid.h"


// internal callback
//  _X          :   subcarrier symbols
//  _p          :   subcarrier allocation
//  _M          :   number of subcarriers
//  _userdata   :   user-defined data structure
int ofdmframesync_autotest_callback(float complex * _X,
                                    unsigned char * _p,
                                    unsigned int    _M,
                                    void * _userdata)
{
    printf("******** callback invoked!\n");

    // type cast _userdata as complex float array
    float complex * X = (float complex *)_userdata;

    // copy values and return
    memmove(X, _X, _M*sizeof(float complex));

    // return
    return 0;
}


// Helper function to keep code base small
//  _num_subcarriers    :   number of subcarriers
//  _cp_len             :   cyclic prefix lenght
//  _taper_len          :   taper length
void ofdmframesync_acquire_test(unsigned int _num_subcarriers,
                                unsigned int _cp_len,
                                unsigned int _taper_len)
{
    // options
    unsigned int M         = _num_subcarriers;  // number of subcarriers
    unsigned int cp_len    = _cp_len;           // cyclic prefix lenght
    unsigned int taper_len = _taper_len;        // taper length
    float tol              = 1e-2f;             // error tolerance

    //
    float dphi = 1.0f / (float)M;       // carrier frequency offset

    // subcarrier allocation (initialize to default)
    unsigned char p[M];
    ofdmframe_init_default_sctype(M, p);

    // derived values
    unsigned int num_samples = (3 + 1)*(M + cp_len);

    // create synthesizer/analyzer objects
    ofdmframegen fg = ofdmframegen_create(M, cp_len, taper_len, p);
    //ofdmframegen_print(fg);

    float complex X[M];         // original data sequence
    float complex X_test[M];    // recovered data sequence
    ofdmframesync fs = ofdmframesync_create(M,cp_len,taper_len,p,ofdmframesync_autotest_callback,(void*)X_test);

    unsigned int i;
    float complex y[num_samples];   // frame samples

    // assemble full frame
    unsigned int n=0;

    // write first S0 symbol
    ofdmframegen_write_S0a(fg, &y[n]);
    n += M + cp_len;

    // write second S0 symbol
    ofdmframegen_write_S0b(fg, &y[n]);
    n += M + cp_len;

    // write S1 symbol
    ofdmframegen_write_S1( fg, &y[n]);
    n += M + cp_len;

    // generate data symbol (random)
    for (i=0; i<M; i++) {
        X[i]      = cexpf(_Complex_I*2*M_PI*randf());
        X_test[i] = 0.0f;
    }

    // write data symbol
    ofdmframegen_writesymbol(fg, X, &y[n]);
    n += M + cp_len;

    // validate frame length
    assert(n == num_samples);

    // add carrier offset
    for (i=0; i<num_samples; i++)
        y[i] *= cexpf(_Complex_I*dphi*i);

    // run receiver
    ofdmframesync_execute(fs,y,num_samples);

    // check output
    for (i=0; i<M; i++) {
        if (p[i] == OFDMFRAME_SCTYPE_DATA) {
            float e = crealf( (X[i] - X_test[i])*conjf(X[i] - X_test[i]) );
            CONTEND_DELTA( cabsf(e), 0.0f, tol );
        }
    }

    // destroy objects
    ofdmframegen_destroy(fg);
    ofdmframesync_destroy(fs);
}

//
void autotest_ofdmframesync_acquire_n64()   { ofdmframesync_acquire_test(64,  8,  0); }
void autotest_ofdmframesync_acquire_n128()  { ofdmframesync_acquire_test(128, 16, 0); }
void autotest_ofdmframesync_acquire_n256()  { ofdmframesync_acquire_test(256, 32, 0); }
void autotest_ofdmframesync_acquire_n512()  { ofdmframesync_acquire_test(512, 64, 0); }

