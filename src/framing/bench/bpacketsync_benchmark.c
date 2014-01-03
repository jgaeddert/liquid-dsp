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
#include <string.h>
#include <sys/resource.h>

#include "liquid.internal.h"

// callback function
static int bpacketsync_benchmark_callback(unsigned char *  _payload,
                                          int              _payload_valid,
                                          unsigned int     _payload_len,
                                          framesyncstats_s _stats,
                                          void *           _userdata)
{
    if (!_payload_valid)
        return 0;

    // increment number of packets found and return
    unsigned int * num_packets_found = (unsigned int *) _userdata;
    *num_packets_found += 1;
    return 0;
}

void benchmark_bpacketsync(struct rusage *_start,
                           struct rusage *_finish,
                           unsigned long int *_num_iterations)
{
    // adjust number of iterations
    *_num_iterations *= 4;

    // options
    unsigned int dec_msg_len = 64;      // original data message length
    crc_scheme check = LIQUID_CRC_NONE; // data integrity check
    fec_scheme fec0 = LIQUID_FEC_NONE;  // inner code
    fec_scheme fec1 = LIQUID_FEC_NONE;  // outer code

    // create packet generator
    bpacketgen pg = bpacketgen_create(0, dec_msg_len, check, fec0, fec1);

    // compute packet length
    unsigned int enc_msg_len = bpacketgen_get_packet_len(pg);

    // initialize arrays
    unsigned char msg_org[dec_msg_len]; // original message
    unsigned char msg_enc[enc_msg_len]; // encoded message

    unsigned int num_packets_found=0;

    // create packet synchronizer
    bpacketsync ps = bpacketsync_create(0, bpacketsync_benchmark_callback, (void*)&num_packets_found);

    unsigned long int i;
    // initialize original data message
    for (i=0; i<dec_msg_len; i++)
        msg_org[i] = rand() % 256;

    // encode packet
    bpacketgen_encode(pg,msg_org,msg_enc);

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        // push packet through synchronizer
        bpacketsync_execute_byte(ps, msg_enc[(4*i+0)%enc_msg_len]);
        bpacketsync_execute_byte(ps, msg_enc[(4*i+1)%enc_msg_len]);
        bpacketsync_execute_byte(ps, msg_enc[(4*i+2)%enc_msg_len]);
        bpacketsync_execute_byte(ps, msg_enc[(4*i+3)%enc_msg_len]);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;

    printf("found %u packets\n", num_packets_found);

    // clean up allocated objects
    bpacketgen_destroy(pg);
    bpacketsync_destroy(ps);
}

