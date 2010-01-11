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
// Optim / pattern set
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "liquid.internal.h"

// create pattern set object
optim_ps optim_ps_create(unsigned int _num_inputs,
                         unsigned int _num_outputs)
{
    optim_ps ps = (optim_ps) malloc(sizeof(struct optim_ps_s));
    ps->num_inputs  = _num_inputs;
    ps->num_outputs = _num_outputs;
    ps->num_patterns = 0;
    ps->num_allocated = 1;
    ps->x = (float*) malloc((ps->num_allocated)*(ps->num_inputs)*sizeof(float));
    ps->y = (float*) malloc((ps->num_allocated)*(ps->num_outputs)*sizeof(float));

    return ps;
}

// destroy pattern set object
void optim_ps_destroy(optim_ps _ps)
{
    free(_ps->x);
    free(_ps->y);
    free(_ps);
}

// print pattern set
void optim_ps_print(optim_ps _ps)
{
    printf("optim pattern set [%u] :\n", _ps->num_patterns);
    if (_ps->num_patterns == 0) return;

    unsigned int n,i,ix=0,iy=0;
    for (n=0; n<_ps->num_patterns; n++) {
        printf("  %u\t:", n);
        for (i=0; i<_ps->num_inputs; i++)
            printf(" %8.5f", _ps->x[ix++]);
        printf(" : ");
        for (i=0; i<_ps->num_outputs; i++)
            printf(" %8.5f", _ps->y[iy++]);
        printf("\n");
    }
}

// append single pattern to set
void optim_ps_append_pattern(optim_ps _ps, float *_x, float *_y)
{
    // increase memory if necessary
    if (_ps->num_allocated == _ps->num_patterns)
        optim_ps_increase_mem(_ps,4);

    // intput, output write pointers
    float * wx = _ps->x + _ps->num_patterns * _ps->num_inputs;
    float * wy = _ps->y + _ps->num_patterns * _ps->num_outputs;

    // copy input to appropriate memory location
    memmove(wx, _x, (_ps->num_inputs)*sizeof(float));
    memmove(wy, _y, (_ps->num_outputs)*sizeof(float));
    _ps->num_patterns++;
}

// append multiple patterns to set
void optim_ps_append_patterns(optim_ps _ps, float *_x, float *_y, unsigned int _num_patterns)
{
    // increase memory if necessary
    if (_ps->num_allocated < (_ps->num_patterns + _num_patterns))
        optim_ps_increase_mem(_ps, _num_patterns);

    // intput, output write pointers
    float * wx = _ps->x + _ps->num_patterns * _ps->num_inputs;
    float * wy = _ps->y + _ps->num_patterns * _ps->num_outputs;

    // copy input to appropriate memory location
    memmove(wx, _x, (_ps->num_inputs)*_num_patterns*sizeof(float));
    memmove(wy, _y, (_ps->num_outputs)*_num_patterns*sizeof(float));
    _ps->num_patterns += _num_patterns;
}

void optim_ps_delete_pattern(optim_ps _ps, unsigned int _i)
{
    if (_i > _ps->num_patterns) {
        printf("error: optim_ps_delete_pattern(), index exceeds available patterns\n");
        exit(1);
    }

    _ps->num_patterns--;
    if (_ps->num_patterns == _i)
        return;

    unsigned int ix1 = (_ps->num_inputs)*(_i);
    unsigned int ix2 = (_ps->num_inputs)*(_i+1);
    memmove(&(_ps->x[ix1]), &(_ps->x[ix2]), (_ps->num_inputs)*(_ps->num_patterns - _i)*sizeof(float));

    unsigned int iy1 = (_ps->num_outputs)*(_i);
    unsigned int iy2 = (_ps->num_outputs)*(_i+1);
    memmove(&(_ps->y[iy1]), &(_ps->y[iy2]), (_ps->num_outputs)*(_ps->num_patterns - _i)*sizeof(float));
}

// clear pattern sets
void optim_ps_clear(optim_ps _ps)
{
    _ps->num_patterns = 0;
}

// access pattern set
void optim_ps_access(optim_ps _ps, unsigned int _i, float **_x, float **_y)
{
    if (_i > _ps->num_patterns) {
        printf("error: optim_ps_access(), index exceeds available patterns\n");
        exit(1);
    }

    *_x = &(_ps->x[(_ps->num_inputs)*_i]);
    *_y = &(_ps->y[(_ps->num_outputs)*_i]);
}

// protected

// increase memory size
void optim_ps_increase_mem(optim_ps _ps, unsigned int _n)
{
    _ps->num_allocated += _n;
    _ps->x = (float*) realloc((void*)(_ps->x), (_ps->num_inputs)*(_ps->num_allocated)*sizeof(float));
    _ps->y = (float*) realloc((void*)(_ps->y), (_ps->num_outputs)*(_ps->num_allocated)*sizeof(float));
}
