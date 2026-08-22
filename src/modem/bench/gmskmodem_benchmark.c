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

// TODO: test different parameters
LIQUID_BENCHMARK(gmskmodem_modulate, "gmskmod modulate", "modem,gmsk")
{
    // options
    unsigned int k=2;   // filter samples/symbol
    unsigned int m=3;   // filter delay (symbols)
    float BT=0.3f;      // bandwidth-time product

    // create modem object
    gmskmod mod   = gmskmod_create(k, m, BT);
    float complex x[k];
    unsigned int symbol_in = 0;
    unsigned long int i;
    // start trials (4 modulates per iteration; round down)
    unsigned long int n = num_iterations / 4;
    liquid_timer timer = liquid_timer_create(LIQUID_TIMER_RUSAGE);
    for (i=0; i<n; i++) {
        gmskmod_modulate(mod, symbol_in, x);
        gmskmod_modulate(mod, symbol_in, x);
        gmskmod_modulate(mod, symbol_in, x);
        gmskmod_modulate(mod, symbol_in, x);
    }
    float extime = liquid_toc(timer);
    // destroy modem objects
    gmskmod_destroy(mod);
    return extime;
}

// TODO: test different parameters
LIQUID_BENCHMARK(gmskmodem_demodulate, "gmskdem demodulate", "modem,gmsk")
{
    // options
    unsigned int k=2;   // filter samples/symbol
    unsigned int m=3;   // filter delay (symbols)
    float BT=0.3f;      // bandwidth-time product

    // create modem object
    gmskdem demod = gmskdem_create(k, m, BT);
    float complex x[k];
    unsigned int symbol_out = 0;
    unsigned long int i;
    for (i=0; i<k; i++)
        x[i] = randnf()*cexpf(_Complex_I*2*M_PI*randf());
    // start trials (4 demodulates per iteration; round down)
    unsigned long int n = num_iterations / 4;
    liquid_timer timer = liquid_timer_create(LIQUID_TIMER_RUSAGE);
    for (i=0; i<n; i++) {
        gmskdem_demodulate(demod, x, &symbol_out);
        gmskdem_demodulate(demod, x, &symbol_out);
        gmskdem_demodulate(demod, x, &symbol_out);
        gmskdem_demodulate(demod, x, &symbol_out);
    }
    float extime = liquid_toc(timer);
    // destroy modem objects
    gmskdem_destroy(demod);
    return extime;
}

