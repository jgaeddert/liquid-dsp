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
// ofdmoqamframe data, methods common to both generator/synchronizer
// objects
//  - physical layer convergence procedure (PLCP)
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.internal.h"

void ofdmoqamframe_init_S0(unsigned int * _p,
                           unsigned int _num_subcarriers,
                           float complex * _S0)
{
    msequence ms = msequence_create(4);
    modem mod = modem_create(MOD_QPSK,2);
    unsigned int s;
    float complex sym;
    float zeta = 1.0f;
    unsigned int j;
    unsigned int sctype;

    // short sequence
    for (j=0; j<_num_subcarriers; j++) {
        sctype = _p[j];
        if (sctype == OFDMOQAMFRAME_SCTYPE_NULL) {
            // NULL subcarrier
            _S0[j] = 0.0f;
        } else {
            if ((j%4) == 2) {
                // even subcarrer, skipping ever other (14 total)
                s = msequence_generate_symbol(ms,2);
                modem_modulate(mod,s,&sym);
                // retain only quadrature component (time aligned
                // without half-symbol delay), and amplitude-
                // compensated.
                _S0[j] = cimagf(sym) * _Complex_I * zeta * 2.0f * sqrtf(2.0f);
            } else {
                // odd subcarrer
                _S0[j] = 0.0f;
            }
        }
    }
    msequence_destroy(ms);
    modem_destroy(mod);
}

void ofdmoqamframe_init_S1(unsigned int * _p,
                           unsigned int _num_subcarriers,
                           float complex * _S1)
{
    msequence ms = msequence_create(5);
    modem mod = modem_create(MOD_QPSK,2);
    unsigned int s;
    float complex sym;
    float zeta = 1.0f;
    unsigned int j;
    unsigned int sctype;

    // long sequence
    for (j=0; j<_num_subcarriers; j++) {
        sctype = _p[j];
        if (sctype == OFDMOQAMFRAME_SCTYPE_NULL) {
            // NULL subcarrier
            _S1[j] = 0.0f;
        } else {
            s = msequence_generate_symbol(ms,2);
            modem_modulate(mod,s,&sym);
            if ((j%2) == 0) {
                // even subcarrer
                _S1[j] = zeta * sqrtf(2.0f) * crealf(sym);
            } else {
                // odd subcarrer
                _S1[j] = zeta * sqrtf(2.0f) * cimagf(sym) * _Complex_I;
            }
        }
    }
    msequence_destroy(ms);
    modem_destroy(mod);
}

// initialize default subcarrier allocation
//  _M      :   number of subcarriers
//  _p      :   output subcarrier allocation array, [size: _M x 1]
//
// key: '.' (null), 'P' (pilot), '+' (data)
// .+++++++++++++++P.........P+++++++++++++
//
void ofdmoqamframe_init_default_sctype(unsigned int _M,
                                       unsigned int * _p)
{
    // validate input
    if (_M < 6) {
        fprintf(stderr,"warning: ofdmoqamframe_init_default_sctype(), less than 4 subcarriers\n");
    }

    unsigned int i;

    // compute guard band
    unsigned int g = _M / 10;
    if (g < 2) g = 2;

    // compute upper|lower band-edge
    unsigned int i0 = (_M/2) - g;
    unsigned int i1 = (_M/2) + g;

    for (i=0; i<_M; i++) {

        // TODO : allocate more pilot subcarriers (10%?)
        if (i==0 || (i>i0 && i<i1))
            _p[i] = OFDMOQAMFRAME_SCTYPE_NULL;
        else if (i==i0 || i==i1)
            _p[i] = OFDMOQAMFRAME_SCTYPE_PILOT;
        else
            _p[i] = OFDMOQAMFRAME_SCTYPE_DATA;
    }
}

// validate subcarrier type (count number of null, pilot, and data
// subcarriers in the allocation)
//  _p          :   subcarrier allocation array, [size: _M x 1]
//  _M          :   number of subcarriers
//  _M_null     :   output number of null subcarriers
//  _M_pilot    :   output number of pilot subcarriers
//  _M_data     :   output number of data subcarriers
void ofdmoqamframe_validate_sctype(unsigned int * _p,
                                   unsigned int _M,
                                   unsigned int * _M_null,
                                   unsigned int * _M_pilot,
                                   unsigned int * _M_data)
{
    // clear counters
    unsigned int M_null  = 0;
    unsigned int M_pilot = 0;
    unsigned int M_data  = 0;

    unsigned int i;
    for (i=0; i<_M; i++) {
        // update appropriate counters
        if (_p[i] == OFDMOQAMFRAME_SCTYPE_NULL)
            M_null++;
        else if (_p[i] == OFDMOQAMFRAME_SCTYPE_PILOT)
            M_pilot++;
        else if (_p[i] == OFDMOQAMFRAME_SCTYPE_DATA)
            M_data++;
        else {
            fprintf(stderr,"error: ofdmoqamframe_validate_sctype(), invalid subcarrier type (%u)\n", _p[i]);
            exit(1);
        }
    }

    // set outputs
    *_M_null  = M_null;
    *_M_pilot = M_pilot;
    *_M_data  = M_data;
}

