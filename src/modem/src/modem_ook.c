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
// modem_ook.c
//

#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "liquid.internal.h"

// create an ook (on/off keying) modem object
modem modem_create_ook()
{
    modem mod = (modem) malloc( sizeof(struct modem_s) );
    mod->scheme = LIQUID_MODEM_OOK;

    modem_init(mod, 1);

    mod->modulate_func = &modem_modulate_ook;
    mod->demodulate_func = &modem_demodulate_ook;

    return mod;
}

// modulate symbol using on/off keying
void modem_modulate_ook(modem _q,
                        unsigned int symbol_in,
                        float complex *y)
{
    // compute output sample directly from input
    *y = symbol_in ? 0.0f : M_SQRT2;
}

// demodulate OOK
void modem_demodulate_ook(modem _demod,
                          float complex _x,
                          unsigned int * _symbol_out)
{
    // slice directly to output symbol
    *_symbol_out = (crealf(_x) > M_SQRT1_2 ) ? 0 : 1;

    // re-modulate symbol and store state
    modem_modulate_ook(_demod, *_symbol_out, &_demod->x_hat);
    _demod->r = _x;
}

