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
    {"psk",         "phase-shift keying"},
    {"dpsk",        "differential phase-shift keying"},
    {"ask",         "amplitude-shift keying"},
    {"qam",         "quadrature amplitude-shift keying"},
    {"apsk",        "amplitude/phase-shift keying"},
    {"arb",         "arbitrary modem constellation"},
    {"bpsk",        "binary phase-shift keying"},
    {"qpsk",        "quaternary phase-shift keying"},
    {"ook",         "ook (on/off keying)"},
    {"sqam32",      "'square' 32-QAM"},
    {"sqam128",     "'square' 128-QAM"},
    {"V29",         "V.29"},
    {"arb16opt",    "arb16opt (optimal 16-qam)"},
    {"arb32opt",    "arb32opt (optimal 32-qam)"},
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

// Print compact list of existing and available modulation schemes
void liquid_print_modulation_schemes()
{
    unsigned int i;
    unsigned int len = 10;

    // print all available MOD schemes
    printf("          ");
    for (i=0; i<LIQUID_MODEM_NUM_SCHEMES; i++) {
        printf("%s", modulation_scheme_str[i][0]);

        if (i != LIQUID_MODEM_NUM_SCHEMES-1)
            printf(", ");

        len += strlen(modulation_scheme_str[i][0]);
        if (len > 48 && i != LIQUID_MODEM_NUM_SCHEMES-1) {
            len = 10;
            printf("\n          ");
        }
    }
    printf("\n");
}


// Generate random symbol
unsigned int modem_gen_rand_sym(modem _mod)
{
    return rand() % (_mod->M);
}

// Get modem depth (bits/symbol)
unsigned int modem_get_bps(modem _mod)
{
    return _mod->m;
}

// gray encoding
unsigned int gray_encode(unsigned int symbol_in)
{
    return symbol_in ^ (symbol_in >> 1);
}

// gray decoding
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

// pack soft bits into symbol
//  _soft_bits  :   soft input bits [size: _bps x 1]
//  _bps        :   bits per symbol
//  _sym_out    :   output symbol, value in [0,2^_bps)
void liquid_pack_soft_bits(unsigned char * _soft_bits,
                           unsigned int _bps,
                           unsigned int * _sym_out)
{
    // validate input
    if (_bps > MAX_MOD_BITS_PER_SYMBOL) {
        fprintf(stderr,"error: liquid_unpack_soft_bits(), bits/symbol exceeds maximum (%u)\n", MAX_MOD_BITS_PER_SYMBOL);
        exit(1);
    }

    unsigned int i;
    unsigned int s=0;
    for (i=0; i<_bps; i++) {
        s <<= 1;
        s |= _soft_bits[i] > LIQUID_FEC_SOFTBIT_ERASURE ? 1 : 0;
    }
}

// unpack soft bits into symbol
//  _sym_in     :   input symbol, value in [0,2^_bps)
//  _bps        :   bits per symbol
//  _soft_bits  :   soft output bits [size: _bps x 1]
void liquid_unpack_soft_bits(unsigned int _sym_in,
                             unsigned int _bps,
                             unsigned char * _soft_bits)
{
    // validate input
    if (_bps > MAX_MOD_BITS_PER_SYMBOL) {
        fprintf(stderr,"error: liquid_unpack_soft_bits(), bits/symbol exceeds maximum (%u)\n", MAX_MOD_BITS_PER_SYMBOL);
        exit(1);
    }

    unsigned int i;
    for (i=0; i<_bps; i++)
        _soft_bits[i] = ((_sym_in >> (_bps-i-1)) & 0x0001) ? LIQUID_FEC_SOFTBIT_1 : LIQUID_FEC_SOFTBIT_0;
}


