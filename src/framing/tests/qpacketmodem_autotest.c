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
void qpacketmodem_test(unsigned int _payload_len,
                       int          _check,
                       int          _fec0,
                       int          _fec1,
                       int          _ms)
{
    // derived values
    unsigned int i;

    // create and configure packet encoder/decoder object
    qpacketmodem q = qpacketmodem_create();
    qpacketmodem_configure(q, _payload_len, _check, _fec0, _fec1, _ms);
    if (liquid_autotest_verbose)
        qpacketmodem_print(q);

    // initialize payload
    unsigned char payload_tx[_payload_len];
    unsigned char payload_rx[_payload_len];

    // initialize payload
    for (i=0; i<_payload_len; i++) {
        payload_tx[i] = rand() & 0xff;
        payload_rx[i] = rand() & 0xff;
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
    CONTEND_SAME_DATA( payload_tx, payload_rx, _payload_len );
}

void autotest_qpacketmodem_bpsk()   { qpacketmodem_test(400,LIQUID_CRC_32,LIQUID_FEC_NONE,LIQUID_FEC_NONE, LIQUID_MODEM_QPSK);    }
void autotest_qpacketmodem_qpsk()   { qpacketmodem_test(400,LIQUID_CRC_32,LIQUID_FEC_NONE,LIQUID_FEC_NONE, LIQUID_MODEM_QPSK);    }
void autotest_qpacketmodem_psk8()   { qpacketmodem_test(400,LIQUID_CRC_32,LIQUID_FEC_NONE,LIQUID_FEC_NONE, LIQUID_MODEM_PSK8);    }
void autotest_qpacketmodem_qam16()  { qpacketmodem_test(400,LIQUID_CRC_32,LIQUID_FEC_NONE,LIQUID_FEC_NONE, LIQUID_MODEM_QAM16);   }
void autotest_qpacketmodem_sqam32() { qpacketmodem_test(400,LIQUID_CRC_32,LIQUID_FEC_NONE,LIQUID_FEC_NONE, LIQUID_MODEM_SQAM32);  }
void autotest_qpacketmodem_qam64()  { qpacketmodem_test(400,LIQUID_CRC_32,LIQUID_FEC_NONE,LIQUID_FEC_NONE, LIQUID_MODEM_QAM64);   }
void autotest_qpacketmodem_sqam128(){ qpacketmodem_test(400,LIQUID_CRC_32,LIQUID_FEC_NONE,LIQUID_FEC_NONE, LIQUID_MODEM_SQAM128); }
void autotest_qpacketmodem_qam256() { qpacketmodem_test(400,LIQUID_CRC_32,LIQUID_FEC_NONE,LIQUID_FEC_NONE, LIQUID_MODEM_QAM256);  }

