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

//
// framesyncstats.c
//
// Default and generic frame statistics
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <complex.h>

#include "liquid.internal.h"

framesyncstats_s framesyncstats_default = {
    // signal quality
    0.0f,                   // error vector magnitude
    0.0f,                   // rssi
    0.0f,                   // carrier frequency offset

    // demodulated frame symbols
    NULL,                   // framesyms
    0,                      // num_framesyms

    // modulation/coding scheme, etc.
    LIQUID_MODEM_UNKNOWN,   // mod_scheme
    0,                      // mod_bps
    LIQUID_CRC_UNKNOWN,     // check
    LIQUID_FEC_UNKNOWN,     // fec0
    LIQUID_FEC_UNKNOWN      // fec1
};

// initialize framesyncstats object on default
void framesyncstats_init_default(framesyncstats_s * _stats)
{
    memmove(_stats, &framesyncstats_default, sizeof(framesyncstats_s));
}
// print framesyncstats object
void framesyncstats_print(framesyncstats_s * _stats)
{
    // validate statistics object
    if (_stats->mod_scheme >= LIQUID_MODEM_NUM_SCHEMES) {
        fprintf(stderr,"error: framesyncstats_print(), invalid modulation scheme\n");
        exit(1);
     } else if (_stats->check >= LIQUID_CRC_NUM_SCHEMES) {
        fprintf(stderr,"error: framesyncstats_print(), invalid CRC scheme\n");
        exit(1);
     } else if (_stats->fec0 >= LIQUID_FEC_NUM_SCHEMES) {
        fprintf(stderr,"error: framesyncstats_print(), invalid FEC scheme (inner)\n");
        exit(1);
     } else if (_stats->fec1 >= LIQUID_FEC_NUM_SCHEMES) {
        fprintf(stderr,"error: framesyncstats_print(), invalid FEC scheme (outer)\n");
        exit(1);
    }

    printf("    EVM                 :   %12.8f dB\n", _stats->evm);
    printf("    rssi                :   %12.8f dB\n", _stats->rssi);
    printf("    carrier offset      :   %12.8f Fs\n", _stats->cfo);
    printf("    num symbols         :   %u\n", _stats->num_framesyms);
    printf("    mod scheme          :   %s (%u bits/symbol)\n",
            modulation_types[_stats->mod_scheme].name,
            _stats->mod_bps);
    printf("    validity check      :   %s\n", crc_scheme_str[_stats->check][0]);
    printf("    fec (inner)         :   %s\n", fec_scheme_str[_stats->fec0][0]);
    printf("    fec (outer)         :   %s\n", fec_scheme_str[_stats->fec1][0]);
}

