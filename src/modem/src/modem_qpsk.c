/*
 * Copyright (c) 2007, 2008, 2009, 2010, 2011, 2012 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010, 2011, 2012 Virginia Polytechnic
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
// modem_qpsk.c
//

#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "liquid.internal.h"

// create a qpsk (quaternary phase-shift keying) modem object
modem modem_create_qpsk()
{
    modem mod = (modem) malloc( sizeof(struct modem_s) );
    mod->scheme = LIQUID_MODEM_QPSK;

    modem_init(mod, 2);

    mod->modulate_func = &modem_modulate_qpsk;
    mod->demodulate_func = &modem_demodulate_qpsk;

    return mod;
}


// modulate QPSK
void modem_modulate_qpsk(modem _mod,
                         unsigned int symbol_in,
                         float complex *y)
{
    // compute output sample directly from input
    *y  = (symbol_in & 0x01 ? -M_SQRT1_2 : M_SQRT1_2) +
          (symbol_in & 0x02 ? -M_SQRT1_2 : M_SQRT1_2)*_Complex_I;
}

// demodulate QPSK
void modem_demodulate_qpsk(modem _demod,
                           float complex _x,
                           unsigned int * _symbol_out)
{
    // slice directly to output symbol
    *_symbol_out  = (crealf(_x) > 0 ? 0 : 1) +
                    (cimagf(_x) > 0 ? 0 : 2);

    // re-modulate symbol and store state
    modem_modulate_qpsk(_demod, *_symbol_out, &_demod->x_hat);
    _demod->r = _x;
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

