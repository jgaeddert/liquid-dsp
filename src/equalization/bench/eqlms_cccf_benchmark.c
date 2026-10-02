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

// Helper function to keep code base small
float eqlms_cccf_train_bench(unsigned long int num_iterations, unsigned int _h_len)
{
    eqlms_cccf eq = eqlms_cccf_create(NULL,_h_len);
    
    unsigned long int i;

    // set up initial arrays to 'randomize' inputs/outputs
    float complex y[11];
    for (i=0; i<11; i++)
        y[i] = randnf() + _Complex_I*randnf();

    float complex d[13];
    for (i=0; i<13; i++)
        d[i] = randnf() + _Complex_I*randnf();

    unsigned int iy=0;
    unsigned int id=0;

    float complex z;

    // start trials (1 push/execute/step per iteration)
    liquid_timer timer = liquid_timer_create(LIQUID_TIMER_RUSAGE);
    for (i=0; i<num_iterations; i++) {
        eqlms_cccf_push(eq, y[iy]);     // push input into equalizer
        eqlms_cccf_execute(eq, &z);     // compute equalizer output
        eqlms_cccf_step(eq, d[id], z);  // step equalizer internals

        // update counters
        iy = (iy+1)%11;
        id = (id+1)%13;
    }
    float extime = liquid_toc(timer);
    eqlms_cccf_destroy(eq);
    return extime;
}

LIQUID_BENCHMARK(eqlms_cccf_n4,  "eqlms_cccf train, h_len=4",  "equalization,eqlms")
    { return eqlms_cccf_train_bench(num_iterations, 4); }

LIQUID_BENCHMARK(eqlms_cccf_n8,  "eqlms_cccf train, h_len=8",  "equalization,eqlms")
    { return eqlms_cccf_train_bench(num_iterations, 8); }

LIQUID_BENCHMARK(eqlms_cccf_n16, "eqlms_cccf train, h_len=16", "equalization,eqlms")
    { return eqlms_cccf_train_bench(num_iterations, 16); }

LIQUID_BENCHMARK(eqlms_cccf_n32, "eqlms_cccf train, h_len=32", "equalization,eqlms")
    { return eqlms_cccf_train_bench(num_iterations, 32); }

LIQUID_BENCHMARK(eqlms_cccf_n64, "eqlms_cccf train, h_len=64", "equalization,eqlms")
    { return eqlms_cccf_train_bench(num_iterations, 64); }

