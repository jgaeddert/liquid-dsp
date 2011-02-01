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
// gmskframegen.c
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <complex.h>

#include "liquid.internal.h"

#define DEBUG_GMSKFRAMEGEN    0

// gmskframe object structure
struct gmskframegen_s {
    gmskmod mod;            // GMSK modulator
    packetizer p_header;    // header packetizer
    packetizer p_payload;   // payload packetizer

    // header
    unsigned char header_enc[21];   // 21 = 12 bytes, crc16, h128
    unsigned char header_sym[84];   // 84 = 21*4

    // payload
    unsigned char payload_enc[99];  // 99 = 64 bytes, crc16, h128
    unsigned char payload_sym[396]; // 396 = 99*4
};

// create gmskframegen object
gmskframegen gmskframegen_create(unsigned int _k,
                                 unsigned int _m,
                                 float _BT)
{
    gmskframegen fg = (gmskframegen) malloc(sizeof(struct gmskframegen_s));

    // create modulator
    fg->mod = gmskmod_create(_k, _m, _BT);

    return fg;
}

// destroy gmskframegen object
void gmskframegen_destroy(gmskframegen _fg)
{
    // destroy gmsk modulator
    gmskmod_destroy(_fg->mod);

    // free main object memory
    free(_fg);
}

// print gmskframegen object internals
void gmskframegen_print(gmskframegen _fg)
{
    printf("gmskframegen:\n");
}

// execute frame generator (creates a frame)
//  _fg         :   frame generator object
//  _header     :   12-byte input header
//  _payload    :   64-byte input payload
//  _y          :   1280-sample frame
void gmskframegen_execute(gmskframegen _fg,
                        unsigned char * _header,
                        unsigned char * _payload,
                        float complex * _y)
{
}

