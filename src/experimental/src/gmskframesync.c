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
// gmskframesync.c
//

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <complex.h>

#include "liquid.experimental.h"

#define DEBUG_GMSKFRAMESYNC           1

// gmskframesync object structure
struct gmskframesync_s {
    unsigned int k;                     // filter samples/symbol
    unsigned int m;                     // filter semi-length (symbols)
    float BT;                           // filter bandwidth-time product

    // synchronizer parameters, objects
    float complex x_prime;              // received signal state
    unsigned int npfb;                  // number of filterbanks
    symsync_rrrf symsync;               // symbol synchronizer, matched filter
    bpacketsync psync;                  // packet synchronizer

    bool header_valid;                  // header valid?
    bool payload_valid;                 // payload valid?

    // callback
    gmskframesync_callback callback;    // user-defined callback function
    void * userdata;                    // user-defined data structure
    framesyncstats_s framestats;        //
};

// create gmskframesync object
//  _props          :   properties structure pointer (default if NULL)
//  _callback       :   callback function invoked when frame is received
//  _userdata       :   user-defined data object passed to callback
gmskframesync gmskframesync_create(unsigned int _k,
                                   unsigned int _m,
                                   float _BT,
                                   gmskframesync_callback _callback,
                                   void * _userdata)
{
    // TODO : validate input

    gmskframesync q = (gmskframesync) malloc(sizeof(struct gmskframesync_s));
    q->k  = _k;
    q->m  = _m;
    q->BT = _BT;
    q->callback = _callback;
    q->userdata = _userdata;

    // internal parameters
    q->npfb = 32;

    // create symbol synchronizer
    q->symsync = symsync_rrrf_create_rnyquist(LIQUID_RNYQUIST_GMSKRX,
                                              q->k,
                                              q->m,
                                              q->BT,
                                              q->npfb);
    symsync_rrrf_set_lf_bw(q->symsync, 0.02f);
    symsync_rrrf_set_output_rate(q->symsync, 1);

    // create packet synchronizer
    q->psync = bpacketsync_create(0, gmskframesync_internal_callback, (void*)q);

    return q;
}


// destroy frame synchronizer object, freeing all internal memory
void gmskframesync_destroy(gmskframesync _q)
{
    // destroy synchronizer objects
    symsync_rrrf_destroy(_q->symsync);  // symbol synchronizer, matched filter
    bpacketsync_destroy(_q->psync);     // packet synchronizer

    // free main object memory
    free(_q);
}

// print frame synchronizer object internals
void gmskframesync_print(gmskframesync _q)
{
    printf("gmskframesync:\n");
}

// reset frame synchronizer object
void gmskframesync_reset(gmskframesync _q)
{
    // reset state
    _q->x_prime = 0.0f;

    // reset synchronizer objects
    symsync_rrrf_clear(_q->symsync);
}

// execute frame synchronizer
//  _q      :   frame synchronizer object
//  _x      :   input sample array [size: _n x 1]
//  _n      :   number of input samples
void gmskframesync_execute(gmskframesync _q,
                           float complex *_x,
                           unsigned int _n)
{
    // synchronized sample buffer
    float buffer[16];
    unsigned int num_written=0;

    // push through synchronizer
    unsigned int i;
    for (i=0; i<_n; i++) {
        // compute phase difference
        float phi = cargf( conjf(_q->x_prime)*_x[i] );
        _q->x_prime = _x[i];

        // push through matched filter/symbol timing recovery
        symsync_rrrf_execute(_q->symsync, &phi, 1, buffer, &num_written);

        // demodulate
        unsigned int j;
        for (j=0; j<num_written; j++) {
            unsigned char s = buffer[j] > 0.0f ? 1 : 0;

            // push sample through packet synchronizer...
            bpacketsync_execute_bit(_q->psync, s);
        }
    }
}

// 
// internal methods
//

// internal callback
int gmskframesync_internal_callback(unsigned char * _payload,
                                    int             _payload_valid,
                                    unsigned int    _payload_len,
                                    void *          _userdata)
{
    // type-cast internal object
    gmskframesync _q = (gmskframesync) _userdata;

    // invoke user-defined callback
    framesyncstats_init_default(&_q->framestats);
    _q->callback(_payload, _payload_len, _payload_valid, _q->framestats, _q->userdata);

    //
    return 0;
}

