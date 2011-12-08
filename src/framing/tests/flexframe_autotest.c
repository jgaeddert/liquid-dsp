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

#include "autotest/autotest.h"
#include "liquid.internal.h"

// 
// AUTOTEST: encode, modulate, demodulate, decode header
//
void autotest_flexframe_decode_header()
{
    unsigned int i;

    // create flexframegen object
    flexframegenprops_s fgprops;
    fgprops.rampup_len  = 16;
    fgprops.phasing_len = 50;
    fgprops.payload_len = 64;
    fgprops.check       = LIQUID_CRC_NONE;
    fgprops.fec0        = LIQUID_FEC_NONE;
    fgprops.fec1        = LIQUID_FEC_NONE;
    fgprops.mod_scheme  = LIQUID_MODEM_PSK8;
    fgprops.rampdn_len  = 16;
    flexframegen fg = flexframegen_create(&fgprops);
    if (liquid_autotest_verbose)
        flexframegen_print(fg);

    // create flexframesync object
    //flexframesyncprops_s fsprops;
    flexframesync fs = flexframesync_create(NULL,NULL,NULL);

    // initialize header, payload
    unsigned char header[14];
    for (i=0; i<14; i++)
        header[i] = i;

    // internal test : encode/decode header
    /*
    float complex header_modulated[128];
    flexframegen_encode_header(fg, header);
    flexframegen_modulate_header(fg, header_modulated);

    flexframesync_demodulate_header(fs, header_modulated);
    flexframesync_decode_header(fs, NULL);
    */

    flexframegen_destroy(fg);
    flexframesync_destroy(fs);
}

