/*
 * Copyright (c) 2010, 2011 Joseph Gaeddert
 * Copyright (c) 2010, 2011 Virginia Polytechnic
 *                          Institute & State University
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
// ofdmframegen.c
//
// OFDM frame generator
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "liquid.internal.h"

#if HAVE_FFTW3_H
#   include <fftw3.h>
#endif

#define DEBUG_OFDMFRAMEGEN            1

struct ofdmframegen_s {
    unsigned int M;         // number of subcarriers
    unsigned int cp_len;    // cyclic prefix length
    unsigned char * p;      // subcarrier allocation (null, pilot, data)

    // constants
    unsigned int M_null;    // number of null subcarriers
    unsigned int M_pilot;   // number of pilot subcarriers
    unsigned int M_data;    // number of data subcarriers
    unsigned int M_S0;      // number of enabled subcarriers in S0
    unsigned int M_S1;      // number of enabled subcarriers in S1

    // scaling factors
    float g_data;           //

    // transform object
    FFT_PLAN ifft;          // ifft object
    float complex * X;      // frequency-domain buffer
    float complex * x;      // time-domain buffer

    // PLCP short
    float complex * S0;     // short sequence (frequency)
    float complex * s0;     // short sequence (time)

    // PLCP long
    float complex * S1;     // long sequence (frequency)
    float complex * s1;     // long sequence (time)

    // pilot sequence
    msequence ms_pilot;
};

ofdmframegen ofdmframegen_create(unsigned int _M,
                                 unsigned int _cp_len,
                                 unsigned char * _p)
{
    // validate input
    if (_M < 2) {
        fprintf(stderr,"error: ofdmframegen_create(), number of subcarriers must be at least 2\n");
        exit(1);
    } else if (_M % 2) {
        fprintf(stderr,"error: ofdmframegen_create(), number of subcarriers must be even\n");
        exit(1);
    }

    ofdmframegen q = (ofdmframegen) malloc(sizeof(struct ofdmframegen_s));
    q->M = _M;
    q->cp_len = _cp_len;

    // allocate memory for subcarrier allocation IDs
    q->p = (unsigned char*) malloc((q->M)*sizeof(unsigned char));
    if (_p == NULL) {
        // initialize default subcarrier allocation
        ofdmframe_init_default_sctype(q->M, q->p);
    } else {
        // copy user-defined subcarrier allocation
        memmove(q->p, _p, q->M*sizeof(unsigned char));
    }

    // validate and count subcarrier allocation
    ofdmframe_validate_sctype(q->p, q->M, &q->M_null, &q->M_pilot, &q->M_data);
    if ( (q->M_pilot + q->M_data) == 0) {
        fprintf(stderr,"error: ofdmframegen_create(), must have at least one enabled subcarrier\n");
        exit(1);
    } else if (q->M_data == 0) {
        fprintf(stderr,"error: ofdmframegen_create(), must have at least one data subcarriers\n");
        exit(1);
    } else if (q->M_pilot < 2) {
        fprintf(stderr,"error: ofdmframegen_create(), must have at least two pilot subcarriers\n");
        exit(1);
    }

    // allocate memory for transform objects
    q->X = (float complex*) malloc((q->M)*sizeof(float complex));
    q->x = (float complex*) malloc((q->M)*sizeof(float complex));
    q->ifft = FFT_CREATE_PLAN(q->M, q->X, q->x, FFT_DIR_BACKWARD, FFT_METHOD);

    // allocate memory for PLCP arrays
    q->S0 = (float complex*) malloc((q->M)*sizeof(float complex));
    q->s0 = (float complex*) malloc((q->M)*sizeof(float complex));
    q->S1 = (float complex*) malloc((q->M)*sizeof(float complex));
    q->s1 = (float complex*) malloc((q->M)*sizeof(float complex));
    ofdmframe_init_S0(q->p, q->M, q->S0, q->s0, &q->M_S0);
    ofdmframe_init_S1(q->p, q->M, q->S1, q->s1, &q->M_S1);

    // compute scaling factor
    q->g_data = 1.0f / sqrtf(q->M_pilot + q->M_data);

    // set pilot sequence
    q->ms_pilot = msequence_create_default(8);

    return q;
}

void ofdmframegen_destroy(ofdmframegen _q)
{
    // free transform object memory
    //

    // free subcarrier type array memory
    free(_q->p);

    // free transform array memory
    free(_q->X);
    free(_q->x);
    FFT_DESTROY_PLAN(_q->ifft);

    // free PLCP memory arrays
    free(_q->S0);
    free(_q->s0);
    free(_q->S1);
    free(_q->s1);

    // free pilot msequence object memory
    msequence_destroy(_q->ms_pilot);

    // free main object memory
    free(_q);
}

void ofdmframegen_print(ofdmframegen _q)
{
    printf("ofdmframegen:\n");
    printf("    num subcarriers     :   %-u\n", _q->M);
    printf("      - NULL            :   %-u\n", _q->M_null);
    printf("      - pilot           :   %-u\n", _q->M_pilot);
    printf("      - data            :   %-u\n", _q->M_data);
    printf("    cyclic prefix len   :   %-u\n", _q->cp_len);
    printf("    ");
    ofdmframe_print_sctype(_q->p, _q->M);
}

void ofdmframegen_reset(ofdmframegen _q)
{
    msequence_reset(_q->ms_pilot);
}

void ofdmframegen_write_S0(ofdmframegen _q,
                           float complex * _y)
{
    memmove(_y, _q->s0, (_q->M)*sizeof(float complex));
}


void ofdmframegen_write_S1(ofdmframegen _q,
                           float complex * _y)
{
    memmove(_y, _q->s1, (_q->M)*sizeof(float complex));
}


// write OFDM symbol
//  _q      :   framging generator object
//  _x      :   input symbols, [size: _M x 1]
//  _y      :   output samples, [size: _M x 1]
void ofdmframegen_writesymbol(ofdmframegen _q,
                              float complex * _x,
                              float complex * _y)
{
    // move frequency data to internal buffer
    unsigned int i;
    unsigned int k;
    int sctype;
    for (i=0; i<_q->M; i++) {
        // start at mid-point (effective fftshift)
        k = (i + _q->M/2) % _q->M;

        sctype = _q->p[k];
        if (sctype==OFDMFRAME_SCTYPE_NULL) {
            // disabled subcarrier
            _q->X[k] = 0.0f;
        } else if (sctype==OFDMFRAME_SCTYPE_PILOT) {
            // pilot subcarrier
            _q->X[k] = (msequence_advance(_q->ms_pilot) ? 1.0f : -1.0f) * _q->g_data;
        } else {
            // data subcarrier
            _q->X[k] = _x[k] * _q->g_data;
        }

        //printf("X[%3u] = %12.8f + j*%12.8f;\n",i+1,crealf(_q->X[i]),cimagf(_q->X[i]));
    }

    // execute transform
    FFT_EXECUTE(_q->ifft);

    // copy result to output
    memmove( _y, &_q->x[_q->M - _q->cp_len], (_q->cp_len)*sizeof(float complex));
    memmove(&_y[_q->cp_len], _q->x, (_q->M)*sizeof(float complex));
}


