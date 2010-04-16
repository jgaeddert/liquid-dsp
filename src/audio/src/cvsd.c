/*
 * Copyright (c) 2007, 2008, 2009, 2010 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010 Virginia Polytechnic
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
// CVSD: continuously variable slope delta
//

#include <stdio.h>
#include <stdlib.h>

#include "liquid.internal.h"

struct cvsd_s {
    unsigned int num_bits;
    unsigned char bitref;   // historical bit reference
    unsigned char bitmask;  // historical bit reference mask
    float ref;              // internal reference

    float zeta;             // delta step factor
    float delta;            // current step size
    float delta_min;        // minimum delta
    float delta_max;        // maximum delta
};

cvsd cvsd_create(unsigned int _num_bits, float _zeta)
{
    if (_num_bits == 0) {
        printf("error: cvsd_create(), _num_bits must be positive\n");
        exit(1);
    } else if (_zeta <= 1.0f) {
        printf("error: cvsd_create(), zeta must be greater than 1\n");
        exit(1);
    }
    cvsd q = (cvsd) malloc(sizeof(struct cvsd_s));
    q->num_bits = _num_bits;
    q->bitref = 0;
    q->bitmask = (1<<(q->num_bits)) - 1;

    q->ref = 0.0f;
    q->zeta = _zeta;
    q->delta = 0.01f;
    q->delta_min = 0.01f;
    q->delta_max = 1.0f;

    return q;
}

void cvsd_destroy(cvsd _q)
{
    free(_q);
}

void cvsd_print(cvsd _q)
{
    printf("cvsd codec:\n");
    printf("    num bits: %u\n", _q->num_bits);
    printf("    zeta    : %8.4f\n", _q->zeta);
}

// encode single sample
unsigned char cvsd_encode(cvsd _q, float _audio_sample)
{
    // determine output value
    unsigned char bit = (_q->ref > _audio_sample) ? 0 : 1;

    // shift last value into buffer
    _q->bitref <<= 1;
    _q->bitref |= bit;
    _q->bitref &= _q->bitmask;

    // update delta
    if (_q->bitref == 0 || _q->bitref == _q->bitmask)
        _q->delta *= _q->zeta;  // increase delta
    else
        _q->delta /= _q->zeta;  // decrease delta

    // limit delta
    _q->delta = (_q->delta > _q->delta_max) ? _q->delta_max : _q->delta;
    _q->delta = (_q->delta < _q->delta_min) ? _q->delta_min : _q->delta;

    // update reference
    _q->ref += (bit) ? _q->delta : -_q->delta;

    // limit reference
    _q->ref = (_q->ref >  1.0f) ?  1.0f : _q->ref;
    _q->ref = (_q->ref < -1.0f) ? -1.0f : _q->ref;

    return bit;
}

// decode single sample
float cvsd_decode(cvsd _q, unsigned char _bit)
{
    // append bit into register
    _q->bitref <<= 1;
    _q->bitref |= (_bit & 0x01);
    _q->bitref &= _q->bitmask;

    // update delta
    if (_q->bitref == 0 || _q->bitref == _q->bitmask)
        _q->delta *= _q->zeta;  // increase delta
    else
        _q->delta /= _q->zeta;  // decrease delta

    // limit delta
    _q->delta = (_q->delta > _q->delta_max) ? _q->delta_max : _q->delta;
    _q->delta = (_q->delta < _q->delta_min) ? _q->delta_min : _q->delta;

    // update reference
    _q->ref += (_bit&0x01) ? _q->delta : -_q->delta;

    // limit reference
    _q->ref = (_q->ref >  1.0f) ?  1.0f : _q->ref;
    _q->ref = (_q->ref < -1.0f) ? -1.0f : _q->ref;

    return _q->ref;
}

// encode 8 samples
void cvsd_encode8(cvsd _q, float * _audio, unsigned char * _data)
{
    unsigned char data=0x00;
    unsigned int i;
    for (i=0; i<8; i++) {
        data <<= 1;
        data |= cvsd_encode(_q, _audio[i]);
    }

    // set return value
    *_data = data;
}

// decode 8 samples
void cvsd_decode8(cvsd _q, unsigned char _data, float * _audio)
{
    unsigned char bit;
    unsigned int i;
    for (i=0; i<8; i++) {
        bit = (_data >> (8-i-1)) & 0x01;
        _audio[i] = cvsd_decode(_q, bit);
    }
}

