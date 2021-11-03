/*
 * Copyright (c) 2007 - 2021 Joseph Gaeddert
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

static int gmskframesync_autotest_callback(
    unsigned char *  _header,
    int              _header_valid,
    unsigned char *  _payload,
    unsigned int     _payload_len,
    int              _payload_valid,
    framesyncstats_s _stats,
    void *           _userdata)
{
    unsigned int * secret = (unsigned int*) _userdata;
    *secret = 0x01234567;
    return 0;
}

// test simple recovery of GMSK frame
void autotest_gmskframesync()
{
    // initialization and options
    unsigned int k      = 2;        // samples per symbol
    unsigned int m      = 12;       // filter semi-length
    float        bt     = 0.3f;     // bandwidth-time factor
    unsigned int msg_len= 40;       // message length [bytes]
    crc_scheme   crc    = LIQUID_CRC_32;
    fec_scheme   fec0   = LIQUID_FEC_NONE;
    fec_scheme   fec1   = LIQUID_FEC_NONE;
    unsigned int secret = 0;        //

    // create and assemble frame generator
    gmskframegen fg = gmskframegen_create(k,m,bt);
    gmskframegen_assemble(fg, NULL, NULL, msg_len, crc, fec0, fec1);

    // create frame synchronizer
    gmskframesync fs = gmskframesync_create(k,m,bt,
            gmskframesync_autotest_callback,(void*)&secret);

    if (liquid_autotest_verbose) {
        gmskframegen_print(fg);
        gmskframesync_print(fs);
    }

    // allocate buffer (irregular size to test write method)
    unsigned int  buf_len = 53;
    float complex buf[buf_len];

    // generate the frame in blocks
    unsigned int i;
    int frame_complete = 0;
    while (!frame_complete) {
        frame_complete = gmskframegen_write(fg, buf, buf_len);
        gmskframesync_execute(fs, buf, buf_len);
    }

    // check to see that frame was recovered
    CONTEND_EQUALITY( secret, 0x01234567 );

    // parse statistics
    framedatastats_s stats = gmskframesync_get_framedatastats(fs);
    CONTEND_EQUALITY(stats.num_frames_detected, 1);
    CONTEND_EQUALITY(stats.num_headers_valid,   1);
    CONTEND_EQUALITY(stats.num_payloads_valid,  1);
    CONTEND_EQUALITY(stats.num_bytes_received,  msg_len);

    // destroy objects
    gmskframegen_destroy(fg);
    gmskframesync_destroy(fs);
}

