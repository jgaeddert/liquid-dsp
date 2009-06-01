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
//
//

#include <stdio.h>
#include <stdlib.h>

#include "framing_internal.h"

frame frame_create()
{
    frame f = (frame) malloc(sizeof(struct frame_s));

    // set default values
    f->src0 = 0;
    f->src1 = 1;
    f->dst0 = 0;
    f->dst1 = 1;

    f->ms = MOD_BPSK;
    f->bps = 1;

    f->fec_inner = FEC_UNKNOWN;
    f->fec_outer = FEC_UNKNOWN;

    f->intlv_inner = 0;
    f->intlv_outer = 0;

    f->protocol = 0;
    f->msg_length = 0;
    f->num_symbols = 0;

    return f;
}

void frame_destroy(frame _f)
{
    free(_f);
}

void frame_print(frame _f)
{
    printf("frame:\n");
    printf("    crc             %-4u %-16u\n",32,0); // crc-key
    printf("    src0            %-4u %-16u\n",8,_f->src0);
    printf("    src1            %-4u %-16u\n",8,_f->src1);
    printf("    dst0            %-4u %-16u\n",8,_f->dst0);
    printf("    dst1            %-4u %-16u\n",8,_f->dst1);
    printf("    mod scheme      %-4u %-16s\n",4,"blah");
    printf("    mod bps         %-4u %-16u\n",4,_f->bps);
    printf("    num_symbols     %-4u %-16u\n",16,_f->num_symbols);
}

void frame_setkey(frame _f, frame_keyid _id, unsigned int _value)
{
    switch (_id) {
    case FRAME_SRC0:                _f->src0 = _value;  return;
    case FRAME_SRC1:                _f->src1 = _value;  return;
    case FRAME_DST0:                _f->dst0 = _value;  return;
    case FRAME_DST1:                _f->dst1 = _value;  return;

    case FRAME_MOD_SCHEME:          _f->ms = (modulation_scheme) _value; return;
    case FRAME_MOD_BPS:             _f->bps = _value; return;

    case FRAME_FEC_INNER_SCHEME:    _f->fec_inner = (fec_scheme) _value; return;
    case FRAME_FEC_OUTER_SCHEME:    _f->fec_outer = (fec_scheme) _value; return;

    case FRAME_INTLV_INNER_SCHEME:  _f->intlv_inner = _value; return;
    case FRAME_INTLV_OUTER_SCHEME:  _f->intlv_outer = _value; return;

    case FRAME_PROTOCOL:            _f->protocol = _value;      return;
    case FRAME_MSG_LENGTH:          _f->msg_length = _value;    return;
    case FRAME_NUM_SYMBOLS:         _f->num_symbols = _value;   return;

    default:
        printf("error: frame_setkey(), unknown key: %u\n", _id);
        exit(0);
    };
}

//void frame_encode(frame _f, unsigned char * _header, unsigned char *_out)
//bool frame_decode(frame _f, unsigned char * _in, unsigned char * _header);


