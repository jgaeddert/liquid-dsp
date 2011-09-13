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
// ofdmoqamframegen.c
//
// OFDM/OQAM frame generator
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

#define DEBUG_OFDMOQAMFRAMEGEN            1

struct ofdmoqamframegen_s {
    unsigned int M;         // number of subcarriers
    unsigned int m;         // filter delay (symbols)
    float beta;             // filter excess bandwidth factor
    unsigned char * p;      // subcarrier allocation (null, pilot, data)

    // constants
    unsigned int M_null;    // number of null subcarriers
    unsigned int M_pilot;   // number of pilot subcarriers
    unsigned int M_data;    // number of data subcarriers
    unsigned int M_S0;      // number of enabled subcarriers in S0
    unsigned int M_S1;      // number of enabled subcarriers in S1

    // scaling factors
    float g_data;           //
    float g_S0;             //
    float g_S1;             //

    // filterbank objects
    ofdmoqam synthesizer;

    // transform objects
    float complex * X;
    float complex * x;

    // PLCP
    float complex * S0;     // short sequence
    float complex * S1;     // long sequence

    // pilot sequence
    msequence ms_pilot;
};

ofdmoqamframegen ofdmoqamframegen_create(unsigned int _M,
                                         unsigned int _m,
                                         float _beta,
                                         unsigned char * _p)
{
    // validate input
    if (_M < 2) {
        fprintf(stderr,"error: ofdmoqamframesync_create(), number of subcarriers must be at least 2\n");
        exit(1);
    } if (_M % 2) {
        fprintf(stderr,"error: ofdmoqamframesync_create(), number of subcarriers must be even\n");
        exit(1);
    } if (_m < 1) {
        fprintf(stderr,"error: ofdmoqamframesync_create(), filter delay must be > 0\n");
        exit(1);
    } else if (_beta < 0.0f) {
        fprintf(stderr,"error: ofdmoqamframesync_create(), filter excess bandwidth must be > 0\n");
        exit(1);
    }

    ofdmoqamframegen q = (ofdmoqamframegen) malloc(sizeof(struct ofdmoqamframegen_s));
    q->M = _M;
    q->m = _m;
    q->beta = _beta;

    // create synthsizer
    q->synthesizer = ofdmoqam_create(q->M,
                                     q->m,
                                     q->beta,
                                     0.0f,  // dt
                                     LIQUID_SYNTHESIZER,
                                     0);    // gradient

    // allocate memory for subcarrier allocation IDs
    q->p = (unsigned char*) malloc((q->M)*sizeof(unsigned char));
    if (_p == NULL) {
        // initialize default subcarrier allocation
        ofdmoqamframe_init_default_sctype(q->M, q->p);
    } else {
        // copy user-defined subcarrier allocation
        memmove(q->p, _p, q->M*sizeof(unsigned char));
    }

    // validate and count subcarrier allocation
    ofdmoqamframe_validate_sctype(q->p, q->M, &q->M_null, &q->M_pilot, &q->M_data);
    if ( (q->M_pilot + q->M_data) == 0) {
        fprintf(stderr,"error: ofdmoqamframegen_create(), must have at least one enabled subcarrier\n");
        exit(1);
    }

    // allocate memory for transform objects
    q->X  = (float complex*) malloc((q->M)*sizeof(float complex));
    q->x  = (float complex*) malloc((q->M)*sizeof(float complex));

    // allocate memory for PLCP arrays
    q->S0 = (float complex*) malloc((q->M)*sizeof(float complex));
    q->S1 = (float complex*) malloc((q->M)*sizeof(float complex));
    ofdmoqamframe_init_S0(q->p, q->M, q->S0, &q->M_S0);
    ofdmoqamframe_init_S1(q->p, q->M, q->S1, &q->M_S1);

    // compute scaling factors
    //q->g_data = q->M / sqrtf(q->M_pilot + q->M_data);
    q->g_data = sqrtf(q->M) / sqrtf(q->M_pilot + q->M_data);
    q->g_S0   = sqrtf(q->M) / sqrtf(q->M_S0);
    q->g_S1   = sqrtf(q->M) / sqrtf(q->M_S1);

    // set pilot sequence
    q->ms_pilot = msequence_create_default(8);

    return q;
}

void ofdmoqamframegen_destroy(ofdmoqamframegen _q)
{
    // free synthesizer object memory
    ofdmoqam_destroy(_q->synthesizer);

    // free subcarrier type array memory
    free(_q->p);

    // free transform array memory
    free(_q->X);
    free(_q->x);

    // free PLCP memory arrays
    free(_q->S0);
    free(_q->S1);

    // free pilot msequence object memory
    msequence_destroy(_q->ms_pilot);

    // free main object memory
    free(_q);
}

void ofdmoqamframegen_print(ofdmoqamframegen _q)
{
    printf("ofdmoqamframegen:\n");
    printf("    num subcarriers     :   %-u\n", _q->M);
    printf("      - NULL            :   %-u\n", _q->M_null);
    printf("      - pilot           :   %-u\n", _q->M_pilot);
    printf("      - data            :   %-u\n", _q->M_data);
    printf("    m (filter delay)    :   %-u\n", _q->m);
    printf("    beta (excess b/w)   :   %8.6f\n", _q->beta);
}

void ofdmoqamframegen_reset(ofdmoqamframegen _q)
{
}

void ofdmoqamframegen_writeshortsequence(ofdmoqamframegen _q,
                                         float complex * _y)
{
    // move short sequence to freq-domain buffer
    memmove(_q->X, _q->S0, (_q->M)*sizeof(float complex));

    // apply gain
    unsigned int i;
    for (i=0; i<_q->M; i++)
        _q->X[i] *= _q->g_S0;

    // execute synthesizer, store result in output array
    ofdmoqam_execute(_q->synthesizer, _q->X, _y);
}


void ofdmoqamframegen_writelongsequence(ofdmoqamframegen _q,
                                        float complex * _y)
{
    // move long sequence to freq-domain buffer
    memmove(_q->X, _q->S1, (_q->M)*sizeof(float complex));

    // apply gain
    unsigned int i;
    for (i=0; i<_q->M; i++)
        _q->X[i] *= _q->g_S1;

    // execute synthesizer, store result in output array
    ofdmoqam_execute(_q->synthesizer, _q->X, _y);
}


void ofdmoqamframegen_writeheader(ofdmoqamframegen _q,
                                  float complex * _y)
{
}

// write OFDM/OQAM symbol
//  _q      :   framging generator object
//  _x      :   input symbols, [size: _M x 1]
//  _y      :   output samples, [size: _M x 1]
void ofdmoqamframegen_writesymbol(ofdmoqamframegen _q,
                                  float complex * _x,
                                  float complex * _y)
{
    unsigned int pilot_phase = msequence_advance(_q->ms_pilot);

    // move frequency data to internal buffer
    unsigned int i;
    int sctype;
    for (i=0; i<_q->M; i++) {
        sctype = _q->p[i];
        if (sctype==OFDMOQAMFRAME_SCTYPE_NULL) {
            // disabled subcarrier
            _q->X[i] = 0.0f;
        } else if (sctype==OFDMOQAMFRAME_SCTYPE_PILOT) {
            // pilot subcarrier
            _q->X[i] = (pilot_phase ? 1.0f : -1.0f) * _q->g_data;
        } else {
            // data subcarrier
            _q->X[i] = _x[i] * _q->g_data;
        }

        //printf("X[%3u] = %12.8f + j*%12.8f;\n",i+1,crealf(_q->X[i]),cimagf(_q->X[i]));
    }

    // execute synthesizer, store result in output array
    ofdmoqam_execute(_q->synthesizer, _q->X, _y);
}

void ofdmoqamframegen_flush(ofdmoqamframegen _q,
                            float complex * _y)
{
    // fill freq-domain buffer with zeros
    unsigned int j;
    for (j=0; j<_q->M; j++)
        _q->X[j] = 0.0f;

    // execute synthesizer, store result in output array
    ofdmoqam_execute(_q->synthesizer, _q->X, _y);
}

