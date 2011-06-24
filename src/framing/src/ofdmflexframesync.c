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
// ofdmflexframesync.c
//
// OFDM frame synchronizer
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "liquid.internal.h"

#define DEBUG_OFDMFLEXFRAMESYNC             1
#define DEBUG_OFDMFLEXFRAMESYNC_PRINT       0
#define DEBUG_OFDMFLEXFRAMESYNC_FILENAME    "ofdmflexframesync_internal_debug.m"
#define DEBUG_OFDMFLEXFRAMESYNC_BUFFER_LEN  (2048)

#if DEBUG_OFDMFLEXFRAMESYNC
void ofdmflexframesync_debug_print(ofdmflexframesync _q);
#endif

struct ofdmflexframesync_s {
    unsigned int M;         // number of subcarriers
    unsigned int cp_len;    // cyclic prefix length
    unsigned int * p;       // subcarrier allocation (null, pilot, data)

    // constants
    unsigned int M_null;    // number of null subcarriers
    unsigned int M_pilot;   // number of pilot subcarriers
    unsigned int M_data;    // number of data subcarriers
    unsigned int M_S0;      // number of enabled subcarriers in S0
    unsigned int M_S1;      // number of enabled subcarriers in S1

    // internal...
    ofdmframesync fs;       // internal OFDM frame synchronizer

    // header
    // payload
    // packetizer
};

ofdmflexframesync ofdmflexframesync_create(unsigned int _M,
                                           unsigned int _cp_len,
                                           unsigned int * _p,
                                           //unsigned int _taper_len,
                                           ofdmflexframesync_callback _callback,
                                           void * _userdata)
{
    ofdmflexframesync q = (ofdmflexframesync) malloc(sizeof(struct ofdmflexframesync_s));

    // validate input
    if (_M < 8) {
        fprintf(stderr,"warning: ofdmflexframesync_create(), less than 8 subcarriers\n");
    } else if (_M % 2) {
        fprintf(stderr,"error: ofdmflexframesync_create(), number of subcarriers must be even\n");
        exit(1);
    } else if (_cp_len > _M) {
        fprintf(stderr,"error: ofdmflexframesync_create(), cyclic prefix length cannot exceed number of subcarriers\n");
        exit(1);
    }
    q->M = _M;
    q->cp_len = _cp_len;

    // set callback data
    //q->callback = _callback;
    //q->userdata = _userdata;

    // create ...
    q->fs = ofdmframesync_create(_M, _cp_len, _p, ofdmflexframesync_internal_callback, (void*)q);

    // return object
    return q;
}

void ofdmflexframesync_destroy(ofdmflexframesync _q)
{
    // free internal objects
    ofdmframesync_destroy(_q->fs);

    // free main object memory
    free(_q);
}

void ofdmflexframesync_print(ofdmflexframesync _q)
{
    printf("ofdmflexframesync:\n");
}

void ofdmflexframesync_reset(ofdmflexframesync _q)
{
}

void ofdmflexframesync_execute(ofdmflexframesync _q,
                               float complex * _x,
                               unsigned int _n)
{
    // push samples through ofdmframesync object
    ofdmframesync_execute(_q->fs, _x, _n);
}

//
// internal methods
//

// internal callback
int ofdmflexframesync_internal_callback(float complex *_x,
                                        unsigned int * _p,
                                        unsigned int _M,
                                        void * _userdata)
{
    printf("******* ofdmflexframesync callback invoked!\n");
    return 0;
}

