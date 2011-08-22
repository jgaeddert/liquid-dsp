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
    {"arb64opt",    "arb64opt (optimal 64-qam)"},
    {"arb128opt",   "arb128opt (optimal 128-qam)"},
    {"arb256opt",   "arb256opt (optimal 256-qam)"},
    {"arb64vt",     "arb64vt (64-qam vt logo)"}
};

// full modulation type descriptor
const struct modulation_type_s modulation_types[LIQUID_MODEM_NUM_FULL_SCHEMES] = {
    // unknown
    {"unknown", LIQUID_MODEM_UNKNOWN, 0},

    // phase-shift keying
    {"psk2",    LIQUID_MODEM_PSK, 1},
    {"psk4",    LIQUID_MODEM_PSK, 2},
    {"psk8",    LIQUID_MODEM_PSK, 3},
    {"psk16",   LIQUID_MODEM_PSK, 4},
    {"psk32",   LIQUID_MODEM_PSK, 5},
    {"psk64",   LIQUID_MODEM_PSK, 6},
    {"psk128",  LIQUID_MODEM_PSK, 7},
    {"psk256",  LIQUID_MODEM_PSK, 8},

    // differential phase-shift keying
    {"dpsk2",   LIQUID_MODEM_DPSK, 1},
    {"dpsk4",   LIQUID_MODEM_DPSK, 2},
    {"dpsk8",   LIQUID_MODEM_DPSK, 3},
    {"dpsk16",  LIQUID_MODEM_DPSK, 4},
    {"dpsk32",  LIQUID_MODEM_DPSK, 5},
    {"dpsk64",  LIQUID_MODEM_DPSK, 6},
    {"dpsk128", LIQUID_MODEM_DPSK, 7},
    {"dpsk256", LIQUID_MODEM_DPSK, 8},

    // amplitude-shift keying
    {"ask2",    LIQUID_MODEM_ASK, 1},
    {"ask4",    LIQUID_MODEM_ASK, 2},
    {"ask8",    LIQUID_MODEM_ASK, 3},
    {"ask16",   LIQUID_MODEM_ASK, 4},
    {"ask32",   LIQUID_MODEM_ASK, 5},
    {"ask64",   LIQUID_MODEM_ASK, 6},
    {"ask128",  LIQUID_MODEM_ASK, 7},
    {"ask256",  LIQUID_MODEM_ASK, 8},

    // quadrature amplitude-shift keying
    {"qam4",    LIQUID_MODEM_QAM, 2},
    {"qam8",    LIQUID_MODEM_QAM, 3},
    {"qam16",   LIQUID_MODEM_QAM, 4},
    {"qam32",   LIQUID_MODEM_QAM, 5},
    {"qam64",   LIQUID_MODEM_QAM, 6},
    {"qam128",  LIQUID_MODEM_QAM, 7},
    {"qam256",  LIQUID_MODEM_QAM, 8},

    // amplitude/phase-shift keying
    {"apsk4",   LIQUID_MODEM_APSK, 2},
    {"apsk8",   LIQUID_MODEM_APSK, 3},
    {"apsk16",  LIQUID_MODEM_APSK, 4},
    {"apsk32",  LIQUID_MODEM_APSK, 5},
    {"apsk64",  LIQUID_MODEM_APSK, 6},
    {"apsk128", LIQUID_MODEM_APSK, 7},
    {"apsk256", LIQUID_MODEM_APSK, 8},

    // arbitrary modem (unavailble)

    // specific modem types
    {"bpsk",     LIQUID_MODEM_BPSK,     1},
    {"qpsk",     LIQUID_MODEM_QPSK,     2},
    {"ook",      LIQUID_MODEM_OOK,      1},
    {"sqam32",   LIQUID_MODEM_SQAM32,   5},
    {"sqam128",  LIQUID_MODEM_SQAM128,  7},
    {"V29",      LIQUID_MODEM_V29,      4},
    {"arb16opt", LIQUID_MODEM_ARB16OPT, 4},
    {"arb32opt", LIQUID_MODEM_ARB32OPT, 5},
    {"arb64opt", LIQUID_MODEM_ARB64OPT, 6},
    {"arb128opt",LIQUID_MODEM_ARB128OPT,7},
    {"arb256opt",LIQUID_MODEM_ARB256OPT,8},
    {"arb64vt",  LIQUID_MODEM_ARB64VT,  6}
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

// returns modulation_scheme and depth based on input string
void liquid_getopt_str2modbps(const char * _string,
                              modulation_scheme * _ms,
                              unsigned int * _bps)
{
    // default to 'unknown' type
    *_ms  = LIQUID_MODEM_UNKNOWN;
    *_bps = 0;

    if (_string == NULL) {
        //fprintf(stderr,"warning: liquid_getopt_str2modbps(), input string is NULL\n");
        return;
    }

    unsigned int i;
    for (i=0; i<LIQUID_MODEM_NUM_FULL_SCHEMES; i++) {
        if (strcmp(_string,modulation_types[i].name)==0) {
            *_ms  = modulation_types[i].ms;
            *_bps = modulation_types[i].bps;
            return;
        }
    }

    //fprintf(stderr,"warning: liquid_getopt_str2modbps(), unknown scheme '%s'\n", _string);
}


// Print compact list of existing and available modulation schemes
void liquid_print_modulation_schemes()
{
    unsigned int i;
    unsigned int len = 10;

    // print all available modem schemes
    printf("          ");
    for (i=1; i<LIQUID_MODEM_NUM_FULL_SCHEMES; i++) {
        printf("%s", modulation_types[i].name);

        if (i != LIQUID_MODEM_NUM_FULL_SCHEMES-1)
            printf(", ");

        len += strlen(modulation_types[i].name);
        if (len > 48 && i != LIQUID_MODEM_NUM_FULL_SCHEMES-1) {
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
        s |= _soft_bits[i] > LIQUID_SOFTBIT_ERASURE ? 1 : 0;
    }
    *_sym_out = s;
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
        _soft_bits[i] = ((_sym_in >> (_bps-i-1)) & 0x0001) ? LIQUID_SOFTBIT_1 : LIQUID_SOFTBIT_0;
}


