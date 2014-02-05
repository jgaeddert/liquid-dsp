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
#include <math.h>
#include "liquid.h"

typedef struct {
    unsigned int num_frames_tx;         // number of transmitted frames
    unsigned int num_frames_detected;   // number of received frames (detected)
    unsigned int num_frames_valid;      // number of valid payloads
} framedata;

static int callback(unsigned char *  _header,
                    int              _header_valid,
                    unsigned char *  _payload,
                    unsigned int     _payload_len,
                    int              _payload_valid,
                    framesyncstats_s _stats,
                    void *           _userdata)
{
    //printf("callback invoked, payload valid: %s\n", _payload_valid ? "yes" : "no");
    framedata * fd = (framedata*) _userdata;
    fd->num_frames_detected += 1;
    fd->num_frames_valid    += _payload_valid ? 1 : 0;
    return 0;
}

// Helper function to keep code base small
void benchmark_framesync64(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations)
{
    *_num_iterations /= 128;
    unsigned long int i;

    framegen64 fg = framegen64_create();
    framegen64_print(fg);

    // frame data
    unsigned char header[8] = {0, 1, 2, 3, 4, 5, 6, 7};
    unsigned char payload[64];
    // initialize payload
    for (i=0; i<64; i++)
        payload[i] = rand() & 0xff;
    framedata fd = {0, 0, 0};

    // create framesync64 object
    framesync64 fs = framesync64_create(callback,(void*)&fd);
    framesync64_print(fs);

    // generate the frame
    //unsigned int frame_len = framegen64_getframelen(fg);
    unsigned int frame_len = LIQUID_FRAME64_LEN;
    float complex frame[frame_len];
    framegen64_execute(fg, header, payload, frame);

    // add some noise
    for (i=0; i<frame_len; i++)
        frame[i] += 0.01f*(randnf() + _Complex_I*randnf()) * M_SQRT1_2;

    // 
    // start trials
    //
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        framesync64_execute(fs, frame, frame_len);
    }
    getrusage(RUSAGE_SELF, _finish);


    fd.num_frames_tx = *_num_iterations;
    printf("  frames detected/valid/transmitted  :   %6u / %6u / %6u\n",
            fd.num_frames_detected,
            fd.num_frames_valid,
            fd.num_frames_tx);

    framegen64_destroy(fg);
    framesync64_destroy(fs);
}

