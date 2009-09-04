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

#ifndef __LIQUID_OFDMFRAME64SYNC_BENCH_H__
#define __LIQUID_OFDMFRAME64SYNC_BENCH_H__

#include <sys/resource.h>
#include <math.h>
#include "liquid.h"

// Helper function to keep code base small
void benchmark_ofdmframe64sync(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations)
{
    // options
    modulation_scheme ms = MOD_QAM;
    unsigned int bps = 4;
    unsigned int num_symbols = 8;

    // create synthesizer/analyzer objects
    ofdmframe64gen fg = ofdmframe64gen_create();
    //ofdmframe64gen_print(fg);

    modem mod = modem_create(ms,bps);

    ofdmframe64sync fs = ofdmframe64sync_create(NULL,NULL);

    unsigned int i;
    unsigned int frame_len = 160+160+80*num_symbols;
    float complex X[48];
    float complex x[frame_len];         // time-domain symbol

    unsigned int s;
    for (i=0; i<48; i++) {
        s = modem_gen_rand_sym(mod);
        modem_modulate(mod,s,&X[i]);
    }

    // generate frame
    unsigned int n=0;
    ofdmframe64gen_writeshortsequence(fg,&x[n]);
    n += 160;
    ofdmframe64gen_writelongsequence(fg,&x[n]);
    n += 160;
    for (i=0; i<num_symbols; i++) {
        ofdmframe64gen_writesymbol(fg,X,&x[n]);
        n += 80;
    }

    // start trials
    *_num_iterations /= 64;
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        ofdmframe64sync_execute(fs,x,frame_len);
        ofdmframe64sync_reset(fs);
    }
    getrusage(RUSAGE_SELF, _finish);

    // destroy objects
    ofdmframe64gen_destroy(fg);
    ofdmframe64sync_destroy(fs);
    modem_destroy(mod);
}


#endif // __LIQUID_OFDMFRAME64SYNC_BENCH_H__

