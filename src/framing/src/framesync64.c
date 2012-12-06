/*
 * Copyright (c) 2007, 2008, 2009, 2010 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010 Virginia Polytechnic
 *                                      Institute & State University
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
// framesync64.c
//
// basic frame synchronizer
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <complex.h>

#include "liquid.internal.h"

#define FRAMESYNC64_SQUELCH_TIMEOUT (32)

#define FRAME64_PN_LEN              (64)

#define DEBUG_FRAMESYNC64           1
#define DEBUG_FRAMESYNC64_PRINT     0
#define DEBUG_FILENAME              "framesync64_internal_debug.m"
#define DEBUG_BUFFER_LEN            (4096)

void framesync64_debug_print(framesync64 _q);

// framesync64 object structure
struct framesync64_s {

    // synchronizer objects
    bpresync_cccf sync; // = bpresync_cccf_create(s0, k*num_sync_symbols, 0.05f, 11);

#if DEBUG_FRAMESYNC64
    windowcf debug_x;
    windowcf debug_rxy;
#endif
};

// create framesync64 object
//  _props          :   properties structure pointer (default if NULL)
//  _callback       :   callback function invoked when frame is received
//  _userdata       :   user-defined data object passed to callback
framesync64 framesync64_create(framesyncprops_s * _props,
                               framesync64_callback _callback,
                               void * _userdata)
{
    framesync64 q = (framesync64) malloc(sizeof(struct framesync64_s));

    unsigned int i;
    unsigned int j;

    unsigned int k = 2;
    unsigned int num_sync_symbols = 64;

    // generate pn sequence
    msequence ms = msequence_create(6, 0x0043, 1);
    float complex pn_sequence[num_sync_symbols];
    for (i=0; i<64; i++)
        pn_sequence[i] = (msequence_advance(ms)) ? 1.0f : -1.0f;
    msequence_destroy(ms);

    //
    float dphi_max = 0.08f;
    float complex s0[k*num_sync_symbols];
    for (i=0; i<num_sync_symbols; i++) {
        for (j=0; j<k; j++)
            s0[i*k + j] = pn_sequence[i];
    }
    q->sync = bpresync_cccf_create(s0, k*num_sync_symbols, dphi_max, 5);

#if DEBUG_FRAMESYNC64
    q->debug_x         = windowcf_create(DEBUG_BUFFER_LEN);
    q->debug_rxy       = windowcf_create(DEBUG_BUFFER_LEN);
#endif

    return q;
}

// get framesync64 properties
//  _q     :   frame synchronizer object
//  _props  :   frame synchronizer properties structure pointer
void framesync64_getprops(framesync64 _q,
                          framesyncprops_s * _props)
{
}

// set framesync64 properties
//  _q     :   frame synchronizer object
//  _props  :   frame synchronizer properties structure pointer
void framesync64_setprops(framesync64 _q,
                          framesyncprops_s * _props)
{
}

// destroy frame synchronizer object, freeing all internal memory
void framesync64_destroy(framesync64 _q)
{
#if DEBUG_FRAMESYNC64
    framesync64_debug_print(_q);

    // clean up debug windows
    windowcf_destroy(_q->debug_x);
    windowcf_destroy(_q->debug_rxy);
#endif

    // destroy synchronization objects
    bpresync_cccf_destroy(_q->sync);

    // free main object memory
    free(_q);
}

// print frame synchronizer object internals
void framesync64_print(framesync64 _q)
{
    printf("framesync64:\n");
}

// reset frame synchronizer object
void framesync64_reset(framesync64 _q)
{
    // reset binary pre-demod synchronizer
    bpresync_cccf_reset(_q->sync);
}

// execute frame synchronizer
//  _q     :   frame synchronizer object
//  _x      :   input sample array [size: _n x 1]
//  _n      :   number of input samples
void framesync64_execute(framesync64     _q,
                         float complex * _x,
                         unsigned int    _n)
{
    unsigned int i;
    float complex rxy;
    float dphi_est;
    for (i=0; i<_n; i++) {
        // push through pre-demod synchronizer
        bpresync_cccf_push(_q->sync, _x[i]);
        bpresync_cccf_correlate(_q->sync, &rxy, &dphi_est);

#if DEBUG_FRAMESYNC64
        windowcf_push(_q->debug_x,   _x[i]);
        windowcf_push(_q->debug_rxy, rxy);
#endif
    }
}

// 
// internal
//

// open bandwidth of synchronizers (acquisition mode)
void framesync64_open_bandwidth(framesync64 _q)
{
}

// close bandwidth of synchronizers (tracking mode)
void framesync64_close_bandwidth(framesync64 _q)
{
}

// 
// state-specific execute methods
//

// execute synchronizer, seeking p/n sequence
//  _q     :   frame synchronizer object
//  _x      :   input sample
//  _sym    :   demodulated symbol
void framesync64_execute_seekpn(framesync64   _q,
                                float complex _x,
                                unsigned int  _sym)
{
}

// execute synchronizer, receiving header
//  _q     :   frame synchronizer object
//  _x      :   input sample
//  _sym    :   demodulated symbol
void framesync64_execute_rxheader(framesync64   _q,
                                  float complex _x,
                                  unsigned int  _sym)
{
}

// execute synchronizer, receiving payload
//  _q     :   frame synchronizer object
//  _x      :   input sample
//  _sym    :   demodulated symbol
void framesync64_execute_rxpayload(framesync64   _q,
                                   float complex _x,
                                   unsigned int  _sym)
{
}

// execute synchronizer, resetting object
//  _q     :   frame synchronizer object
//  _x      :   input sample
//  _sym    :   demodulated symbol
void framesync64_execute_reset(framesync64   _q,
                               float complex _x,
                               unsigned int  _sym)
{
}


// enable csma and set external callback functions
//  _q             :   frame synchronizer object
//  _csma_lock      :   callback to be invoked when signal is high
//  _csma_unlock    :   callback to be invoked when signal is again low
//  _csma_userdata  :   structure passed to callback functions
void framesync64_set_csma_callbacks(framesync64 _q,
                                    framesync_csma_callback _csma_lock,
                                    framesync_csma_callback _csma_unlock,
                                    void * _csma_userdata)
{
}

// if enabled, invoke external csma lock callback
void framesync64_csma_lock(framesync64 _q)
{
}

// if enabled, invoke external csma unlock callback
void framesync64_csma_unlock(framesync64 _q)
{
}


// 
// decoding methods
//

// decode header
void framesync64_decode_header(framesync64 _q)
{
}

void framesync64_decode_payload(framesync64 _q)
{
}

// convert four 2-bit symbols into one 8-bit byte
//  _syms   :   input symbols [size: 4 x 1]
//  _byte   :   output byte
void framesync64_syms_to_byte(unsigned char * _syms,
                              unsigned char * _byte)
{
    unsigned char b=0;
    b |= (_syms[0] << 6) & 0xc0;
    b |= (_syms[1] << 4) & 0x30;
    b |= (_syms[2] << 2) & 0x0c;
    b |= (_syms[3]     ) & 0x03;
    *_byte = b;
}

// huge method to write debugging data to file
void framesync64_debug_print(framesync64 _q)
{
#if DEBUG_FRAMESYNC64
    unsigned int i;
    float complex * rc;
    FILE* fid = fopen(DEBUG_FILENAME,"w");
    fprintf(fid,"%% %s: auto-generated file", DEBUG_FILENAME);
    fprintf(fid,"\n\n");
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n\n");

    // write x
    fprintf(fid,"x = zeros(1,%u);\n", DEBUG_BUFFER_LEN);
    windowcf_read(_q->debug_x, &rc);
    for (i=0; i<DEBUG_BUFFER_LEN; i++)
        fprintf(fid,"x(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(1:length(x),real(x), 1:length(x),imag(x));\n");
    fprintf(fid,"ylabel('received signal, x');\n");

    // write rxy
    fprintf(fid,"rxy = zeros(1,%u);\n", DEBUG_BUFFER_LEN);
    windowcf_read(_q->debug_rxy, &rc);
    for (i=0; i<DEBUG_BUFFER_LEN; i++)
        fprintf(fid,"rxy(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(abs(rxy))\n");
    fprintf(fid,"ylabel('|r_{xy}|');\n");

    fprintf(fid,"\n\n");
    fclose(fid);

    printf("framesync64/debug: results written to %s\n", DEBUG_FILENAME);
#endif
}
