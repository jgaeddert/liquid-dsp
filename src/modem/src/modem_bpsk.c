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
// modem_bpsk.c : specific BPSK modem
//

#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "liquid.internal.h"

// create a bpsk (binary phase-shift keying) modem object
MODEM() MODEM(_create_bpsk)()
{
    MODEM() q = (MODEM()) malloc( sizeof(struct MODEM(_s)) );
    q->scheme = LIQUID_MODEM_BPSK;

    MODEM(_init)(q, 1);

    q->modulate_func   = &MODEM(_modulate_bpsk);
    q->demodulate_func = &MODEM(_demodulate_bpsk);

    // reset and return
    MODEM(_reset)(q);
    return q;
}

// modulate BPSK
void MODEM(_modulate_bpsk)(MODEM()      _q,
                           unsigned int _sym_in,
                           TC *         _y)
{
    // compute output sample directly from input
    *_y = _sym_in ? -1.0f : 1.0f;
}

// demodulate BPSK
void MODEM(_demodulate_bpsk)(MODEM()        _q,
                             TC             _x,
                             unsigned int * _sym_out)
{
    // slice directly to output symbol
    *_sym_out = (crealf(_x) > 0 ) ? 0 : 1;

    // re-modulate symbol and store state
    MODEM(_modulate_bpsk)(_q, *_sym_out, &_q->x_hat);
    _q->r = _x;
}

// demodulate BPSK (soft)
void MODEM(_demodulate_soft_bpsk)(MODEM()         _q,
                                  TC              _x,
                                  unsigned int  * _s,
                                  unsigned char * _soft_bits)
{
    // gamma = 1/(2*sigma^2), approximate for constellation size
    T gamma = 4.0f;

    // approximate log-likelihood ratio
    T LLR = -2.0f * crealf(_x) * gamma;
    int soft_bit = LLR*16 + 127;
    if (soft_bit > 255) soft_bit = 255;
    if (soft_bit <   0) soft_bit = 0;
    _soft_bits[0] = (unsigned char) ( soft_bit );

    // re-modulate symbol and store state
    unsigned int symbol_out = (crealf(_x) > 0 ) ? 0 : 1;
    MODEM(_modulate_bpsk)(_q, symbol_out, &_q->x_hat);
    _q->r = _x;
    *_s = symbol_out;
}

