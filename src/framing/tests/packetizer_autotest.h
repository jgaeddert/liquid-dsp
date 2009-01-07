#ifndef __PACKETIZER_AUTOTEST_H__
#define __PACKETIZER_AUTOTEST_H__

#include "../../../autotest/autotest.h"
#include "../src/framing.h"

// Help function to keep code base small
void packetizer_test_codec(unsigned int _n, fec_scheme _fec0, fec_scheme _fec1)
{
    unsigned char msg_tx[_n];
    unsigned char msg_rx[_n];
    packetizer p = packetizer_create(_n,_fec0,_fec1);
    unsigned int pkt_len = packetizer_get_packet_length(p);
    unsigned char packet[pkt_len];

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

