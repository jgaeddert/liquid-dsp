/*
 * Copyright (c) 2008, 2009, 2010 Joseph Gaeddert
 * Copyright (c) 2008, 2009, 2010 Virginia Polytechnic
 *                                Institute & State University
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
// qtype_mul.port.c
//
// Portable qtype_t fixed-point multiplication implementation
//

#include "liquidfpm.internal.h"

#define Q(name)     LIQUIDFPM_CONCAT(q32,name)

Q(_t) Q(_mul)(Q(_t) _x, Q(_t) _y)
{
#if 0
    // This method loses significant precision by pre-shifting
    // out the least significant bits of each argument

    unsigned int s0 = Q(_fracbits) >> 1;
    unsigned int s1 = Q(_fracbits) - s0;

    return (_x >> s0) * (_y >> s1);
#else
    // compute multiplication using high-order accumulator,
    // then post-shift the result and return

    Q(_at) z = (Q(_at))(_x) * (Q(_at))(_y);

    return (Q(_t)) (z >> Q(_fracbits));
#endif
}

