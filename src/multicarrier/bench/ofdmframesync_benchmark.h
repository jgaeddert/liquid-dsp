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

#ifndef __LIQUID_OFDMFRAMESYNC_BENCH_H__
#define __LIQUID_OFDMFRAMESYNC_BENCH_H__

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/resource.h>
#include "liquid.h"

#define OFDMFRAMESYNC_EXECUTE_BENCH_API(NUM_SUBCARRIERS,CP_LEN) \
(   struct rusage *_start,                              \
    struct rusage *_finish,                             \
    unsigned long int *_num_iterations)                 \
{ ofdmframesync_execute_bench(_start, _finish, _num_iterations, NUM_SUBCARRIERS, CP_LEN); }

// Helper function to keep code base small
void ofdmframesync_execute_bench(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations,
    unsigned int _num_subcarriers,
    unsigned int _cp_len)
{

    // options
    modulation_scheme ms = MOD_QAM;
    unsigned int bps     = 4;

    // 
    unsigned int frame_len = _num_subcarriers + _cp_len;

    // create synthesizer/analyzer objects
    ofdmframegen fg = ofdmframegen_create(_num_subcarriers, _cp_len);
    //ofdmframegen_print(fg);

    modem mod = modem_create(ms,bps);

    ofdmframesync fs = ofdmframesync_create(_num_subcarriers,_cp_len,NULL,NULL);

    unsigned int i;
    float complex X[_num_subcarriers];   // channelized symbols
    float complex x[frame_len];         // time-domain symbol

    unsigned int s;
    for (i=0; i<_num_subcarriers; i++) {
        s = modem_gen_rand_sym(mod);
        modem_modulate(mod,s,&X[i]);
    }

    // generate frame
    ofdmframegen_execute(fg,X,x);

    // start trials
    *_num_iterations /= _num_subcarriers;
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        ofdmframesync_execute(fs,x,frame_len);
        ofdmframesync_execute(fs,x,frame_len);
        ofdmframesync_execute(fs,x,frame_len);
        ofdmframesync_execute(fs,x,frame_len);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;

    // destroy objects
    ofdmframegen_destroy(fg);
    ofdmframesync_destroy(fs);
    modem_destroy(mod);
}

//
void benchmark_ofdmframesync_execute_n64    OFDMFRAMESYNC_EXECUTE_BENCH_API(64, 8)
void benchmark_ofdmframesync_execute_n128   OFDMFRAMESYNC_EXECUTE_BENCH_API(128,16)
void benchmark_ofdmframesync_execute_n256   OFDMFRAMESYNC_EXECUTE_BENCH_API(256,32)
void benchmark_ofdmframesync_execute_n512   OFDMFRAMESYNC_EXECUTE_BENCH_API(512,64)

#endif // __LIQUID_OFDMFRAMESYNC_BENCH_H__

