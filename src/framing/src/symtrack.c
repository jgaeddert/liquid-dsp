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
// Symbol tracker/synchronizer
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define DEBUG_SYMTRACK           0
#define DEBUG_SYMTRACK_PRINT     0
#define DEBUG_SYMTRACK_FILENAME  "symsync_internal_debug.m"
#define DEBUG_BUFFER_LEN        (1024)

//
// forward declaration of internal methods
//

// internal structure
struct SYMTRACK(_s) {
    // parameters
    int          filter_type;   // filter type (e.g. LIQUID_RNYQUIST_RKAISER)
    unsigned int k;             // samples/symbol
    unsigned int m;             // filter semi-length
    float        beta;          // filter excess bandwidth
    int          mod_scheme;    // demodulator

    // derived values

    // automatic gain control
    AGC() agc;
    float agc_bandwidth;

    // symbol timing recovery
    SYMSYNC() symsync;
    float     symsync_bandwidth;

    // equalizer/decimator
    EQLMS() eq;
    float   eq_bandwidth;

    // nco/phase-locked loop
    NCO() nco;
    float pll_bandwidth;

    // demodulator
    MODEM() demod;
};

// create symtrack object with basic parameters
//  _type   : filter type (e.g. LIQUID_RNYQUIST_RRC)
//  _k      : samples per symbol
//  _m      : filter delay (symbols)
//  _beta   : filter excess bandwidth
//  _ms     : modulation scheme (e.g. LIQUID_MODEM_QPSK)
SYMTRACK() SYMTRACK(_create)(int          _type,
                             unsigned int _k,
                             unsigned int _m,
                             float        _beta,
                             int          _ms)
{
    // validate input
    if (_k != 2) {
        fprintf(stderr,"error: symtrack_%s_create(), samples/symbol must be 2\n", EXTENSION_FULL);
        exit(1);
    } else if (_m == 0) {
        fprintf(stderr,"error: symtrack_%s_create(), filter delay must be greater than zero\n", EXTENSION_FULL);
        exit(1);
    } else if (_beta <= 0.0f || _beta > 1.0f) {
        fprintf(stderr,"error: symtrack_%s_create(), filter excess bandwidth must be in (0,1]\n", EXTENSION_FULL);
        exit(1);
    } else if (_ms == LIQUID_MODEM_UNKNOWN || _ms >= LIQUID_MODEM_NUM_SCHEMES) {
        fprintf(stderr,"error: symtrack_%s_create(), invalid modulation scheme\n", EXTENSION_FULL);
        exit(1);
    }

    // allocate memory for main object
    SYMTRACK() q = (SYMTRACK()) malloc( sizeof(struct SYMTRACK(_s)) );

    // return main object
    return q;
}

// create symtrack object using default parameters
SYMTRACK() SYMTRACK(_create_default)()
{
    return SYMTRACK(_create)(LIQUID_FIRFILT_ARKAISER,
                             2,     // samples/symbol
                             7,     // filter delay
                             0.3f,  // filter excess bandwidth
                             LIQUID_MODEM_QPSK);
}


// destroy symtrack object, freeing all internal memory
void SYMTRACK(_destroy)(SYMTRACK() _q)
{
    // free main object
    free(_q);
}

// print symtrack object's parameters
void SYMTRACK(_print)(SYMTRACK() _q)
{
    printf("symtrack_%s:\n", EXTENSION_FULL);
}

// reset symtrack internal state
void SYMTRACK(_reset)(SYMTRACK() _q)
{
}

// execute synchronizer on single input sample
//  _q      : synchronizer object
//  _x      : input data sample
//  _y      : output data array
//  _ny     : number of samples written to output buffer
void SYMTRACK(_execute)(SYMTRACK()     _q,
                        TI             _x,
                        TO *           _y,
                        unsigned int * _ny)
{
}

// execute synchronizer on input data array
//  _q      : synchronizer object
//  _x      : input data array
//  _nx     : number of input samples
//  _y      : output data array
//  _ny     : number of samples written to output buffer
void SYMTRACK(_execute_block)(SYMTRACK()     _q,
                              TI *           _x,
                              unsigned int   _nx,
                              TO *           _y,
                              unsigned int * _ny)
{
}

