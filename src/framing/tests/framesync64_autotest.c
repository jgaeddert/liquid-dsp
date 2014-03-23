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

