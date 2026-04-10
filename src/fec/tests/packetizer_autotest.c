/*
 * Copyright (c) 2007 - 2026 Joseph Gaeddert
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

#include "liquid.autotest.h"
#include "liquid.h"

// Help function to keep code base small
void testbench_packetizer(liquid_autotest __q__,
                          unsigned int _n,
                          crc_scheme _crc,
                          fec_scheme _fec0,
                          fec_scheme _fec1)
{
    LIQUID_VLA(unsigned char, msg_tx, _n);
    LIQUID_VLA(unsigned char, msg_rx, _n);
    unsigned int pkt_len = packetizer_compute_enc_msg_len(_n,_crc,_fec0,_fec1);
    LIQUID_VLA(unsigned char, packet, pkt_len);

    // create object
    packetizer p = packetizer_create(_n,_crc,_fec0,_fec1);

    // initialize data
    unsigned int i;
    for (i=0; i<_n; i++) {
        msg_tx[i] = i % 256;
        msg_rx[i] = 0;
    }

    // encode/decode packet
    packetizer_encode(p, msg_tx, packet);
    int crc_pass = packetizer_decode(p, packet, msg_rx);

    LIQUID_CHECK_ARRAY(msg_tx, msg_rx, _n);
    LIQUID_CHECK(crc_pass ==  1);

    // clean up objects
    packetizer_destroy(p);
}

//
LIQUID_AUTOTEST(packetizer_n16_0_0,"","",0.1)  { testbench_packetizer(__q__, 16, LIQUID_CRC_32, LIQUID_FEC_NONE, LIQUID_FEC_NONE);       }
LIQUID_AUTOTEST(packetizer_n16_0_1,"","",0.1)  { testbench_packetizer(__q__, 16, LIQUID_CRC_32, LIQUID_FEC_NONE, LIQUID_FEC_REP3);       }
LIQUID_AUTOTEST(packetizer_n16_0_2,"","",0.1)  { testbench_packetizer(__q__, 16, LIQUID_CRC_32, LIQUID_FEC_NONE, LIQUID_FEC_HAMMING74);  }

