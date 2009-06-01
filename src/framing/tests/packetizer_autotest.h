/*
 * Copyright (c) 2007, 2009 Joseph Gaeddert
 * Copyright (c) 2007, 2009 Virginia Polytechnic Institute & State University
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

#ifndef __PACKETIZER_AUTOTEST_H__
#define __PACKETIZER_AUTOTEST_H__

#include "autotest/autotest.h"
#include "liquid.h"

// Help function to keep code base small
void packetizer_test_codec(unsigned int _n, fec_scheme _fec0, fec_scheme _fec1)
{
    unsigned char msg_tx[_n];
    unsigned char msg_rx[_n];
    unsigned int pkt_len = packetizer_get_packet_length(_n,_fec0,_fec1);
    unsigned char packet[pkt_len];

    // create object
    packetizer p = packetizer_create(_n,_fec0,_fec1);

    if (_autotest_verbose)
        packetizer_print(p);

    // initialize data
    unsigned int i;
    for (i=0; i<_n; i++) {
        msg_tx[i] = i % 256;
        msg_rx[i] = 0;
    }

    bool crc_pass;

    // encode/decode packet
    packetizer_encode(p, msg_tx, packet);
    crc_pass = packetizer_decode(p, packet, msg_rx);

    CONTEND_SAME_DATA(msg_tx, msg_rx, _n);
    CONTEND_EQUALITY(crc_pass, true);

    // clean up objects
    packetizer_destroy(p);
}

const fec_scheme ft[3] = {
    FEC_NONE,
    FEC_REP3,
    FEC_HAMMING74
};

//
// AUTOTESTS
//
void autotest_packetizer_n16_0_0()  { packetizer_test_codec(16,ft[0],ft[0]); }
void autotest_packetizer_n16_0_1()  { packetizer_test_codec(16,ft[0],ft[1]); }
void autotest_packetizer_n16_0_2()  { packetizer_test_codec(16,ft[0],ft[2]); }

void autotest_packetizer_n16_1_0()  { packetizer_test_codec(16,ft[1],ft[0]); }
void autotest_packetizer_n16_1_1()  { packetizer_test_codec(16,ft[1],ft[1]); }
void autotest_packetizer_n16_1_2()  { packetizer_test_codec(16,ft[1],ft[2]); }

void autotest_packetizer_n16_2_0()  { packetizer_test_codec(16,ft[2],ft[0]); }
void autotest_packetizer_n16_2_1()  { packetizer_test_codec(16,ft[2],ft[1]); }
void autotest_packetizer_n16_2_2()  { packetizer_test_codec(16,ft[2],ft[2]); }

#endif 

