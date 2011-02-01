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
// bpacketsync
//
// binary packet synchronizer/decoder
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "liquid.internal.h"

// bpacketsync object structure
struct bpacketsync_s {
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

bpacketsync bpacketsync_create(unsigned int _m,
                               bpacketsync_callback _callback,
                               void * _userdata)
{
    bpacketsync q = (bpacketsync) malloc(sizeof(struct bpacketsync_s));

    return q;
}

void bpacketsync_destroy(bpacketsync _q)
{
    free(_q);
}

void bpacketsync_print(bpacketsync _q)
{
}

void bpacketsync_execute(bpacketsync _q,
                         unsigned char _b)
{
}


