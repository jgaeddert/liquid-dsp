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
// modem_bpsk.c : specific BPSK modem
//

#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "liquid.internal.h"

// create a bpsk (binary phase-shift keying) modem object
modem modem_create_bpsk()
{
    modem mod = (modem) malloc( sizeof(struct modem_s) );
    mod->scheme = LIQUID_MODEM_BPSK;

    modem_init(mod, 1);

    mod->modulate_func = &modem_modulate_bpsk;
    mod->demodulate_func = &modem_demodulate_bpsk;

    return mod;
}

// modulate BPSK
void modem_modulate_bpsk(modem _mod,
                         unsigned int symbol_in,
                         float complex *y)
{
    // compute output sample directly from input
    *y = symbol_in ? -1.0f : 1.0f;
}

// demodulate BPSK
void modem_demodulate_bpsk(modem _demod,
                           float complex _x,
                           unsigned int * _symbol_out)
{
    // slice directly to output symbol
    *_symbol_out = (crealf(_x) > 0 ) ? 0 : 1;

    // re-modulate symbol and store state
    modem_modulate_bpsk(_demod, *_symbol_out, &_demod->x_hat);
    _demod->r = _x;
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

