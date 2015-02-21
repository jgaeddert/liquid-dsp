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

// 
// AUTOTEST : test simple recovery of frame in noise
//
void autotest_qpacketmodem()
{
    // options
    // TODO: test different combinations
    modulation_scheme ms     = LIQUID_MODEM_QPSK;       // mod. scheme
    crc_scheme check         = LIQUID_CRC_32;           // data validity check
    fec_scheme fec0          = LIQUID_FEC_GOLAY2412;    // fec (inner)
    fec_scheme fec1          = LIQUID_FEC_NONE;         // fec (outer)
    unsigned int payload_len = 400;                     // payload length

    // derived values
    unsigned int i;

    // create and configure packet encoder/decoder object
    qpacketmodem q = qpacketmodem_create();
    qpacketmodem_configure(q, payload_len, check, fec0, fec1, ms);

    // initialize payload
    unsigned char payload_tx[payload_len];
    unsigned char payload_rx[payload_len];

    // initialize payload
    for (i=0; i<payload_len; i++) {
        payload_tx[i] = rand() & 0xff;
        payload_rx[i] = 0x00;
    }

    // get frame length
    unsigned int frame_len = qpacketmodem_get_frame_len(q);

    // allocate memory for frame samples
    float complex frame[frame_len];

    // encode frame
    qpacketmodem_encode(q, payload_tx, frame);

    // decode frame
    int crc_pass = qpacketmodem_decode(q, frame, payload_rx);

    // destroy object
    qpacketmodem_destroy(q);

    // check to see that frame was recovered
    CONTEND_EQUALITY( crc_pass, 1 );
    CONTEND_SAME_DATA( payload_tx, payload_rx, payload_len );
}

