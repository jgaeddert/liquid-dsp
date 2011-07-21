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

// generate short sequence symbols
//  _p                  :   subcarrier allocation array
//  _num_subcarriers    :   total number of subcarriers
//  _S0                 :   output symbol
//  _M_S0               :   total number of enabled subcarriers in S0
void ofdmoqamframe_init_S0(unsigned char * _p,
                           unsigned int _num_subcarriers,
                           float complex * _S0,
                           unsigned int * _M_S0)
{
    unsigned int i;

    // compute m-sequence length
    unsigned int m = liquid_nextpow2(_num_subcarriers);
    if (m < 4)      m = 4;
    else if (m > 8) m = 8;

    // generate m-sequence generator object
    msequence ms = msequence_create_default(m);

    unsigned int s;
    unsigned int M_S0 = 0;

    // short sequence
    for (i=0; i<_num_subcarriers; i++) {
        // generate symbol
        //s = msequence_generate_symbol(ms,1);
        s = msequence_generate_symbol(ms,3) & 0x01;

        if (_p[i] == OFDMOQAMFRAME_SCTYPE_NULL) {
            // NULL subcarrier
            _S0[i] = 0.0f;
        } else {
            if ( (i%2) == 0 ) {
                // even subcarrer
                _S0[i] = s ? 1.0f : -1.0f;
                M_S0++;
            } else {
                // odd subcarrer (ignore)
                _S0[i] = 0.0f;
            }
        }
    }

    // destroy objects
    msequence_destroy(ms);

    // ensure at least one subcarrier was enabled
    if (M_S0 == 0) {
        fprintf(stderr,"error: ofdmoqamframe_init_S0(), no subcarriers enabled; check allocation\n");
        exit(1);
    }

    // set return value(s)
    *_M_S0 = M_S0;
}


// generate long sequence symbols
//  _p                  :   subcarrier allocation array
//  _num_subcarriers    :   total number of subcarriers
//  _S1                 :   output symbol
//  _M_S1               :   total number of enabled subcarriers in S1
void ofdmoqamframe_init_S1(unsigned char * _p,
                           unsigned int _num_subcarriers,
                           float complex * _S1,
                           unsigned int * _M_S1)
{
    unsigned int i;

    // compute m-sequence length
    unsigned int m = liquid_nextpow2(_num_subcarriers);
    if (m < 4)      m = 4;
    else if (m > 8) m = 8;

    // increase m such that the resulting S1 sequence will
    // differ significantly from S0 with the same subcarrier
    // allocation array
    m++;

    // generate m-sequence generator object
    msequence ms = msequence_create_default(m);

    unsigned int s;
    unsigned int M_S1 = 0;

    // long sequence
    for (i=0; i<_num_subcarriers; i++) {
        // generate symbol
        //s = msequence_generate_symbol(ms,1);
        s = msequence_generate_symbol(ms,3) & 0x01;

        if (_p[i] == OFDMOQAMFRAME_SCTYPE_NULL) {
            // NULL subcarrier
            _S1[i] = 0.0f;
        } else {
            _S1[i] = s ? 1.0f : -1.0f;
            M_S1++;

            // rotate by pi/2 on odd subcarriers
            _S1[i] *= (i%2)==0 ? 1.0f : _Complex_I;
        }
    }

    // destroy objects
    msequence_destroy(ms);

    // ensure at least one subcarrier was enabled
    if (M_S1 == 0) {
        fprintf(stderr,"error: ofdmoqamframe_init_S1(), no subcarriers enabled; check allocation\n");
        exit(1);
    }

    // set return value(s)
    *_M_S1 = M_S1;
}

// initialize default subcarrier allocation
//  _M      :   number of subcarriers
//  _p      :   output subcarrier allocation array, [size: _M x 1]
//
// key: '.' (null), 'P' (pilot), '+' (data)
// .+++++++++++++++P.........P+++++++++++++
//
void ofdmoqamframe_init_default_sctype(unsigned int _M,
                                       unsigned char * _p)
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
void ofdmoqamframe_validate_sctype(unsigned char * _p,
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

