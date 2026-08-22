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

// benchmark regular frame synchronizer with short frames; effectively
// test acquisition complexity
LIQUID_BENCHMARK(gmskframesync, "gmskframesync execute", "framing,gmskframe")
{
    // options
    unsigned int k = 2;                 // samples/symbol
    unsigned int m = 3;                 // filter delay (symbols)
    float BT = 0.5f;                    // filter bandwidth-time product
    unsigned int payload_len = 8;       // length of payload (bytes)
    float SNRdB = 30.0f;                // SNR

    // derived values
    float nstd  = powf(10.0f, -SNRdB/20.0f);

    unsigned long int i;

    // create gmskframegen object and assemble the frame
    gmskframegen fg = gmskframegen_create_set(k, m, BT);
    gmskframegen_assemble(fg, NULL, NULL, payload_len,
            LIQUID_CRC_NONE, LIQUID_FEC_NONE, LIQUID_FEC_NONE);
    unsigned int frame_len = gmskframegen_getframelen(fg);
    float complex frame[frame_len];
    gmskframegen_write(fg, frame, frame_len);
    // add some noise
    for (i=0; i<frame_len; i++)
        frame[i] += nstd*(randnf() + _Complex_I*randnf());

    // create gmskframesync object
    gmskframesync fs = gmskframesync_create_set(k, m, BT, NULL, NULL);

    // start trials (1 frame execute per iteration)
    liquid_timer timer = liquid_timer_create(LIQUID_TIMER_RUSAGE);
    for (i=0; i<num_iterations; i++)
        gmskframesync_execute(fs, frame, frame_len);
    float extime = liquid_toc(timer);
    // destroy objects
    gmskframegen_destroy(fg);
    gmskframesync_destroy(fs);
    return extime;
}

// benchmark regular frame synchronizer with noise; essentially test
// complexity when no signal is present
LIQUID_BENCHMARK(gmskframesync_noise, "gmskframesync execute (noise only)", "framing,gmskframe,noise")
{
    // create frame synchronizer
    gmskframesync fs = gmskframesync_create_set(2, 3, 0.5f, NULL, NULL);
    // allocate memory for noise buffer and initialize
    unsigned int num_samples = 1024;
    float complex y[num_samples];
    unsigned long int i;
    for (i=0; i<num_samples; i++)
        y[i] = 0.01f*(randnf() + randnf()*_Complex_I)*M_SQRT1_2;
    // start trials (num_samples samples per iteration; round down)
    unsigned long int n = num_iterations / num_samples;
    liquid_timer timer = liquid_timer_create(LIQUID_TIMER_RUSAGE);
    for (i=0; i<n; i++) {
        // push samples through synchronizer
        gmskframesync_execute(fs, y, num_samples);
    }
    float extime = liquid_toc(timer);
    // destroy framing objects
    gmskframesync_destroy(fs);
    return extime;
}

