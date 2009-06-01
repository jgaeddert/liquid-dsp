/*
 * Copyright (c) 2007, 2009 Joseph Gaeddert
 * Copyright (c) 2007, 2009 Virginia Polytechnic Institute & State University
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


#ifndef __LIQUID_MODEM_API_H__
#define __LIQUID_MODEM_API_H__

#include "modem_common.h"

#include "complex.h"

#define MODEM_CONCAT(prefix, name) prefix ## name
#define MODEM_MANGLE_FLOAT(name) MODEM_CONCAT(modem, name)

#define MODEM_DEFINE_API(X, R, C)                                       \
                                                                        \
LIQUID_DEFINE_COMPLEX(R, C)                                             \
typedef struct X(_s) * X();                                             \
X() X(create)(modulation_scheme, unsigned int _bits_per_symbol);        \
void X(_free)(X() _mod);                                                \
void X(_arb_init)(X() _mod, C *_symbol_map, unsigned int _len);         \
void X(_arb_init_file)(X() _mod, char* filename);                       \
unsigned int X(_gen_rand_sym)(X() _mod);                                \
unsigned int X(_get_bps)(X() _mod);                                     \
void modulate(X() _mod, unsigned int symbol_in, C *y);                  \
void demodulate(X() _demod, C x, unsigned int *symbol_out);

MODEM_DEFINE_API(MODEM_MANGLE_FLOAT, float, complex)

#endif // __LIQUID_MODEM_API_H__


