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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/resource.h>
#include "liquid.h"

// 
void benchmark_ofdmoqamframe64sync(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations)
{
    // options
    unsigned int m=3;
    float beta=0.7f;
    modulation_scheme ms = MOD_QAM;
    unsigned int bps = 4;
    unsigned int num_symbols = 8;

    // create synthesizer/analyzer objects
    ofdmoqamframe64gen fg = ofdmoqamframe64gen_create(m,beta);
    //ofdmoqamframe64gen_print(fg);

    modem mod = modem_create(ms,bps);

    ofdmoqamframe64sync fs = ofdmoqamframe64sync_create(m,beta,NULL,NULL);

    unsigned int i;
    unsigned int frame_len = 64*8 + 64*(num_symbols + 2*m+1);
    float complex X[48];
    float complex x[frame_len];         // time-domain symbol

    unsigned int s;
    for (i=0; i<48; i++) {
        s = modem_gen_rand_sym(mod);
        modem_modulate(mod,s,&X[i]);
    }

    // generate frame
    unsigned int n=0;

    // short sequence S0
    ofdmoqamframe64gen_writeshortsequence(fg,&x[n]);    n += 64;
    ofdmoqamframe64gen_writeshortsequence(fg,&x[n]);    n += 64;

    // long sequence S1
    ofdmoqamframe64gen_writelongsequence(fg,&x[n]);     n += 64;
    ofdmoqamframe64gen_writelongsequence(fg,&x[n]);     n += 64;

    // training sequence S2
    ofdmoqamframe64gen_writetrainingsequence(fg,&x[n]); n += 64;
    ofdmoqamframe64gen_writetrainingsequence(fg,&x[n]); n += 64;
    ofdmoqamframe64gen_writetrainingsequence(fg,&x[n]); n += 64;
    ofdmoqamframe64gen_writetrainingsequence(fg,&x[n]); n += 64;

    // data symbols
    for (i=0; i<num_symbols; i++) {
        ofdmoqamframe64gen_writesymbol(fg,X,&x[n]);
        n += 64;
    }

    // flush
    for (i=0; i<2*m+1; i++) {
        ofdmoqamframe64gen_flush(fg,&x[n]);
        n += 64;
    }

    // start trials
    *_num_iterations /= 640;
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        ofdmoqamframe64sync_execute(fs,x,frame_len);
        ofdmoqamframe64sync_reset(fs);
    }
    getrusage(RUSAGE_SELF, _finish);

    // destroy objects
    ofdmoqamframe64gen_destroy(fg);
    ofdmoqamframe64sync_destroy(fs);
    modem_destroy(mod);
}

