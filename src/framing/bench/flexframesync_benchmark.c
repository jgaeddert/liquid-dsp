/*
 * Copyright (c) 2007 - 2014 Joseph Gaeddert
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

#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <assert.h>
#include "liquid.h"

typedef struct {
    unsigned char * header;
    unsigned char * payload;
    unsigned int num_frames_tx;
    unsigned int num_frames_detected;
    unsigned int num_headers_valid;
    unsigned int num_payloads_valid;
} framedata;

static int callback(unsigned char *  _header,
                    int              _header_valid,
                    unsigned char *  _payload,
                    unsigned int     _payload_len,
                    int              _payload_valid,
                    framesyncstats_s _stats,
                    void *           _userdata)
{
    //printf("callback invoked\n");
    framedata * fd = (framedata*) _userdata;

    // increment number of frames detected
    fd->num_frames_detected++;

    // check if header is valid
    if (_header_valid)
        fd->num_headers_valid++;

    // check if payload is valid
    if (_payload_valid)
        fd->num_payloads_valid++;

    return 0;
}

// Helper function to keep code base small
void benchmark_flexframesync(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations)
{
    *_num_iterations /= 128;
    unsigned long int i;

    // create flexframegen object
    flexframegenprops_s fgprops;
    flexframegenprops_init_default(&fgprops);
    fgprops.check      = LIQUID_CRC_32;
    fgprops.fec0       = LIQUID_FEC_NONE;
    fgprops.fec1       = LIQUID_FEC_NONE;
    fgprops.mod_scheme = LIQUID_MODEM_QPSK;
    flexframegen fg = flexframegen_create(&fgprops);
    flexframegen_print(fg);

    // frame data
    unsigned int payload_len = 8;
    unsigned char header[14];
    unsigned char payload[payload_len];
    // initialize header, payload
    for (i=0; i<14; i++)
        header[i] = i;
    for (i=0; i<payload_len; i++)
        payload[i] = rand() & 0xff;
    framedata fd = {header, payload, 0, 0, 0, 0};

    // create flexframesync object
    flexframesync fs = flexframesync_create(callback,(void*)&fd);
    flexframesync_print(fs);

    // generate the frame
    flexframegen_assemble(fg, header, payload, payload_len);
    unsigned int frame_len = flexframegen_getframelen(fg);
    float complex frame[frame_len];
    int frame_complete = 0;
    unsigned int n=0;
    while (!frame_complete) {
        assert(n < frame_len);
        frame_complete = flexframegen_write_samples(fg, &frame[n]);
        n += 2;
    }
    // add some noise
    for (i=0; i<frame_len; i++)
        frame[i] += 0.02f*(randnf() + _Complex_I*randnf());

    // 
    // start trials
    //
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        flexframesync_execute(fs, frame, frame_len);
    }
    getrusage(RUSAGE_SELF, _finish);

    // print results
    fd.num_frames_tx = *_num_iterations;
    printf("  frames detected/header/payload/transmitted:   %6u / %6u / %6u / %6u\n",
            fd.num_frames_detected,
            fd.num_headers_valid,
            fd.num_payloads_valid,
            fd.num_frames_tx);

    // destroy objects
    flexframegen_destroy(fg);
    flexframesync_destroy(fs);
}

