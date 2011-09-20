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

#define DEBUG_GMSKFRAMESYNC             1
#define DEBUG_GMSKFRAMESYNC_PRINT       0
#define DEBUG_GMSKFRAMESYNC_FILENAME    "gmskframesync_internal_debug.m"
#define DEBUG_GMSKFRAMESYNC_BUFFER_LEN  (2000)

// gmskframesync object structure
struct gmskframesync_s {
    unsigned int k;                     // filter samples/symbol
    unsigned int m;                     // filter semi-length (symbols)
    float BT;                           // filter bandwidth-time product

    // synchronizer parameters, objects
    float complex x_prime;              // received signal state
    float rssi_hat;                     // rssi estimate
    unsigned int npfb;                  // number of filterbanks
    symsync_rrrf symsync;               // symbol synchronizer, matched filter
    bpacketsync psync;                  // packet synchronizer

    bool header_valid;                  // header valid?
    bool payload_valid;                 // payload valid?

    // callback
    gmskframesync_callback callback;    // user-defined callback function
    void * userdata;                    // user-defined data structure
    framesyncstats_s framestats;        //

    // debugging macros
#if DEBUG_GMSKFRAMESYNC
    windowf  debug_agc_rssi;            // rssi buffer
    windowcf debug_x;                   // received samples buffer
#endif
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

    //
    q->rssi_hat = 1.0f;

#if DEBUG_GMSKFRAMESYNC
    // debugging
    q->debug_agc_rssi   = windowf_create(DEBUG_GMSKFRAMESYNC_BUFFER_LEN);
    q->debug_x          = windowcf_create(DEBUG_GMSKFRAMESYNC_BUFFER_LEN);
#endif

    return q;
}


// destroy frame synchronizer object, freeing all internal memory
void gmskframesync_destroy(gmskframesync _q)
{
#if DEBUG_GMSKFRAMESYNC
    // output debugging file
    gmskframesync_output_debug_file(_q, DEBUG_GMSKFRAMESYNC_FILENAME);

    // destroy debugging windows
    windowf_destroy(_q->debug_agc_rssi);
    windowcf_destroy(_q->debug_x);
#endif

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
                           float complex * _x,
                           unsigned int _n)
{
    // synchronized sample buffer
    float buffer[4];
    unsigned int num_written=0;

    // push through synchronizer
    unsigned int i;
    for (i=0; i<_n; i++) {
        // compute differential
        float complex s = conjf(_q->x_prime)*_x[i];
        _q->x_prime = _x[i];

        // update rssi estimate
        float alpha = 0.2f;
        _q->rssi_hat = (1.0f-alpha)*_q->rssi_hat + alpha*cabsf(s);
#if DEBUG_GMSKFRAMESYNC
        windowf_push(_q->debug_agc_rssi, 10*log10f(_q->rssi_hat));
        windowcf_push(_q->debug_x, _x[i]);
#endif
        // compute phase difference
        float phi = cargf(s);

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
    //_q->framestats.rssi = 10*log10f(_q->rssi_hat); // returns wrong value...
    _q->callback(_payload, _payload_len, _payload_valid, _q->framestats, _q->userdata);

    //
    return 0;
}

void gmskframesync_output_debug_file(gmskframesync _q,
                                     const char * _filename)
{
    unsigned int i;
    float * r;
    float complex * rc;
    FILE* fid = fopen(_filename,"w");
    if (!fid) {
        fprintf(stderr, "error: flexframesync_output_debug_file(), could not open '%s' for writing\n", _filename);
        return;
    }
    fprintf(fid,"%% %s: auto-generated file", _filename);
    fprintf(fid,"\n\n");
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n\n");

    // write agc_rssi
    fprintf(fid,"agc_rssi = zeros(1,%u);\n", DEBUG_GMSKFRAMESYNC_BUFFER_LEN);
    windowf_read(_q->debug_agc_rssi, &r);
    for (i=0; i<DEBUG_GMSKFRAMESYNC_BUFFER_LEN; i++)
        fprintf(fid,"agc_rssi(%4u) = %12.4e;\n", i+1, r[i]);
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(agc_rssi)\n");
    fprintf(fid,"ylabel('RSSI [dB]');\n");
    fprintf(fid,"\n\n");

    // write x
    fprintf(fid,"x = zeros(1,%u);\n", DEBUG_GMSKFRAMESYNC_BUFFER_LEN);
    windowcf_read(_q->debug_x, &rc);
    for (i=0; i<DEBUG_GMSKFRAMESYNC_BUFFER_LEN; i++)
        fprintf(fid,"x(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(1:length(x),real(x), 1:length(x),imag(x));\n");
    fprintf(fid,"ylabel('received signal, x');\n");
    fprintf(fid,"\n\n");


    fclose(fid);

    printf("gmskframesync/debug: results written to '%s'\n", _filename);

}
