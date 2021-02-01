/*
 * Copyright (c) 2007 - 2015 Joseph Gaeddert
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
#include <math.h>
#include "autotest/autotest.h"
#include "liquid.h"

static int callback(unsigned char *  _header,
                    int              _header_valid,
                    unsigned char *  _payload,
                    unsigned int     _payload_len,
                    int              _payload_valid,
                    framesyncstats_s _stats,
                    void *           _userdata)
{
    //printf("callback invoked, payload valid: %s\n", _payload_valid ? "yes" : "no");
    int * frame_recovered = (int*) _userdata;

    *frame_recovered = _header_valid && _payload_valid;
    return 0;
}

// 
// AUTOTEST : test simple recovery of frame in noise
//
void autotest_framesync64()
{
    unsigned int i;
    int frame_recovered = 0;

    // create objects
    framegen64 fg = framegen64_create();
    framesync64 fs = framesync64_create(callback,(void*)&frame_recovered);
    
    if (liquid_autotest_verbose) {
        framesync64_print(fs);
        framegen64_print(fg);
    }

    // generate the frame
    float complex frame[LIQUID_FRAME64_LEN];
    framegen64_execute(fg, NULL, NULL, frame);

    // add some noise
    for (i=0; i<LIQUID_FRAME64_LEN; i++)
        frame[i] += 0.01f*(randnf() + _Complex_I*randnf()) * M_SQRT1_2;

    // try to receive the frame
    framesync64_execute(fs, frame, LIQUID_FRAME64_LEN);

    // check to see that frame was recovered
    CONTEND_EQUALITY( frame_recovered, 1 );

    // parse statistics
    framedatastats_s stats = framesync64_get_framedatastats(fs);
    CONTEND_EQUALITY(stats.num_frames_detected, 1);
    CONTEND_EQUALITY(stats.num_headers_valid,   1);
    CONTEND_EQUALITY(stats.num_payloads_valid,  1);
    CONTEND_EQUALITY(stats.num_bytes_received, 64);

    // destroy objects
    framegen64_destroy(fg);
    framesync64_destroy(fs);
}

