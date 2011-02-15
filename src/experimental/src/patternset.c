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
// patternset.c : optimization pattern set
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "liquid.experimental.h"

// create pattern set
//  _num_inputs     :   number of inputs in the set
//  _num_outputs    :   number of output in the set
patternset patternset_create(unsigned int _num_inputs,
                             unsigned int _num_outputs)
{
    patternset q = (patternset) malloc(sizeof(struct patternset_s));
    q->num_inputs  = _num_inputs;
    q->num_outputs = _num_outputs;

    // set internal counters
    q->num_patterns = 0;
    q->num_allocated = 1;

    // allocate memory for input/output arrays
    q->x = (float*) malloc((q->num_allocated)*(q->num_inputs)*sizeof(float));
    q->y = (float*) malloc((q->num_allocated)*(q->num_outputs)*sizeof(float));

    return q;
}

// destroy pattern set object
void patternset_destroy(patternset _q)
{
    // free internally-allocated memory
    free(_q->x);
    free(_q->y);

    // free main object memory
    free(_q);
}

// print pattern set
void patternset_print(patternset _q)
{
    printf("optim pattern set [%u] :\n", _q->num_patterns);
    if (_q->num_patterns == 0) return;

    unsigned int n,i,ix=0,iy=0;
    for (n=0; n<_q->num_patterns; n++) {
        printf("  %u\t:", n);
        for (i=0; i<_q->num_inputs; i++)
            printf(" %8.5f", _q->x[ix++]);
        printf(" : ");
        for (i=0; i<_q->num_outputs; i++)
            printf(" %8.5f", _q->y[iy++]);
        printf("\n");
    }
}

// get number of patterns currently in the set
unsigned int patternset_get_num_patterns(patternset _q)
{
    return _q->num_patterns;
}

// append single pattern to set
//  _q      :   pattern set object
//  _x      :   input [size: _num_inputs x 1]
//  _y      :   output [size: _num_outputs x 1]
void patternset_append_pattern(patternset _q,
                               float * _x,
                               float * _y)
{
    // increase memory if necessary
    if (_q->num_allocated == _q->num_patterns)
        patternset_increase_mem(_q,4);

    // input, output write pointers
    float * wx = _q->x + _q->num_patterns * _q->num_inputs;
    float * wy = _q->y + _q->num_patterns * _q->num_outputs;

    // copy input to appropriate memory location
    memmove(wx, _x, (_q->num_inputs)*sizeof(float));
    memmove(wy, _y, (_q->num_outputs)*sizeof(float));
    _q->num_patterns++;
}

// append multiple patterns to the set
//  _q      :   pattern set object
//  _x      :   inputs [size: _num_inputs x _n]
//  _y      :   outputs [size: _num_outputs x _n]
//  _n      :   number of patterns to append
void patternset_append_patterns(patternset _q,
                                float * _x,
                                float * _y,
                                unsigned int _num_patterns)
{
    // increase memory if necessary
    if (_q->num_allocated < (_q->num_patterns + _num_patterns))
        patternset_increase_mem(_q, _num_patterns);

    // input, output write pointers
    float * wx = _q->x + _q->num_patterns * _q->num_inputs;
    float * wy = _q->y + _q->num_patterns * _q->num_outputs;

    // copy input to appropriate memory location
    memmove(wx, _x, (_q->num_inputs)*_num_patterns*sizeof(float));
    memmove(wy, _y, (_q->num_outputs)*_num_patterns*sizeof(float));
    _q->num_patterns += _num_patterns;
}

// remove pattern from set at index _i
void patternset_delete_pattern(patternset _q,
                               unsigned int _i)
{
    if (_i >= _q->num_patterns) {
        fprintf(stderr,"error: patternset_delete_pattern(), index exceeds available patterns\n");
        exit(1);
    }

    _q->num_patterns--;
    if (_q->num_patterns == _i)
        return;

    // shift input values back one slot
    unsigned int ix1 = (_q->num_inputs)*(_i);
    unsigned int ix2 = (_q->num_inputs)*(_i+1);
    memmove(&(_q->x[ix1]), &(_q->x[ix2]), (_q->num_inputs)*(_q->num_patterns - _i)*sizeof(float));

    // shift output values back one slot
    unsigned int iy1 = (_q->num_outputs)*(_i);
    unsigned int iy2 = (_q->num_outputs)*(_i+1);
    memmove(&(_q->y[iy1]), &(_q->y[iy2]), (_q->num_outputs)*(_q->num_patterns - _i)*sizeof(float));
}

// remove all patterns from the set
void patternset_clear(patternset _q)
{
    _q->num_patterns = 0;
}

// access a single pattern in the set
//  _q      :   pattern set object
//  _i      :   index of pattern
//  _x      :   input pointer
//  _y      :   output pointer
void patternset_access(patternset _q,
                       unsigned int _i,
                       float ** _x,
                       float ** _y)
{
    if (_i >= _q->num_patterns) {
        fprintf(stderr,"error: patternset_access(), index exceeds available patterns\n");
        exit(1);
    }

    // set output pointers accordingly
    *_x = &(_q->x[(_q->num_inputs)*_i]);
    *_y = &(_q->y[(_q->num_outputs)*_i]);
}


// access all patterns in the set
//  _q      :   pattern set object
//  _x      :   input pointer
//  _y      :   output pointer
void patternset_access_all(patternset _q,
                           float ** _x,
                           float ** _y)
{
    // set output pointers accordingly
    *_x = _q->x;    // inputs
    *_y = _q->y;    // outputs
}



// 
// internal methods
//

// increase memory size by _n slots
void patternset_increase_mem(patternset _q,
                             unsigned int _n)
{
    _q->num_allocated += _n;
    _q->x = (float*) realloc((void*)(_q->x), (_q->num_inputs)*(_q->num_allocated)*sizeof(float));
    _q->y = (float*) realloc((void*)(_q->y), (_q->num_outputs)*(_q->num_allocated)*sizeof(float));
}
