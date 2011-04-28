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

const char* modulation_scheme_str[LIQUID_MODEM_NUM_SCHEMES][2] = {
    // short name,  long name
    {"unknown",     "unknown"},
    {"psk",         "psk"},
    {"dpsk",        "dpsk"},
    {"ask",         "ask"},
    {"qam",         "qam"},
    {"apsk",        "apsk"},
    {"arb",         "arb"},
    {"bpsk",        "bpsk"},
    {"qpsk",        "qpsk"},
    {"apsk4",       "apsk4(1,3)"},
    {"apsk8",       "apsk8(1,7)"},
    {"apsk16",      "apsk16(4,12)"},
    {"apsk32",      "apsk32(4,12,16)"},
    {"apsk64",      "apsk64(4,14,20,26)"},
    {"apsk128",     "apsk128(8,18,24,36,42)"},
    {"V29",         "V.29"},
    {"arb16opt",    "arb16opt (optimal 16-qam)"},
    {"arb64vt",     "arb64vt (64-qam vt logo)"}
};

modulation_scheme liquid_getopt_str2mod(const char * _str)
{
    // compare each string to short name
    unsigned int i;
    for (i=0; i<LIQUID_MODEM_NUM_SCHEMES; i++) {
        if (strcmp(_str,modulation_scheme_str[i][0])==0)
            return i;
    }
    fprintf(stderr,"warning: liquid_getopt_str2mod(), unknown/unsupported mod scheme : %s\n", _str);
    return LIQUID_MODEM_UNKNOWN;
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

