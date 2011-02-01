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

#include "liquid.internal.h"

#define DEBUG_GMSKFRAMESYNC           1

// gmskframesync object structure
struct gmskframesync_s {
    gmskdem demod;          // gmsk demodulator

    // status variables
    enum {
        GMSKFRAMESYNC_STATE_SEEKPN=0,     // seek p/n sequence
        GMSKFRAMESYNC_STATE_RXHEADER,     // receive header data
        GMSKFRAMESYNC_STATE_RXPAYLOAD,    // receive payload data
        GMSKFRAMESYNC_STATE_RESET         // reset synchronizer
    } state;
    unsigned int num_symbols_collected; // symbols collected counter
    bool header_valid;                  // header valid?
    bool payload_valid;                 // payload valid?

    // callback
    gmskframesync_callback callback;      // user-defined callback function
    void * userdata;                    // user-defined data structure

    // header
    unsigned char header_sym[84];       // header symbols (modem output)
    unsigned char header_enc[21];       // header data (encoded)
    unsigned char header[12];           // header data (decoded)

    // payload
    unsigned char payload_sym[396];     // payload symbols (modem output)
    unsigned char payload_enc[99];      // payload data (encoded)
    unsigned char payload[64];          // payload data (decoded)

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
    gmskframesync fs = (gmskframesync) malloc(sizeof(struct gmskframesync_s));
    fs->callback = _callback;
    fs->userdata = _userdata;

    // create gmsk demodulator object
    fs->demod = gmskdem_create(_k, _m, _BT);

    return fs;
}


// destroy frame synchronizer object, freeing all internal memory
void gmskframesync_destroy(gmskframesync _fs)
{
    // destroy gmsk demodulator
    gmskdem_destroy(_fs->demod);

    // free main object memory
    free(_fs);
}

// print frame synchronizer object internals
void gmskframesync_print(gmskframesync _fs)
{
    printf("gmskframesync:\n");
}

// reset frame synchronizer object
void gmskframesync_reset(gmskframesync _fs)
{
    // reset demodulator
    gmskdem_reset(_fs->demod);
}

// execute frame synchronizer
//  _fs     :   frame synchronizer object
//  _x      :   input sample array [size: _n x 1]
//  _n      :   number of input samples
void gmskframesync_execute(gmskframesync _fs,
                           float complex *_x,
                           unsigned int _n)
{
    // ...
}

