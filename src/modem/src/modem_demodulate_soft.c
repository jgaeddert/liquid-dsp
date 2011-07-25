/*
 * Copyright (c) 2007, 2008, 2009, 2010, 2011 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010, 2011 Virginia Polytechnic
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
// modem_demodulate_soft.c
//
// Definitions for linear soft demodulation of symbols.
//

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

#include "liquid.internal.h"

#define DEBUG_DEMODULATE_SOFT 0

// generic demodulation
void modem_demodulate_soft(modem _demod,
                          float complex _x,
                          unsigned int  * _s,
                          unsigned char * _soft_bits)
{
    // switch scheme
    switch (_demod->scheme) {
    case LIQUID_MODEM_ARB:  modem_demodulate_soft_arb( _demod,_x,_s,_soft_bits); return;
    case LIQUID_MODEM_BPSK: modem_demodulate_soft_bpsk(_demod,_x,_s,_soft_bits); return;
    case LIQUID_MODEM_QPSK: modem_demodulate_soft_qpsk(_demod,_x,_s,_soft_bits); return;
    default:;
    }

    // check if...
    if (_demod->demod_soft_neighbors != NULL && _demod->demod_soft_p != 0) {
        // demodulate using approximate log-likelihood method with
        // look-up table for nearest neighbors
        modem_demodulate_soft_table(_demod, _x, _s, _soft_bits);

        return;
    }

    // for now demodulate normally and simply copy the
    // hard-demodulated bits
    unsigned int symbol_out;
    _demod->demodulate_func(_demod, _x, &symbol_out);
    *_s = symbol_out;

    // unpack soft bits
    liquid_unpack_soft_bits(symbol_out, _demod->m, _soft_bits);
}

#if DEBUG_DEMODULATE_SOFT
// print a string of bits to the standard output
void print_bitstring_demod_soft(unsigned int _x,
                                unsigned int _n)
{
    unsigned int i;
    for (i=0; i<_n; i++)
        printf("%1u", (_x >> (_n-i-1)) & 1);
}
#endif

// generic soft demodulation using look-up table...
//  _demod      :   demodulator object
//  _r          :   received sample
//  _s          :   hard demodulator output
//  _soft_bits  :   soft bit ouput (approximate log-likelihood ratio)
void modem_demodulate_soft_table(modem _demod,
                                 float complex _r,
                                 unsigned int * _s,
                                 unsigned char * _soft_bits)
{
    // run hard demodulation; this will store re-modulated sample
    // as internal variable x_hat
    unsigned int s;
    modem_demodulate(_demod, _r, &s);

    unsigned int bps = modem_get_bps(_demod);

    // gamma = 1/(2*sigma^2), approximate for constellation size
    float gamma = 1.2f*_demod->M;

    // set and initialize minimum bit values
    unsigned int i;
    unsigned int k;
    float dmin_0[bps];
    float dmin_1[bps];
    for (k=0; k<bps; k++) {
        dmin_0[k] = 8.0f;
        dmin_1[k] = 8.0f;
    }

    unsigned int bit;
    float d;
    float complex x_hat;    // re-modulated symbol
    unsigned char * softab = _demod->demod_soft_neighbors;
    unsigned int p = _demod->demod_soft_p;

    // check hard demodulation
    d = crealf( (_r-_demod->x_hat)*conjf(_r-_demod->x_hat) );
    for (k=0; k<bps; k++) {
        bit = (s >> (bps-k-1)) & 0x01;
        if (bit) dmin_1[k] = d;
        else     dmin_0[k] = d;
    }

    // parse all 'nearest neighbors' and find minimum distance for each bit
    for (i=0; i<p; i++) {
        // remodulate symbol
        if (_demod->modulate_using_map)
            x_hat = _demod->symbol_map[ softab[s*p + i] ];
        else
            modem_modulate(_demod, softab[s*p+i], &x_hat);

        // compute magnitude squared of Euclidean distance
        //d = crealf( (_r-x_hat)*conjf(_r-x_hat) );
        // (same as above, but faster)
        float complex e = _r - x_hat;
        d = crealf(e)*crealf(e) + cimagf(e)*cimagf(e);

        // look at each bit in 'nearest neighbor' and update minimum
        for (k=0; k<bps; k++) {
            // strip bit
            unsigned int bit = (softab[s*p+i] >> (bps-k-1)) & 0x01;
            if ( bit ) {
                if (d < dmin_1[k]) dmin_1[k] = d;
            } else {
                if (d < dmin_0[k]) dmin_0[k] = d;
            }
        }
    }

    // make soft bit assignments
    for (k=0; k<bps; k++) {
        int soft_bit = ((dmin_0[k] - dmin_1[k])*gamma)*16 + 127;
        if (soft_bit > 255) soft_bit = 255;
        if (soft_bit <   0) soft_bit = 0;
        _soft_bits[k] = (unsigned char)soft_bit;
    }

    // set hard output symbol
    *_s = s;
}



// demodulate arbitrary modem type (soft)
void modem_demodulate_soft_arb(modem _demod,
                               float complex _r,
                               unsigned int  * _s,
                               unsigned char * _soft_bits)
{
    unsigned int bps = _demod->m;
    unsigned int M   = _demod->M;

    // gamma = 1/(2*sigma^2), approximate for constellation size
    float gamma = 1.2f*_demod->M;

    unsigned int s=0;       // hard decision output
    unsigned int k;         // bit index
    unsigned int i;         // symbol index
    float d;                // distance for this symbol
    float complex x_hat;    // re-modulated symbol

    float dmin_0[bps];
    float dmin_1[bps];
    for (k=0; k<bps; k++) {
        dmin_0[k] = 4.0f;
        dmin_1[k] = 4.0f;
    }
    float dmin = 0.0f;

    for (i=0; i<M; i++) {
        // compute distance from received symbol
        x_hat = _demod->symbol_map[i];
        d = crealf( (_r-x_hat)*conjf(_r-x_hat) );

        // set hard-decision...
        if (d < dmin || i==0) {
            s = i;
            dmin = d;
        }

        for (k=0; k<bps; k++) {
            // strip bit
            if ( (s >> (bps-k-1)) & 0x01 ) {
                if (d < dmin_1[k]) dmin_1[k] = d;
            } else {
                if (d < dmin_0[k]) dmin_0[k] = d;
            }
        }
    }

    // make assignments
    for (k=0; k<bps; k++) {
        int soft_bit = ((dmin_0[k] - dmin_1[k])*gamma)*16 + 127;
        if (soft_bit > 255) soft_bit = 255;
        if (soft_bit <   0) soft_bit = 0;
        _soft_bits[k] = (unsigned char)soft_bit;
    }

    // hard decision

    // set hard output symbol
    *_s = s;

    // re-modulate symbol and store state
    modem_modulate_arb(_demod, *_s, &_demod->x_hat);
    _demod->r = _r;
}

// demodulate BPSK (soft)
void modem_demodulate_soft_bpsk(modem _demod,
                                float complex _x,
                                unsigned int  * _s,
                                unsigned char * _soft_bits)
{
    // gamma = 1/(2*sigma^2), approximate for constellation size
    float gamma = 4.0f;

    // approximate log-likelihood ratio
    float LLR = -2.0f * crealf(_x) * gamma;
    int soft_bit = LLR*16 + 127;
    if (soft_bit > 255) soft_bit = 255;
    if (soft_bit <   0) soft_bit = 0;
    _soft_bits[0] = (unsigned char) ( soft_bit );

    // re-modulate symbol and store state
    unsigned int symbol_out = (crealf(_x) > 0 ) ? 0 : 1;
    modem_modulate_bpsk(_demod, symbol_out, &_demod->x_hat);
    _demod->r = _x;
    *_s = symbol_out;
}

// demodulate QPSK (soft)
void modem_demodulate_soft_qpsk(modem _demod,
                                float complex _x,
                                unsigned int  * _s,
                                unsigned char * _soft_bits)
{
    // gamma = 1/(2*sigma^2), approximate for constellation size
    float gamma = 5.8f;

    // approximate log-likelihood ratios
    float LLR;
    int soft_bit;
    
    // compute soft value for first bit
    LLR = -2.0f * cimagf(_x) * gamma;
    soft_bit = LLR*16 + 127;
    if (soft_bit > 255) soft_bit = 255;
    if (soft_bit <   0) soft_bit = 0;
    _soft_bits[0] = (unsigned char) ( soft_bit );

    // compute soft value for first bit
    LLR = -2.0f * crealf(_x) * gamma;
    soft_bit = LLR*16 + 127;
    if (soft_bit > 255) soft_bit = 255;
    if (soft_bit <   0) soft_bit = 0;
    _soft_bits[1] = (unsigned char) ( soft_bit );

    // re-modulate symbol and store state
    *_s  = (crealf(_x) > 0 ? 0 : 1) +
           (cimagf(_x) > 0 ? 0 : 2);
    modem_modulate_qpsk(_demod, *_s, &_demod->x_hat);
    _demod->r = _x;
}

