/*
 * Copyright (c) 2007 - 2015 Joseph Gaeddert
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
    if (liquid_autotest_verbose)
        printf("*** flexframesync callback invoked, payload valid: %s ***\n", _payload_valid ? "yes" : "no");

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
void autotest_flexframesync()
{
    unsigned int i;

    unsigned int _payload_len = 400;
    int _ms    = LIQUID_MODEM_QPSK;
    int _fec0  = LIQUID_FEC_NONE;
    int _fec1  = LIQUID_FEC_NONE;
    int _check = LIQUID_CRC_32;

    // create flexframegen object
    flexframegenprops_s fgprops;
    flexframegenprops_init_default(&fgprops);
    fgprops.mod_scheme  = _ms;
    fgprops.check       = _check;
    fgprops.fec0        = _fec0;
    fgprops.fec1        = _fec1;
    flexframegen fg = flexframegen_create(&fgprops);

    // create flexframesync object
    int frame_recovered = 0;
    flexframesync fs = flexframesync_create(callback,(void*)&frame_recovered);
    
    // initialize header and payload
    unsigned char header[14] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
    unsigned char payload[_payload_len];
    for (i=0; i<_payload_len; i++)
        payload[i] = rand() & 0xff;
    
    // assemble the frame
    flexframegen_assemble(fg, header, payload, _payload_len);
    if (liquid_autotest_verbose)
        flexframegen_print(fg);

    // generate the frame
    int frame_complete = 0;
    float complex buf[2];
    while (!frame_complete) {
        // write samples to buffer
        frame_complete = flexframegen_write_samples(fg, buf);

        // run through frame synchronizer
        flexframesync_execute(fs, buf, 2);
    }

    // check to see that frame was recovered
    CONTEND_EQUALITY( frame_recovered, 1 );

    // destroy objects
    flexframegen_destroy(fg);
    flexframesync_destroy(fs);
}

