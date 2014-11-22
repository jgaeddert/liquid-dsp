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
// modem_qpsk.c
//

#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "liquid.internal.h"

// create a qpsk (quaternary phase-shift keying) modem object
MODEM() MODEM(_create_qpsk)()
{
    MODEM() q = (MODEM()) malloc( sizeof(struct MODEM(_s)) );
    q->scheme = LIQUID_MODEM_QPSK;

    MODEM(_init)(q, 2);

    q->modulate_func   = &MODEM(_modulate_qpsk);
    q->demodulate_func = &MODEM(_demodulate_qpsk);

    // reset and return
    MODEM(_reset)(q);
    return q;
}


// modulate QPSK
void MODEM(_modulate_qpsk)(MODEM()      _q,
                           unsigned int _sym_in,
                           TC *         _y)
{
    // compute output sample directly from input
    *_y  = (_sym_in & 0x01 ? -M_SQRT1_2 : M_SQRT1_2) +
           (_sym_in & 0x02 ? -M_SQRT1_2 : M_SQRT1_2)*_Complex_I;
}

// demodulate QPSK
void MODEM(_demodulate_qpsk)(MODEM() _q,
                             TC _x,
                             unsigned int * _sym_out)
{
    // slice directly to output symbol
    *_sym_out  = (crealf(_x) > 0 ? 0 : 1) +
                    (cimagf(_x) > 0 ? 0 : 2);

    // re-modulate symbol and store state
    MODEM(_modulate_qpsk)(_q, *_sym_out, &_q->x_hat);
    _q->r = _x;
}

// demodulate QPSK (soft)
void MODEM(_demodulate_soft_qpsk)(MODEM()         _q,
                                  TC              _x,
                                  unsigned int  * _s,
                                  unsigned char * _soft_bits)
{
    // gamma = 1/(2*sigma^2), approximate for constellation size
    T gamma = 5.8f;

    // approximate log-likelihood ratios
    T LLR;
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
    MODEM(_modulate_qpsk)(_q, *_s, &_q->x_hat);
    _q->r = _x;
}

