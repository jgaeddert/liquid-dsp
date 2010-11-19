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
// ofdmoqamframesync.c
//
// OFDM/OQAM frame synchronizer
//

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "liquid.internal.h"

#define DEBUG_OFDMOQAMFRAMESYNC             1
#define DEBUG_OFDMOQAMFRAMESYNC_PRINT       0
#define DEBUG_OFDMOQAMFRAMESYNC_FILENAME    "ofdmoqamframesync_internal_debug.m"
#define DEBUG_OFDMOQAMFRAMESYNC_BUFFER_LEN  (2048)

#if DEBUG_OFDMOQAMFRAMESYNC
void ofdmoqamframesync_debug_print(ofdmoqamframesync _q);
#endif

struct ofdmoqamframesync_s {
    unsigned int M;         // number of subcarriers
    unsigned int m;         // filter delay (symbols)
    float beta;             // filter excess bandwidth factor
    unsigned int * p;       // subcarrier allocation (null, pilot, data)

    // constants
    unsigned int M2;        // M/2 (time delay)
    unsigned int M_null;    // number of null subcarriers
    unsigned int M_pilot;   // number of pilot subcarriers
    unsigned int M_data;    // number of data subcarriers
    float zeta;             // scaling factor : zeta = M / sqrt(M_pilot + M_data)

    // filterbank objects
    firpfbch_crcf ca0;      // upper analysis filterbank
    firpfbch_crcf ca1;      // lower analysis filterbank

    // generic transform buffers
    float complex * X0;     // 
    float complex * X1;     // 

    // 
    float complex * S0;     // short sequence
    float complex * S1;     // long sequence

    // gain
    float g;                // coarse gain estimate
    float complex * G0;     // complex subcarrier gain estimate, S2[0]
    float complex * G1;     // complex subcarrier gain estimate, S2[1]
    float complex * G;      // complex subcarrier gain estimate

    // receiver state
    enum {
        OFDMOQAMFRAMESYNC_STATE_PLCPSHORT=0,  // seek PLCP short sequence
        OFDMOQAMFRAMESYNC_STATE_PLCPLONG0,    // seek first PLCP long sequence
        OFDMOQAMFRAMESYNC_STATE_PLCPLONG1,    // seek second PLCP long sequence
        OFDMOQAMFRAMESYNC_STATE_RXSYMBOLS     // receive payload symbols
    } state;

    // input delay buffer
    windowcf input_buffer;

    ofdmoqamframesync_callback callback;
    void * userdata;

#if DEBUG_OFDMOQAMFRAMESYNC
    windowcf debug_x;
    windowcf debug_rxx;
    windowcf debug_rxy;
    windowcf debug_framesyms;
    windowf debug_pilotphase;
    windowf debug_pilotphase_hat;
    windowf debug_rssi;
#endif
};

ofdmoqamframesync ofdmoqamframesync_create(unsigned int _M,
                                           unsigned int _m,
                                           float _beta,
                                           unsigned int * _p,
                                           ofdmoqamframesync_callback _callback,
                                           void * _userdata)
{
    ofdmoqamframesync q = (ofdmoqamframesync) malloc(sizeof(struct ofdmoqamframesync_s));

    // validate input
    if (_M % 2) {
        fprintf(stderr,"error: ofdmoqamframesync_create(), number of subcarriers must be even\n");
        exit(1);
    } else if (_M < 8) {
        fprintf(stderr,"warning: ofdmoqamframesync_create(), less than 8 subcarriers\n");
        exit(1);
    } else if (_m < 1) {
        fprintf(stderr,"error: ofdmoqamframesync_create(), filter delay must be > 0\n");
        exit(1);
    } else if (_beta < 0.0f) {
        fprintf(stderr,"error: ofdmoqamframesync_create(), filter excess bandwidth must be > 0\n");
        exit(1);
    }
    q->M = _M;
    q->m = _m;
    q->beta = _beta;

    // subcarrier allocation
    q->p = (unsigned int*) malloc((q->M)*sizeof(unsigned int));
    if (_p == NULL) {
        ofdmoqamframe_init_default_sctype(q->M, q->p);
    } else {
        memmove(q->p, _p, q->M*sizeof(unsigned int));
    }

    // validate and count subcarrier allocation
    ofdmoqamframe_validate_sctype(q->p, q->M, &q->M_null, &q->M_pilot, &q->M_data);
    if ( (q->M_pilot + q->M_data) == 0) {
        fprintf(stderr,"error: ofdmoqamframesync_create(), must have at least one enabled subcarrier\n");
        exit(1);
    }

    // compute scaling factor
    q->zeta = q->M / sqrtf(q->M_pilot + q->M_data);

    // derived values
    q->M2 = q->M/2;
    
    // create analysis filter banks
    q->ca0 = firpfbch_crcf_create_rnyquist(FIRPFBCH_ANALYZER, q->M, q->m, q->beta, 0);
    q->ca1 = firpfbch_crcf_create_rnyquist(FIRPFBCH_ANALYZER, q->M, q->m, q->beta, 0);
    q->X0 = (float complex*) malloc((q->M)*sizeof(float complex));
    q->X1 = (float complex*) malloc((q->M)*sizeof(float complex));
 
    // allocate memory for PLCP arrays
    q->S0 = (float complex*) malloc((q->M)*sizeof(float complex));
    q->S1 = (float complex*) malloc((q->M)*sizeof(float complex));
    ofdmoqamframe_init_S0(q->p, q->M, q->S0);
    ofdmoqamframe_init_S1(q->p, q->M, q->S1);

    // input buffer
    q->input_buffer = windowcf_create((q->M));

    // gain
    q->g = 1.0f;
    q->G0 = (float complex*) malloc((q->M)*sizeof(float complex));
    q->G1 = (float complex*) malloc((q->M)*sizeof(float complex));
    q->G  = (float complex*) malloc((q->M)*sizeof(float complex));

    // set callback data
    q->callback = _callback;
    q->userdata = _userdata;

    // reset object
    ofdmoqamframesync_reset(q);

#if DEBUG_OFDMOQAMFRAMESYNC
    q->debug_x =        windowcf_create(DEBUG_OFDMOQAMFRAMESYNC_BUFFER_LEN);
    q->debug_rxx=       windowcf_create(DEBUG_OFDMOQAMFRAMESYNC_BUFFER_LEN);
    q->debug_rxy=       windowcf_create(DEBUG_OFDMOQAMFRAMESYNC_BUFFER_LEN);
    q->debug_framesyms= windowcf_create(DEBUG_OFDMOQAMFRAMESYNC_BUFFER_LEN);
    q->debug_pilotphase= windowf_create(DEBUG_OFDMOQAMFRAMESYNC_BUFFER_LEN);
    q->debug_pilotphase_hat= windowf_create(DEBUG_OFDMOQAMFRAMESYNC_BUFFER_LEN);
    q->debug_rssi=       windowf_create(DEBUG_OFDMOQAMFRAMESYNC_BUFFER_LEN);
#endif

    // return object
    return q;
}

void ofdmoqamframesync_destroy(ofdmoqamframesync _q)
{
#if DEBUG_OFDMOQAMFRAMESYNC
    ofdmoqamframesync_debug_print(_q);
    windowcf_destroy(_q->debug_x);
    windowcf_destroy(_q->debug_rxx);
    windowcf_destroy(_q->debug_rxy);
    windowcf_destroy(_q->debug_framesyms);
    windowf_destroy(_q->debug_pilotphase);
    windowf_destroy(_q->debug_pilotphase_hat);
    windowf_destroy(_q->debug_rssi);
#endif

    // free analysis filterbank objects
    firpfbch_crcf_destroy(_q->ca0);
    firpfbch_crcf_destroy(_q->ca1);
    free(_q->X0);
    free(_q->X1);

    // clean up PLCP arrays
    free(_q->S0);
    free(_q->S1);

    // free gain arrays
    free(_q->G0);
    free(_q->G1);
    free(_q->G);

    // free input buffer
    windowcf_destroy(_q->input_buffer);

    // free main object memory
    free(_q);
}

void ofdmoqamframesync_print(ofdmoqamframesync _q)
{
    printf("ofdmoqamframesync:\n");
    printf("    num subcarriers     :   %-u\n", _q->M);
    printf("    m (filter delay)    :   %-u\n", _q->m);
    printf("    beta (excess b/w)   :   %8.6f\n", _q->beta);
}

void ofdmoqamframesync_reset(ofdmoqamframesync _q)
{
    // clear input buffer
    windowcf_clear(_q->input_buffer);

    // clear analysis filter bank objects
    firpfbch_crcf_clear(_q->ca0);
    firpfbch_crcf_clear(_q->ca1);

    // reset gain parameters
    unsigned int i;
    for (i=0; i<_q->M; i++)
        _q->G[i] = 1.0f;

    // reset state
    _q->state = OFDMOQAMFRAMESYNC_STATE_PLCPSHORT;
}

void ofdmoqamframesync_execute(ofdmoqamframesync _q,
                                 float complex * _x,
                                 unsigned int _n)
{
    unsigned int i;
    float complex x;
    for (i=0; i<_n; i++) {
        x = _x[i];
#if DEBUG_OFDMOQAMFRAMESYNC
        windowcf_push(_q->debug_x,x);
#endif
        
        // push sample into analysis filter banks
        float complex x_delay0;
        float complex x_delay1;
        windowcf_index(_q->input_buffer, 0,      &x_delay0); // full symbol delay
        windowcf_index(_q->input_buffer, _q->M2, &x_delay1); // half symbol delay

        windowcf_push(_q->input_buffer,x);
        firpfbch_crcf_analyzer_push(_q->ca0, x_delay0);  // push input sample
        firpfbch_crcf_analyzer_push(_q->ca1, x_delay1);  // push delayed sample

        switch (_q->state) {
        case OFDMOQAMFRAMESYNC_STATE_PLCPSHORT:
            ofdmoqamframesync_execute_plcpshort(_q,x);
            break;
        case OFDMOQAMFRAMESYNC_STATE_PLCPLONG0:
            ofdmoqamframesync_execute_plcplong0(_q,x);
            break;
        case OFDMOQAMFRAMESYNC_STATE_PLCPLONG1:
            ofdmoqamframesync_execute_plcplong1(_q,x);
            break;
        case OFDMOQAMFRAMESYNC_STATE_RXSYMBOLS:
            ofdmoqamframesync_execute_rxsymbols(_q,x);
            break;
        default:;
        }

    } // for (i=0; i<_n; i++)
} // ofdmoqamframesync_execute()

//
// internal
//

void ofdmoqamframesync_execute_plcpshort(ofdmoqamframesync _q,
                                         float complex _x)
{
}

void ofdmoqamframesync_execute_plcplong0(ofdmoqamframesync _q,
                                         float complex _x)
{
}

void ofdmoqamframesync_execute_plcplong1(ofdmoqamframesync _q,
                                         float complex _x)
{
}

void ofdmoqamframesync_execute_rxsymbols(ofdmoqamframesync _q,
                                         float complex _x)
{
}

void ofdmoqamframesync_rxpayload(ofdmoqamframesync _q,
                                 float complex * _Y0,
                                 float complex * _Y1)
{
}

void ofdmoqamframesync_estimate_gain_plcplong(ofdmoqamframesync _q)
{
}

float ofdmoqamframesync_estimate_pilot_phase(float complex _y0,
                                             float complex _y1,
                                             float complex _p)
{
    return 0.0f;
}


#if DEBUG_OFDMOQAMFRAMESYNC
void ofdmoqamframesync_debug_print(ofdmoqamframesync _q)
{
    FILE * fid = fopen(DEBUG_OFDMOQAMFRAMESYNC_FILENAME,"w");
    if (!fid) {
        printf("error: ofdmoqamframe_debug_print(), could not open file for writing\n");
        return;
    }
    fprintf(fid,"%% %s : auto-generated file\n", DEBUG_OFDMOQAMFRAMESYNC_FILENAME);
    fprintf(fid,"close all;\n");
    fprintf(fid,"clear all;\n");
    fprintf(fid,"n = %u;\n", DEBUG_OFDMOQAMFRAMESYNC_BUFFER_LEN);
    unsigned int i;
    float complex * rc;
    //float * r;


    // short, long, training sequences
    for (i=0; i<_q->M; i++) {
        fprintf(fid,"S0(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(_q->S0[i]), cimagf(_q->S0[i]));
        fprintf(fid,"S1(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(_q->S1[i]), cimagf(_q->S1[i]));
    }

    fprintf(fid,"x = zeros(1,n);\n");
    windowcf_read(_q->debug_x, &rc);
    for (i=0; i<DEBUG_OFDMOQAMFRAMESYNC_BUFFER_LEN; i++)
        fprintf(fid,"x(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(0:(n-1),real(x),0:(n-1),imag(x));\n");
    fprintf(fid,"xlabel('sample index');\n");
    fprintf(fid,"ylabel('received signal, x');\n");

    fclose(fid);
    printf("ofdmoqamframesync/debug: results written to %s\n", DEBUG_OFDMOQAMFRAMESYNC_FILENAME);
}
#endif


