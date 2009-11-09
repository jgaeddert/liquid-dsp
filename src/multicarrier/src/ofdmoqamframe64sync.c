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
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "liquid.internal.h"

#if HAVE_FFTW3_H
#   include <fftw3.h>
#endif

#define DEBUG_OFDMOQAMFRAME64SYNC             1
#define DEBUG_OFDMOQAMFRAME64SYNC_PRINT       0
#define DEBUG_OFDMOQAMFRAME64SYNC_FILENAME    "ofdmoqamframe64sync_internal_debug.m"
#define DEBUG_OFDMOQAMFRAME64SYNC_BUFFER_LEN  (1024)

// auto-correlation integration length
#define OFDMOQAMFRAME64SYNC_AUTOCORR_LEN      (96)

#if DEBUG_OFDMOQAMFRAME64SYNC
void ofdmoqamframe64sync_debug_print(ofdmoqamframe64sync _q);
#endif

struct ofdmoqamframe64sync_s {
    unsigned int num_subcarriers;   // 64
    unsigned int m;
    float beta;

    // constants
    float zeta;         // scaling factor

    // PLCP
    float complex * S0; // short sequence
    float complex * S1; // long sequence

#if DEBUG_OFDMOQAMFRAME64SYNC
    cfwindow debug_x;
    cfwindow debug_rxx;
    cfwindow debug_rxy;
    cfwindow debug_framesyms;
#endif
};

ofdmoqamframe64sync ofdmoqamframe64sync_create(ofdmoqamframe64sync_callback _callback,
                                       void * _userdata)
{
    ofdmoqamframe64sync q = (ofdmoqamframe64sync) malloc(sizeof(struct ofdmoqamframe64sync_s));
    q->num_subcarriers = 64;
    q->m = 2;
    q->beta = 0.7f;

    q->zeta = 1.0f;
    
    // allocate memory for PLCP arrays
    q->S0 = (float complex*) malloc((q->num_subcarriers)*sizeof(float complex));
    q->S1 = (float complex*) malloc((q->num_subcarriers)*sizeof(float complex));
    ofdmoqamframe64_init_S0(q->S0);
    ofdmoqamframe64_init_S1(q->S1);

#if DEBUG_OFDMOQAMFRAME64SYNC
    q->debug_x   = cfwindow_create(DEBUG_OFDMOQAMFRAME64SYNC_BUFFER_LEN);
    q->debug_rxx = cfwindow_create(DEBUG_OFDMOQAMFRAME64SYNC_BUFFER_LEN);
    q->debug_rxy = cfwindow_create(DEBUG_OFDMOQAMFRAME64SYNC_BUFFER_LEN);
    q->debug_framesyms = cfwindow_create(DEBUG_OFDMOQAMFRAME64SYNC_BUFFER_LEN);
#endif

    ofdmoqamframe64sync_reset(q);

    return q;
}

void ofdmoqamframe64sync_destroy(ofdmoqamframe64sync _q)
{
#if DEBUG_OFDMOQAMFRAME64SYNC
    ofdmoqamframe64sync_debug_print(_q);
    cfwindow_destroy(_q->debug_x);
    cfwindow_destroy(_q->debug_rxx);
    cfwindow_destroy(_q->debug_rxy);
    cfwindow_destroy(_q->debug_framesyms);
#endif

    // clean up PLCP arrays
    free(_q->S0);
    free(_q->S1);

    free(_q);
}

void ofdmoqamframe64sync_print(ofdmoqamframe64sync _q)
{
    printf("ofdmoqamframe64sync:\n");
}

void ofdmoqamframe64sync_reset(ofdmoqamframe64sync _q)
{
}

void ofdmoqamframe64sync_execute(ofdmoqamframe64sync _q,
                             float complex * _x,
                             unsigned int _n)
{
}

//
// internal
//

#if DEBUG_OFDMOQAMFRAME64SYNC
void ofdmoqamframe64sync_debug_print(ofdmoqamframe64sync _q)
{
    FILE * fid = fopen(DEBUG_OFDMOQAMFRAME64SYNC_FILENAME,"w");
    if (!fid) {
        printf("error: ofdmoqamframe64_debug_print(), could not open file for writing\n");
        return;
    }
    fprintf(fid,"%% %s : auto-generated file\n", DEBUG_OFDMOQAMFRAME64SYNC_FILENAME);
    fprintf(fid,"close all;\n");
    fprintf(fid,"clear all;\n");
    fprintf(fid,"n = %u;\n", DEBUG_OFDMOQAMFRAME64SYNC_BUFFER_LEN);
    unsigned int i;
    float complex * rc;

    /*
    fprintf(fid,"nu_hat = %12.4e;\n", _q->nu_hat0 + _q->nu_hat1);

    // gain vectors
    for (i=0; i<64; i++) {
        fprintf(fid,"G(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(_q->G[i]), cimagf(_q->G[i]));
        fprintf(fid,"G0(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(_q->G0[i]), cimagf(_q->G0[i]));
        fprintf(fid,"G1(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(_q->G1[i]), cimagf(_q->G1[i]));
    }
    */
 
    fprintf(fid,"x = zeros(1,n);\n");
    cfwindow_read(_q->debug_x, &rc);
    for (i=0; i<DEBUG_OFDMOQAMFRAME64SYNC_BUFFER_LEN; i++)
        fprintf(fid,"x(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(0:(n-1),real(x),0:(n-1),imag(x));\n");
    fprintf(fid,"xlabel('sample index');\n");
    fprintf(fid,"ylabel('received signal, x');\n");

    fprintf(fid,"rxx = zeros(1,n);\n");
    cfwindow_read(_q->debug_rxx, &rc);
    for (i=0; i<DEBUG_OFDMOQAMFRAME64SYNC_BUFFER_LEN; i++)
        fprintf(fid,"rxx(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(0:(n-1),abs(rxx));\n");
    fprintf(fid,"xlabel('sample index');\n");
    fprintf(fid,"ylabel('|r_{xx}|');\n");

    fprintf(fid,"rxy = zeros(1,n);\n");
    cfwindow_read(_q->debug_rxy, &rc);
    for (i=0; i<DEBUG_OFDMOQAMFRAME64SYNC_BUFFER_LEN; i++)
        fprintf(fid,"rxy(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(0:(n-1),abs(rxy));\n");
    fprintf(fid,"xlabel('sample index');\n");
    fprintf(fid,"ylabel('|r_{xy}|');\n");


    // plot gain vectors
    fprintf(fid,"f = [-32:31];\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(2,1,1);\n");
    fprintf(fid,"    plot(f,fftshift(abs(G0)),f,fftshift(abs(G1)),f,fftshift(abs(G)));\n");
    fprintf(fid,"    ylabel('gain');\n");
    fprintf(fid,"subplot(2,1,2);\n");
    fprintf(fid,"    plot(f,unwrap(fftshift(arg(G0))),...\n");
    fprintf(fid,"         f,unwrap(fftshift(arg(G1))),...\n");
    fprintf(fid,"         f,unwrap(fftshift(arg(G))));\n");
    fprintf(fid,"    ylabel('phase');\n");

    // frame symbols
    fprintf(fid,"framesyms = zeros(1,n);\n");
    cfwindow_read(_q->debug_framesyms, &rc);
    for (i=0; i<DEBUG_OFDMOQAMFRAME64SYNC_BUFFER_LEN; i++)
        fprintf(fid,"framesyms(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(real(framesyms),imag(framesyms),'x','MarkerSize',1);\n");
    fprintf(fid,"axis square;\n");
    fprintf(fid,"axis([-1.5 1.5 -1.5 1.5]);\n");
    fprintf(fid,"xlabel('in-phase');\n");
    fprintf(fid,"ylabel('quadrature phase');\n");
    fprintf(fid,"title('Frame Symbols');\n");

    fclose(fid);
    printf("ofdmoqamframe64sync/debug: results written to %s\n", DEBUG_OFDMOQAMFRAME64SYNC_FILENAME);
}
#endif

