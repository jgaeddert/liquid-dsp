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

// returns modulation_scheme and depth based on input string
void liquid_getopt_str2modbps(const char * _string,
                              modulation_scheme * _ms,
                              unsigned int * _bps)
{
    if (_string == NULL) {
        //fprintf(stderr,"warning: liquid_getopt_str2modbps(), input string is NULL\n");
        *_ms  = LIQUID_MODEM_UNKNOWN;
        *_bps = 0;
    }

    // phase-shift keying
    else if ( strcmp(_string,"psk2")==0 )   { *_ms = LIQUID_MODEM_PSK; *_bps = 1; return; }
    else if ( strcmp(_string,"psk4")==0 )   { *_ms = LIQUID_MODEM_PSK; *_bps = 2; return; }
    else if ( strcmp(_string,"psk8")==0 )   { *_ms = LIQUID_MODEM_PSK; *_bps = 3; return; }
    else if ( strcmp(_string,"psk16")==0 )  { *_ms = LIQUID_MODEM_PSK; *_bps = 4; return; }
    else if ( strcmp(_string,"psk32")==0 )  { *_ms = LIQUID_MODEM_PSK; *_bps = 5; return; }
    else if ( strcmp(_string,"psk64")==0 )  { *_ms = LIQUID_MODEM_PSK; *_bps = 6; return; }
    else if ( strcmp(_string,"psk128")==0 ) { *_ms = LIQUID_MODEM_PSK; *_bps = 7; return; }
    else if ( strcmp(_string,"psk256")==0 ) { *_ms = LIQUID_MODEM_PSK; *_bps = 8; return; }

    // differential phase-shift keying
    else if ( strcmp(_string,"dpsk2")==0 )   { *_ms = LIQUID_MODEM_DPSK; *_bps = 1; return; }
    else if ( strcmp(_string,"dpsk4")==0 )   { *_ms = LIQUID_MODEM_DPSK; *_bps = 2; return; }
    else if ( strcmp(_string,"dpsk8")==0 )   { *_ms = LIQUID_MODEM_DPSK; *_bps = 3; return; }
    else if ( strcmp(_string,"dpsk16")==0 )  { *_ms = LIQUID_MODEM_DPSK; *_bps = 4; return; }
    else if ( strcmp(_string,"dpsk32")==0 )  { *_ms = LIQUID_MODEM_DPSK; *_bps = 5; return; }
    else if ( strcmp(_string,"dpsk64")==0 )  { *_ms = LIQUID_MODEM_DPSK; *_bps = 6; return; }
    else if ( strcmp(_string,"dpsk128")==0 ) { *_ms = LIQUID_MODEM_DPSK; *_bps = 7; return; }
    else if ( strcmp(_string,"dpsk256")==0 ) { *_ms = LIQUID_MODEM_DPSK; *_bps = 8; return; }

    // amplitude-shift keying
    else if ( strcmp(_string,"ask2")==0 )   { *_ms = LIQUID_MODEM_ASK; *_bps = 1; return; }
    else if ( strcmp(_string,"ask4")==0 )   { *_ms = LIQUID_MODEM_ASK; *_bps = 2; return; }
    else if ( strcmp(_string,"ask8")==0 )   { *_ms = LIQUID_MODEM_ASK; *_bps = 3; return; }
    else if ( strcmp(_string,"ask16")==0 )  { *_ms = LIQUID_MODEM_ASK; *_bps = 4; return; }
    else if ( strcmp(_string,"ask32")==0 )  { *_ms = LIQUID_MODEM_ASK; *_bps = 5; return; }
    else if ( strcmp(_string,"ask64")==0 )  { *_ms = LIQUID_MODEM_ASK; *_bps = 6; return; }
    else if ( strcmp(_string,"ask128")==0 ) { *_ms = LIQUID_MODEM_ASK; *_bps = 7; return; }
    else if ( strcmp(_string,"ask256")==0 ) { *_ms = LIQUID_MODEM_ASK; *_bps = 8; return; }

    // quadrature amplitude-shift keying
    else if ( strcmp(_string,"qam4")==0 )   { *_ms = LIQUID_MODEM_QAM; *_bps = 2; return; }
    else if ( strcmp(_string,"qam8")==0 )   { *_ms = LIQUID_MODEM_QAM; *_bps = 3; return; }
    else if ( strcmp(_string,"qam16")==0 )  { *_ms = LIQUID_MODEM_QAM; *_bps = 4; return; }
    else if ( strcmp(_string,"qam32")==0 )  { *_ms = LIQUID_MODEM_QAM; *_bps = 5; return; }
    else if ( strcmp(_string,"qam64")==0 )  { *_ms = LIQUID_MODEM_QAM; *_bps = 6; return; }
    else if ( strcmp(_string,"qam128")==0 ) { *_ms = LIQUID_MODEM_QAM; *_bps = 7; return; }
    else if ( strcmp(_string,"qam256")==0 ) { *_ms = LIQUID_MODEM_QAM; *_bps = 8; return; }

    // amplitude/phase-shift keying
    else if ( strcmp(_string,"apsk4")==0 )   { *_ms = LIQUID_MODEM_APSK; *_bps = 2; return; }
    else if ( strcmp(_string,"apsk8")==0 )   { *_ms = LIQUID_MODEM_APSK; *_bps = 3; return; }
    else if ( strcmp(_string,"apsk16")==0 )  { *_ms = LIQUID_MODEM_APSK; *_bps = 4; return; }
    else if ( strcmp(_string,"apsk32")==0 )  { *_ms = LIQUID_MODEM_APSK; *_bps = 5; return; }
    else if ( strcmp(_string,"apsk64")==0 )  { *_ms = LIQUID_MODEM_APSK; *_bps = 6; return; }
    else if ( strcmp(_string,"apsk128")==0 ) { *_ms = LIQUID_MODEM_APSK; *_bps = 7; return; }
    else if ( strcmp(_string,"apsk256")==0 ) { *_ms = LIQUID_MODEM_APSK; *_bps = 8; return; }

    // arbitrary modem (unavailble)

    // specific modem types
    else if ( strcmp(_string,"bpsk")==0 )     { *_ms = LIQUID_MODEM_BPSK;     *_bps = 1; return; }
    else if ( strcmp(_string,"qpsk")==0 )     { *_ms = LIQUID_MODEM_QPSK;     *_bps = 2; return; }
    else if ( strcmp(_string,"ook")==0 )      { *_ms = LIQUID_MODEM_OOK;      *_bps = 1; return; }
    else if ( strcmp(_string,"sqam32")==0 )   { *_ms = LIQUID_MODEM_SQAM32;   *_bps = 5; return; }
    else if ( strcmp(_string,"sqam128")==0 )  { *_ms = LIQUID_MODEM_SQAM128;  *_bps = 7; return; }
    else if ( strcmp(_string,"V29")==0 )      { *_ms = LIQUID_MODEM_V29;      *_bps = 4; return; }
    else if ( strcmp(_string,"arb16opt")==0 ) { *_ms = LIQUID_MODEM_ARB16OPT; *_bps = 4; return; }
    else if ( strcmp(_string,"arb32opt")==0 ) { *_ms = LIQUID_MODEM_ARB32OPT; *_bps = 5; return; }
    else if ( strcmp(_string,"arb64vt")==0 )  { *_ms = LIQUID_MODEM_ARB64VT;  *_bps = 6; return; }

    // unknown schme
    else {
        //fprintf(stderr,"warning: liquid_getopt_str2modbps(), unknown scheme '%s'\n", _string);
        *_ms  = LIQUID_MODEM_UNKNOWN;
        *_bps = 0;
    }
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

