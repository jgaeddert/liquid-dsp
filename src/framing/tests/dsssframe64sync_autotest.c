/*
 * Copyright (c) 2007 - 2024 Joseph Gaeddert
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "autotest/autotest.h"
#include "liquid.h"

// AUTOTEST : test simple recovery of frame in noise
void autotest_dsssframe64sync()
{
    // create objects
    dsssframe64gen  fg = dsssframe64gen_create();
    dsssframe64sync fs = dsssframe64sync_create(NULL, NULL);

    // generate the frame
    unsigned int frame_len = dsssframe64gen_get_frame_len(fg);
    float complex * frame = (float complex *)malloc(frame_len*sizeof(float complex));
    dsssframe64gen_execute(fg, NULL, NULL, frame);

    // add some noise
    unsigned int i;
    for (i=0; i<frame_len; i++)
        frame[i] += 1.0f*(randnf() + _Complex_I*randnf()) * M_SQRT1_2;

    // try to receive the frame
    dsssframe64sync_execute(fs, frame, frame_len);

    // parse statistics
    framedatastats_s stats = dsssframe64sync_get_framedatastats(fs);
    CONTEND_EQUALITY(stats.num_frames_detected, 1);
    CONTEND_EQUALITY(stats.num_headers_valid,   1);
    CONTEND_EQUALITY(stats.num_payloads_valid,  1);
    CONTEND_EQUALITY(stats.num_bytes_received, 64);

    // destroy objects and free memory
    dsssframe64gen_destroy(fg);
    dsssframe64sync_destroy(fs);
    free(frame);
}

