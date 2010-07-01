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

//
// m-sequence
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include "liquid.internal.h"

#define LIQUID_MIN_MSEQUENCE_M  2
#define LIQUID_MAX_MSEQUENCE_M  12

struct msequence_s msequence_default[13] = {
//   m,     g,      a,      n,      v,      b
    {0,     0,      1,      0,      1,      0}, // dummy placeholder
    {0,     0,      1,      0,      1,      0}, // dummy placeholder
    {2,     0x0003, 0x0002, 3,      0x0002, 0},
    {3,     0x0005, 0x0004, 7,      0x0004, 0},
    {4,     0x0009, 0x0008, 15,     0x0008, 0},
    {5,     0x0012, 0x0010, 31,     0x0010, 0},
    {6,     0x0021, 0x0020, 63,     0x0020, 0},
    {7,     0x0044, 0x0040, 127,    0x0040, 0},
    {8,     0x008E, 0x0080, 255,    0x0080, 0},
    {9,     0x0108, 0x0100, 511,    0x0100, 0},
    {10,    0x0204, 0x0200, 1023,   0x0200, 0},
    {11,    0x0402, 0x0400, 2047,   0x0400, 0},
    {12,    0x0829, 0x0800, 4095,   0x0800, 0}
};

msequence msequence_create(unsigned int _m)
{
    if (_m > LIQUID_MAX_MSEQUENCE_M || _m < LIQUID_MIN_MSEQUENCE_M) {
        printf("error: msequence_create(), m not in range\n");
        exit(1);
    }
    msequence ms = (msequence) malloc(sizeof(struct msequence_s));
    memmove(ms, &msequence_default[_m], sizeof(struct msequence_s));
    return ms;
}

void msequence_destroy(msequence _ms)
{
    free(_ms);
}

void msequence_init(msequence _ms, unsigned int _m, unsigned int _g, unsigned int _a)
{
    _ms->m = _m;
    _ms->g = _g >> 1;
    // reverse initial state register:
    // 0001 -> 1000
    unsigned int i;
    _ms->a = 0;
    for (i=0; i<_ms->m; i++) {
        _ms->a <<= 1;
        _ms->a |= (_a & 0x01);
        _a >>= 1;
    }

    _ms->n = (1<<_m)-1;
    _ms->v = _ms->a;
    _ms->b = 0;
}

// Advance msequence on shift register
unsigned int msequence_advance(msequence _ms)
{
    _ms->b = liquid_bdotprod( _ms->v, _ms->g );
    _ms->v <<= 1;       // shift register
    _ms->v |= _ms->b;   // push bit onto register
    _ms->v &= _ms->n;   // apply mask to register
    return _ms->b;      // return result
}


unsigned int msequence_generate_symbol(msequence _ms, unsigned int _bps)
{
    unsigned int i, s = 0;
    for (i=0; i<_bps; i++) {
        s <<= 1;
        s |= msequence_advance(_ms);
    }
    return s;
}

void msequence_reset(msequence _ms)
{
    _ms->v = _ms->a;
}


void bsequence_init_msequence(
    bsequence _bs,
    msequence _ms)
{
    if (_ms->n > LIQUID_MAX_MSEQUENCE_LENGTH) {
        perror("error: bsequence_init_msequence(), msequence length exceeds maximum\n");
        exit(-1);
    }

    // clear binary sequence
    bsequence_clear(_bs);

    unsigned int i;
    for (i=0; i<(_ms->n); i++)
        bsequence_push(_bs, msequence_advance(_ms));
}

unsigned int msequence_get_length(msequence _ms)
{
    return _ms->n;
}

