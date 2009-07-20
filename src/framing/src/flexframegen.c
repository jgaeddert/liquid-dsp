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

//
// flexible frame generator
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <complex.h>

#include "liquid.internal.h"

#define DEBUG_FLEXFRAMEGEN

struct flexframegen_s {
    // buffers: preamble (BPSK)
    float complex * ramp_up;
    float complex * phasing;
    float complex   pn_sequence[64];
    float complex * ramp_dn;

    // header (QPSK)
    modem mod_header;
    fec fec_header;
    interleaver intlv_header;
    unsigned char header[15];
    unsigned char header_enc[32];
    unsigned char header_sym[128];

    // payload
    modem mod_payload;
    unsigned char * payload;
    unsigned char * payload_sym;

    // properties
    flexframegenprops_s props;
};

flexframegen flexframegen_create(flexframegenprops_s * _props)
{
    flexframegen fg = (flexframegen) malloc(sizeof(struct flexframegen_s));

    unsigned int i;

    // generate pn sequence
    msequence ms = msequence_create(6);
    for (i=0; i<64; i++)
        fg->pn_sequence[i] = (msequence_advance(ms)) ? 1.0f : -1.0f;
    msequence_destroy(ms);

    // create header objects
    fg->mod_header = modem_create(MOD_QPSK, 2);
    fg->fec_header = fec_create(FEC_CONV_V29, NULL);
    fg->intlv_header = interleaver_create(32, INT_BLOCK);

    // initialize

    return fg;
}

void flexframegen_destroy(flexframegen _fg)
{
    fec_destroy(_fg->fec_header);
    interleaver_destroy(_fg->intlv_header);
    modem_destroy(_fg->mod_header);
    free(_fg);
}

void flexframegen_print(flexframegen _fg)
{
    printf("flexframegen:\n");
    printf("    ramp up len     :   %u\n", _fg->props.rampup_len);
}

unsigned int flexframegen_getframelen(flexframegen _fg)
{
    return 0;
}

void flexframegen_execute(flexframegen _fg,
                          unsigned char * _header,
                          unsigned char * _payload,
                          float complex * _y)
{
}

