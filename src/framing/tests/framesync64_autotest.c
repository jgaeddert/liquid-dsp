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

    if (_header_valid && _payload_valid)
        *frame_recovered = 1;
    else
        *frame_recovered = 0;
    
    return 0;
}

// 
// AUTOTEST : test simple recovery of frame in noise
//
void autotest_framesync64()
{
    unsigned int i;

    framegen64 fg = framegen64_create();

    // frame data
    unsigned char header[8] = {0, 1, 2, 3, 4, 5, 6, 7};
    unsigned char payload[64];
    // initialize payload
    for (i=0; i<64; i++)
        payload[i] = rand() & 0xff;
    
    // create framesync64 object
    int frame_recovered = 0;
    framesync64 fs = framesync64_create(callback,(void*)&frame_recovered);
    
    if (liquid_autotest_verbose) {
        framesync64_print(fs);
        framegen64_print(fg);
    }

    // generate the frame
    unsigned int frame_len = LIQUID_FRAME64_LEN;
    float complex frame[frame_len];
    framegen64_execute(fg, header, payload, frame);

    // add some noise
    for (i=0; i<frame_len; i++)
        frame[i] += 0.01f*(randnf() + _Complex_I*randnf()) * M_SQRT1_2;

    // try to find the frame
    framesync64_execute(fs, frame, frame_len);

    // check to see that frame was recovered
    CONTEND_EQUALITY( frame_recovered, 1 );

    // destroy objects
    framegen64_destroy(fg);
    framesync64_destroy(fs);
}

