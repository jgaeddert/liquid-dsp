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
    // switch scheme
    switch (_demod->scheme) {
    case LIQUID_MODEM_BPSK:     modem_demodulate_soft_bpsk(_demod, _x, _bits);  return;
    case LIQUID_MODEM_QPSK:     modem_demodulate_soft_qpsk(_demod, _x, _bits);  return;
    default:;
    }

    // for now demodulate normally and simply copy the
    // hard-demodulated bits
    unsigned int symbol_out;
    _demod->demodulate_func(_demod, _x, &symbol_out);

    unsigned int i;
    for (i=0; i<_demod->m; i++)
        _bits[i] = ((symbol_out >> (_demod->m-i-1)) & 0x0001) ? LIQUID_FEC_SOFTBIT_1 : LIQUID_FEC_SOFTBIT_0;
}

// demodulate BPSK (soft)
void modem_demodulate_soft_bpsk(modem _demod,
                                float complex _x,
                                unsigned char * _bits_out)
{
    // soft output
    _bits_out[0] = (unsigned char) ( 255*(0.5 + 0.5*tanhf(crealf(_x))) );

    // re-modulate symbol and store state
    unsigned int symbol_out = (crealf(_x) > 0 ) ? 0 : 1;
    modem_modulate_bpsk(_demod, symbol_out, &_demod->x_hat);
    _demod->r = _x;
}

// demodulate QPSK (soft)
void modem_demodulate_soft_qpsk(modem _demod,
                                float complex _x,
                                unsigned char * _bits_out)
{
    // soft output
    _bits_out[0] = (unsigned char) ( 255*(0.5 + 0.5*tanhf(crealf(_x))) );
    _bits_out[1] = (unsigned char) ( 255*(0.5 + 0.5*tanhf(cimagf(_x))) );

    // re-modulate symbol and store state
    unsigned int symbol_out  = (crealf(_x) > 0 ? 0 : 1) +
                               (cimagf(_x) > 0 ? 0 : 2);
    modem_modulate_qpsk(_demod, symbol_out, &_demod->x_hat);
    _demod->r = _x;
}


