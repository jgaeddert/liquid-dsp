/*
 * Copyright (c) 2007, 2008, 2009, 2010 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010 Virginia Polytechnic
 *                                      Institute & State University
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
// modem_common.c
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "liquid.internal.h"

const char* modulation_scheme_str[LIQUID_NUM_MOD_SCHEMES] = {
    "unknown",
    "psk",
    "dpsk",
    "ask",
    "qam",
    "apsk",
    "arb",
    "arb (mirrored)",
    "arb (rotated)",
    "bpsk",
    "qpsk",
    "apsk4(1,3)",
    "apsk8(1,7)",
    "apsk16(4,12)",
    "apsk32(4,12,16)",
    "apsk64(4,14,20,26)",
    "apsk128(8,18,24,36,42)",
    "arb16opt (optimal 16-qam)",
    "arb64vt (64-qam vt logo)"
};

modulation_scheme liquid_getopt_str2mod(const char * _str)
{
    if (strcmp(_str,"unknown")==0) {
        return MOD_UNKNOWN;
    } else if (strcmp(_str,"psk")==0) {
        return MOD_PSK;
    } else if (strcmp(_str,"dpsk")==0) {
        return MOD_DPSK;
    } else if (strcmp(_str,"ask")==0) {
        return MOD_ASK;
    } else if (strcmp(_str,"qam")==0) {
        return MOD_QAM;
    } else if (strcmp(_str,"apsk")==0) {
        return MOD_APSK;
    } else if (strcmp(_str,"arb")==0) {
        return MOD_ARB;

    } else if (strcmp(_str,"bpsk")==0) {
        return MOD_BPSK;
    } else if (strcmp(_str,"qpsk")==0) {
        return MOD_QPSK;
    } else if (strcmp(_str,"apsk8")==0) {
        return MOD_APSK8;
    } else if (strcmp(_str,"apsk16")==0) {
        return MOD_APSK16;
    } else if (strcmp(_str,"apsk32")==0) {
        return MOD_APSK32;
    } else if (strcmp(_str,"apsk64")==0) {
        return MOD_APSK64;
    } else if (strcmp(_str,"apsk128")==0) {
        return MOD_APSK128;
    } else if (strcmp(_str,"arb16opt")==0) {
        return MOD_ARB16OPT;
    } else if (strcmp(_str,"arb64vt")==0) {
        return MOD_ARB64VT;
    }
    fprintf(stderr,"warning: liquid_getopt_str2mod(), unknown/unsupported mod scheme : %s\n", _str);
    return MOD_UNKNOWN;
}


// Generate random symbol
unsigned int modem_gen_rand_sym(modem _mod)
{
    return rand() % (_mod->M);
}

unsigned int modem_get_bps(modem _mod)
{
    return _mod->m;
}

unsigned int gray_encode(unsigned int symbol_in)
{
    return symbol_in ^ (symbol_in >> 1);
}

unsigned int gray_decode(unsigned int symbol_in)
{
    unsigned int mask = symbol_in;
    unsigned int symbol_out = symbol_in;
    unsigned int i;

    // Run loop in blocks of 4 to reduce number of comparisons. Running
    // loop more times than MAX_MOD_BITS_PER_SYMBOL will not result in
    // decoding errors.
    for (i=0; i<MAX_MOD_BITS_PER_SYMBOL; i+=4) {
        symbol_out ^= (mask >> 1);
        symbol_out ^= (mask >> 2);
        symbol_out ^= (mask >> 3);
        symbol_out ^= (mask >> 4);
        mask >>= 4;
    }

    return symbol_out;
}

unsigned int count_bit_errors(
    unsigned int _s1,
    unsigned int _s2)
{
    return count_ones(_s1^_s2);
}


