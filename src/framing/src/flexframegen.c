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

#define DEBUG_FLEXFRAMEGEN          1
#define DEBUG_FLEXFRAMEGEN_PRINT    1

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

    unsigned int pnsequence_len;    // p/n sequence length
    unsigned int payload_len;       // number of symbols
    unsigned int frame_len;         // number of symbols
};

flexframegen flexframegen_create(flexframegenprops_s * _props)
{
    flexframegen fg = (flexframegen) malloc(sizeof(struct flexframegen_s));

    unsigned int i;

    // generate pn sequence
    fg->pnsequence_len = 64;
    msequence ms = msequence_create(6);
    for (i=0; i<64; i++)
        fg->pn_sequence[i] = (msequence_advance(ms)) ? 1.0f : -1.0f;
    msequence_destroy(ms);

    // create header objects
    fg->mod_header = modem_create(MOD_QPSK, 2);
    fg->fec_header = fec_create(FEC_CONV_V29, NULL);
    fg->intlv_header = interleaver_create(32, INT_BLOCK);

    // initialize
    memmove(&fg->props, _props, sizeof(flexframegenprops_s));
    flexframegen_compute_payload_len(fg);
    flexframegen_compute_frame_len(fg);

    return fg;
}

void flexframegen_destroy(flexframegen _fg)
{
    // destroy header objects
    fec_destroy(_fg->fec_header);
    interleaver_destroy(_fg->intlv_header);
    modem_destroy(_fg->mod_header);

    // destroy frame generator
    free(_fg);
}

void flexframegen_print(flexframegen _fg)
{
    printf("flexframegen [%u]:\n", _fg->frame_len);
    printf("    ramp up len         :   %u\n", _fg->props.rampup_len);
    printf("    phasing len         :   %u\n", _fg->props.phasing_len);
    printf("    p/n sequence len    :   %u\n", _fg->pnsequence_len);
    printf("    payload len         :   %u\n", _fg->payload_len);
    printf("    ramp dn len         :   %u\n", _fg->props.rampdn_len);
}

unsigned int flexframegen_getframelen(flexframegen _fg)
{
    return _fg->frame_len;
}

void flexframegen_execute(flexframegen _fg,
                          unsigned char * _header,
                          unsigned char * _payload,
                          float complex * _y)
{
    // flexframegen_encode_header(_fg);
}

//
// internal
//

void flexframegen_compute_payload_len(flexframegen _fg)
{
    _fg->payload_len = 8*(_fg->props.payload_len);
    _fg->payload_len /= _fg->props.mod_bps;
    _fg->payload_len += _fg->payload_len % _fg->props.mod_bps;
}

void flexframegen_compute_frame_len(flexframegen _fg)
{
    _fg->frame_len = 0;

    _fg->frame_len += _fg->props.rampup_len;    // ramp up length
    _fg->frame_len += _fg->props.phasing_len;   // phasing length
    _fg->frame_len += _fg->pnsequence_len;      // p/n sequence length

    // payload length
    flexframegen_compute_payload_len(_fg);
    _fg->frame_len += _fg->payload_len;

    _fg->frame_len += _fg->props.rampdn_len;    // ramp down length
}

void flexframegen_encode_header(flexframegen _fg,
                                unsigned char * _user_header)
{
    unsigned int i;
    // copy 8 bytes of user data
    for (i=0; i<8; i++)
        _fg->header[i] = _user_header[i];

    // add payload length
    _fg->header[8] = (_fg->props.payload_len >> 8) & 0xff;
    _fg->header[9] = (_fg->props.payload_len     ) & 0xff;

    // add modulation scheme/depth (pack into single byte)
    _fg->header[10]  = (_fg->props.mod_scheme << 4) & 0xf0;
    _fg->header[10] |= (_fg->props.mod_bps) & 0x0f;

    // compute crc
    unsigned int header_key = crc32_generate_key(_fg->header, 11);
    _fg->header[11] = (header_key >> 24) & 0xff;
    _fg->header[12] = (header_key >> 16) & 0xff;
    _fg->header[13] = (header_key >>  8) & 0xff;
    _fg->header[14] = (header_key      ) & 0xff;

    // scramble header
    scramble_data(_fg->header, 15);

    // run encoder
    fec_encode(_fg->fec_header, 15, _fg->header, _fg->header_enc);

    // interleave header bits
    interleaver_interleave(_fg->intlv_header, _fg->header_enc, _fg->header_enc);

#if DEBUG_FLEXFRAMEGEN_PRINT
    // print results
    printf("flexframegen_encode_header():\n");
    printf("    mod scheme  : %u\n", _fg->props.mod_scheme);
    printf("    mod depth   : %u\n", _fg->props.mod_bps);
    printf("    payload len : %u\n", _fg->props.payload_len);
    printf("    header key  : 0x%.8x\n", header_key);

    printf("    user data   :");
    for (i=0; i<8; i++)
        printf(" %.2x", _user_header[i]);
    printf("\n");
#endif
}

void flexframegen_modulate_header(flexframegen _fg, float complex * _y)
{
    unsigned int i;

    // unpack header symbols
    for (i=0; i<32; i++) {
        _fg->header_sym[4*i+0] = (_fg->header_enc[i] >> 6) & 0x03;
        _fg->header_sym[4*i+1] = (_fg->header_enc[i] >> 4) & 0x03;
        _fg->header_sym[4*i+2] = (_fg->header_enc[i] >> 2) & 0x03;
        _fg->header_sym[4*i+3] = (_fg->header_enc[i]     ) & 0x03;
    }

    // modulate symbols
    for (i=0; i<128; i++)
        modem_modulate(_fg->mod_header, _fg->header_sym[i], &_y[i]);
}

void flexframegen_tmp_getheaderenc(flexframegen _fg, unsigned char * _header_enc)
{
    memmove(_header_enc, _fg->header_enc, 32);
}

