/*
 * Copyright (c) 2007, 2008, 2009, 2010, 2011Joseph Gaeddert
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

// generic demodulation
void modem_demodulate_soft(modem _demod,
                          float complex _x,
                          unsigned char * _bits)
{
    // for now demodulate normally and simply copy the
    // hard-demodulated bits
    unsigned int symbol_out;
    _demod->demodulate_func(_demod, _x, &symbol_out);

    unsigned int i;
    for (i=0; i<_demod->m; i++)
        _bits[i] = ((symbol_out >> (_demod->m-i-1)) & 0x0001) ? LIQUID_FEC_SOFTBIT_1 : LIQUID_FEC_SOFTBIT_0;
}

