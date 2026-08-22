/*
 * Copyright (c) 2007 - 2026 Joseph Gaeddert
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "liquid.benchmark.h"
#include <math.h>
#include <assert.h>
#include "liquid.internal.h"

// Helper function to keep code base small
float ofdmframesync_rxsymbol_bench(unsigned long int _num_iterations,
                                   unsigned int      _num_subcarriers,
                                   unsigned int      _cp_len)
{
    // options
    modulation_scheme ms = LIQUID_MODEM_QPSK;
    unsigned int M         = _num_subcarriers;
    unsigned int cp_len    = _cp_len;
    unsigned int taper_len = 0;
    // create synthesizer/analyzer objects
    ofdmframegen fg = ofdmframegen_create(M, cp_len, taper_len, NULL);
    //ofdmframegen_print(fg);
    modemcf mod = modemcf_create(ms);
    ofdmframesync fs = ofdmframesync_create(M,cp_len,taper_len,NULL,NULL,NULL);
    unsigned int i;
    float complex X[M];         // channelized symbol
    float complex x[M+cp_len];  // time-domain symbol
    // synchronize short sequence (first)
    ofdmframegen_write_S0a(fg, x);
    ofdmframesync_execute(fs, x, M+cp_len);
    // synchronize short sequence (second)
    ofdmframegen_write_S0b(fg, x);
    ofdmframesync_execute(fs, x, M+cp_len);
    // synchronize long sequence
    ofdmframegen_write_S1(fg, x);
    ofdmframesync_execute(fs, x, M+cp_len);
    // modulate data symbols (use same symbol, ignore pilot phase)
    unsigned int s;
    for (i=0; i<M; i++) {
        s = modemcf_gen_rand_sym(mod);
        modemcf_modulate(mod,s,&X[i]);
    }
    ofdmframegen_writesymbol(fg, X, x);
    // add noise
    for (i=0; i<M+cp_len; i++)
        x[i] += 0.02f*randnf()*cexpf(_Complex_I*2*M_PI*randf());
    // start trials (4 executes of M+cp_len samples each per iteration; round down)
    unsigned long int n = _num_iterations / 4;
    liquid_timer timer = liquid_timer_create(LIQUID_TIMER_RUSAGE);
    for (i=0; i<n; i++) {
        // receive data symbols (ignoring pilots)
        ofdmframesync_execute(fs, x, M+cp_len);
        ofdmframesync_execute(fs, x, M+cp_len);
        ofdmframesync_execute(fs, x, M+cp_len);
        ofdmframesync_execute(fs, x, M+cp_len);
    }
    float extime = liquid_toc(timer);
    // destroy objects
    ofdmframegen_destroy(fg);
    ofdmframesync_destroy(fs);
    modemcf_destroy(mod);
    return extime;
}

LIQUID_BENCHMARK(ofdmframesync_rxsymbol_n64,  "ofdmframesync rxsymbol, M=64 cp_len=8",  "framing,ofdmframe,rxsymbol")
    { return ofdmframesync_rxsymbol_bench(num_iterations, 64, 8); }
LIQUID_BENCHMARK(ofdmframesync_rxsymbol_n128, "ofdmframesync rxsymbol, M=128 cp_len=16", "framing,ofdmframe,rxsymbol")
    { return ofdmframesync_rxsymbol_bench(num_iterations, 128, 16); }
LIQUID_BENCHMARK(ofdmframesync_rxsymbol_n256, "ofdmframesync rxsymbol, M=256 cp_len=32", "framing,ofdmframe,rxsymbol")
    { return ofdmframesync_rxsymbol_bench(num_iterations, 256, 32); }
LIQUID_BENCHMARK(ofdmframesync_rxsymbol_n512, "ofdmframesync rxsymbol, M=512 cp_len=64", "framing,ofdmframe,rxsymbol")
    { return ofdmframesync_rxsymbol_bench(num_iterations, 512, 64); }

