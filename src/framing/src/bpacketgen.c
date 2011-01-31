/*
 * Copyright (c) 2011 Joseph Gaeddert
 * Copyright (c) 2011 Virginia Polytechnic Institute & State University
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

//
// bpacketgen
//
// binary packet generator/encoder
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "liquid.internal.h"

// bpacketgen object structure
struct bpacketgen_s {
    //
    

    // bpacket header
    //  0   :   version number
    //  1   :   fec0
    //  2   :   fec1
    //  3:4 :   payload length
    //  ...
    unsigned char header_dec[8];
    unsigned char header_enc[16];

    //
    packetizer p_header;
    packetizer p_payload;
};


//
bpacketgen bpacketgen_create(unsigned int _m,
                             unsigned int _dec_msg_len,
                             int _crc,
                             int _fec0,
                             int _fec1)
{
    bpacketgen q = (bpacketgen) malloc(sizeof(struct bpacketgen_s));

    return q;
}

void bpacketgen_destroy(bpacketgen _q)
{
    free(_q);
}

void bpacketgen_print(bpacketgen _q)
{
    printf("bpacketgen:\n");
}

unsigned int bpacketgen_get_packet_len(bpacketgen _q)
{
    return 0;
}

void bpacketgen_encode(bpacketgen _q,
                       unsigned char * _msg_dec,
                       unsigned char * _packet)
{
}


