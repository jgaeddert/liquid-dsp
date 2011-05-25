/*
 * Copyright (c) 2007, 2009 Joseph Gaeddert
 * Copyright (c) 2007, 2009 Virginia Polytechnic Institute & State University
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
//
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "liquid.experimental.h"
#include "liquid.internal.h"

#define DEBUG_OFDMFRAME64GEN                1

struct ofdmframe64gen_s {
    unsigned int num_subcarriers;
    unsigned int cp_len;

    float complex * x;      // time-domain buffer
    float complex * X;      // freq-domain buffer

    // non-allocated pointers
    float complex * xcp;    // cyclic prefix pointer
    float complex * St;     // short PLCP array pointer (time-domain)
    float complex * Lt;     // long PLCP array pointer (time-domain)

    msequence ms_pilot;

    FFT_PLAN fft;

    float zeta;             // fft scaling factor
};

ofdmframe64gen ofdmframe64gen_create()
{
    ofdmframe64gen q = (ofdmframe64gen) malloc(sizeof(struct ofdmframe64gen_s));
    q->num_subcarriers = 64;
    q->cp_len = 16;

    // allocate memory for buffers
    q->x = (float complex*) malloc((q->num_subcarriers)*sizeof(float complex));
    q->X = (float complex*) malloc((q->num_subcarriers)*sizeof(float complex));

    q->fft = FFT_CREATE_PLAN(q->num_subcarriers, q->X, q->x, FFT_DIR_BACKWARD, FFT_METHOD);
    q->zeta = sqrtf(1.0f / 52.0f); // sqrt((64/52)*(1/64)) : 52 subcarriers used

    // set cyclic prefix array pointer
    q->xcp = &(q->x[q->num_subcarriers - q->cp_len]);
    q->St = (float complex*) ofdmframe64_plcp_St;
    q->Lt = (float complex*) ofdmframe64_plcp_Lt;

    // set pilot sequence
    q->ms_pilot = msequence_create_default(8);

    return q;
}

void ofdmframe64gen_destroy(ofdmframe64gen _q)
{
    free(_q->x);
    free(_q->X);
    msequence_destroy(_q->ms_pilot);

    FFT_DESTROY_PLAN(_q->fft);

    free(_q);
}

void ofdmframe64gen_print(ofdmframe64gen _q)
{
    printf("ofdmframe64gen:\n");
    printf("    num subcarriers     :   %u\n", _q->num_subcarriers);
    printf("    cyclic prefix len   :   %u (%6.2f%%)\n",
                    _q->cp_len,
                    100.0f*(float)(_q->cp_len)/(float)(_q->num_subcarriers));
}

void ofdmframe64gen_reset(ofdmframe64gen _q)
{
    msequence_reset(_q->ms_pilot);
}

void ofdmframe64gen_writeshortsequence(ofdmframe64gen _q,
                                       float complex * _y)
{
    // move cyclic prefix (double)
    memmove(_y+0,       _q->St+32,  32*sizeof(float complex));

    // move remainder of signal (twice)
    memmove(_y+32,      _q->St,     64*sizeof(float complex));
    memmove(_y+32+64,   _q->St,     64*sizeof(float complex));
}


void ofdmframe64gen_writelongsequence(ofdmframe64gen _q,
                                      float complex * _y)
{
    // move cyclic prefix (double)
    memmove(_y+0,       _q->Lt+32,  32*sizeof(float complex));

    // move remainder of signal (twice)
    memmove(_y+32,      _q->Lt,     64*sizeof(float complex));
    memmove(_y+32+64,   _q->Lt,     64*sizeof(float complex));
}

void ofdmframe64gen_writeheader(ofdmframe64gen _q,
                                float complex * _y)
{
}

void ofdmframe64gen_writesymbol(ofdmframe64gen _q,
                                float complex * _x,
                                float complex * _y)
{
    unsigned int pilot_phase = msequence_advance(_q->ms_pilot);

    // move frequency data to internal buffer
    unsigned int i, j=0;
    int sctype;
    for (i=0; i<64; i++) {
        sctype = ofdmframe64_getsctype(i);
        if (sctype==OFDMFRAME64_SCTYPE_NULL) {
            // disabled subcarrier
            _q->X[i] = 0.0f;
        } else if (sctype==OFDMFRAME64_SCTYPE_PILOT) {
            // pilot subcarrier
            _q->X[i] = (pilot_phase ? 1.0f : -1.0f) * _q->zeta;
        } else {
            // data subcarrier
            _q->X[i] = _x[j++] * _q->zeta;
        }

        //printf("X[%3u] = %12.8f + j*%12.8f;\n",i+1,crealf(_q->X[i]),cimagf(_q->X[i]));
    }
    assert(j==48);

    // execute inverse fft, store in buffer _q->x
    FFT_EXECUTE(_q->fft);

    // copy cyclic prefix
    memmove(_y, _q->xcp, (_q->cp_len)*sizeof(float complex));

    // copy remainder of signal
    memmove(&_y[_q->cp_len], _q->x, (_q->num_subcarriers)*sizeof(float complex));
}

