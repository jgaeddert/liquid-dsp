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

#include <stdlib.h>

#include "autotest/autotest.h"
#include "liquid.h"

static int bpacketsync_autotest_callback(unsigned char *  _payload,
                                         int              _payload_valid,
                                         unsigned int     _payload_len,
                                         framesyncstats_s _stats,
                                         void *           _userdata)
{
    if (!_payload_valid)
        return 0;

    unsigned int * num_packets_found = (unsigned int *) _userdata;

    *num_packets_found += 1;

    return 0;
}

// 
// AUTOTEST: bpacketsync
//
void autotest_bpacketsync()
{
    // options
    unsigned int num_packets = 50;          // number of packets to encode
    unsigned int dec_msg_len = 64;          // original data message length
    crc_scheme check = LIQUID_CRC_32;       // data integrity check
    fec_scheme fec0 = LIQUID_FEC_HAMMING74; // inner code
    fec_scheme fec1 = LIQUID_FEC_NONE;      // outer code

    // create packet generator
    bpacketgen pg = bpacketgen_create(0, dec_msg_len, check, fec0, fec1);
    if (liquid_autotest_verbose)
        bpacketgen_print(pg);

    // compute packet length
    unsigned int enc_msg_len = bpacketgen_get_packet_len(pg);

    // initialize arrays
    unsigned char msg_org[dec_msg_len]; // original message
    unsigned char msg_enc[enc_msg_len]; // encoded message

    unsigned int num_packets_found=0;

    // create packet synchronizer
    bpacketsync ps = bpacketsync_create(0, bpacketsync_autotest_callback, (void*)&num_packets_found);

    unsigned int i;
    unsigned int n;
    for (n=0; n<num_packets; n++) {
        // initialize original data message
        for (i=0; i<dec_msg_len; i++)
            msg_org[i] = rand() % 256;

        // encode packet
        bpacketgen_encode(pg,msg_org,msg_enc);

        // push packet through synchronizer
        bpacketsync_execute(ps, msg_enc, enc_msg_len);
    }

    // count number of packets
    if (liquid_autotest_verbose)
        printf("found %u / %u packets\n", num_packets_found, num_packets);

    CONTEND_EQUALITY( num_packets_found, num_packets );

    // clean up allocated objects
    bpacketgen_destroy(pg);
    bpacketsync_destroy(ps);
}

