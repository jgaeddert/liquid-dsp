/*
 * Copyright (c) 2011 Joseph Gaeddert
 * Copyright (c) 2011 Virginia Polytechnic Institute & State University
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
// ofdmflexframegen.c
//
// OFDM flexible frame generator
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "liquid.internal.h"

#define DEBUG_OFDMFLEXFRAMEGEN            1

struct ofdmflexframegen_s {
    unsigned int M;         // number of subcarriers
    unsigned int cp_len;    // cyclic prefix length
    unsigned int * p;       // subcarrier allocation (null, pilot, data)

    // constants
    unsigned int M_null;    // number of null subcarriers
    unsigned int M_pilot;   // number of pilot subcarriers
    unsigned int M_data;    // number of data subcarriers
    unsigned int M_S0;      // number of enabled subcarriers in S0
    unsigned int M_S1;      // number of enabled subcarriers in S1

    // buffers
    float complex * X;      // frequency-domain buffer
    float complex * x;      // time-domain buffer

    // internal low-level objects
    ofdmframegen fg;        // frame generator object

    // options
    unsigned int num_symbols_S0;
    unsigned int num_symbols_S1;
    unsigned int num_symbols_header;
    unsigned int num_symbols_payload;
};

// TODO : put these options in 'assemble()' method?
ofdmflexframegen ofdmflexframegen_create(unsigned int _M,
                                         unsigned int _cp_len,
                                         unsigned int * _p)
{
    // validate input
    if (_M < 2) {
        fprintf(stderr,"error: ofdmflexframegen_create(), number of subcarriers must be at least 2\n");
        exit(1);
    } else if (_M % 2) {
        fprintf(stderr,"error: ofdmflexframegen_create(), number of subcarriers must be even\n");
        exit(1);
    }

    ofdmflexframegen q = (ofdmflexframegen) malloc(sizeof(struct ofdmflexframegen_s));
    q->M = _M;
    q->cp_len = _cp_len;

    // allocate memory for transform buffers
    q->X = (float complex*) malloc((q->M)*sizeof(float complex));
    q->x = (float complex*) malloc((q->M)*sizeof(float complex));

    return q;
}

void ofdmflexframegen_destroy(ofdmflexframegen _q)
{
    // free transform array memory
    free(_q->X);
    free(_q->x);

    // free main object memory
    free(_q);
}

void ofdmflexframegen_print(ofdmflexframegen _q)
{
    printf("ofdmflexframegen:\n");
    printf("    num subcarriers     :   %-u\n", _q->M);
    printf("      - NULL            :   %-u\n", _q->M_null);
    printf("      - pilot           :   %-u\n", _q->M_pilot);
    printf("      - data            :   %-u\n", _q->M_data);
    printf("    cyclic prefix len   :   %-u\n", _q->cp_len);
}

void ofdmflexframegen_reset(ofdmflexframegen _q)
{
}

// get length of frame (symbols)
//  _q              :   OFDM frame generator object
//  _header         :   frame header [size?]
//  _payload        :   payload data
//  _payload_len    :   length of payload
//  _opts           :   options (modulation scheme, etc.)
unsigned int ofdmflexframegen_gen_frame_len(ofdmflexframegen _q,
                                            unsigned char * _header,
                                            unsigned char * _payload,
                                            unsigned int    _payload_len,
                                            void * _opts)
{
    // number of S0 symbols
    // number of S1 symbols (1)
    // number of header symbols
    // number of payload symbols
    return 10;
}

// assemble a frame from an array of data
//  _q              :   OFDM frame generator object
//  _header         :   frame header [size?]
//  _payload        :   payload data
//  _payload_len    :   length of payload
//  _opts           :   options (modulation scheme, etc.)
void ofdmflexframegen_assemble(ofdmflexframegen _q,
                               unsigned char * _header,
                               unsigned char * _payload,
                               unsigned int    _payload_len,
                               void * _opts)
{
    // copy header

    // compute number of data symbols...
}

// write symbols of assembled frame
//  _q              :   OFDM frame generator object
//  _buffer         :   output buffer [size: N+cp_len x 1]
//  _num_written    :   number written (either N or N+cp_len)
int ofdmflexframegen_writesymbol(ofdmflexframegen _q,
                                 liquid_float_complex * _buffer,
                                 unsigned int * _num_written)
{
    // write S0 symbols
    // write S1 symbols
    // write header symbols
    // write payload symbols

    return 1;
}

