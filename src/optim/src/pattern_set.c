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

optim_ps optim_ps_create(unsigned int _nx, unsigned int _ny)
{
    optim_ps ps = (optim_ps) malloc(sizeof(struct optim_ps_s));
    ps->nx = _nx;
    ps->ny = _ny;
    ps->np = 0;
    ps->na = 1;
    ps->x = (float*) malloc((ps->na)*(ps->nx)*sizeof(float));
    ps->y = (float*) malloc((ps->na)*(ps->ny)*sizeof(float));

    return ps;
}

void optim_ps_destroy(optim_ps _ps)
{
    free(_ps->x);
    free(_ps->y);
    free(_ps);
}

void optim_ps_print(optim_ps _ps)
{
    printf("optim pattern set [%u] :\n", _ps->np);
    if (_ps->np == 0) return;

    unsigned int n,i,ix=0,iy=0;
    for (n=0; n<_ps->np; n++) {
        printf("  %u\t:", n);
        for (i=0; i<_ps->nx; i++)
            printf(" %8.5f", _ps->x[ix++]);
        printf(" : ");
        for (i=0; i<_ps->ny; i++)
            printf(" %8.5f", _ps->y[iy++]);
        printf("\n");
    }
}

void optim_ps_append_pattern(optim_ps _ps, float *_x, float *_y)
{
    if (_ps->na == _ps->np)
        optim_ps_increase_mem(_ps,4);

    memmove(_ps->x + _ps->np*_ps->nx, _x, (_ps->nx)*sizeof(float));
    memmove(_ps->y + _ps->np*_ps->ny, _y, (_ps->ny)*sizeof(float));
    _ps->np++;
}

void optim_ps_append_patterns(optim_ps _ps, float *_x, float *_y, unsigned int _np)
{
    if (_ps->na < (_ps->np + _np))
        optim_ps_increase_mem(_ps, _np);

    memmove(_ps->x + _ps->np*_ps->nx, _x, (_ps->nx)*_np*sizeof(float));
    memmove(_ps->y + _ps->np*_ps->ny, _y, (_ps->ny)*_np*sizeof(float));
    _ps->np += _np;
}

void optim_ps_delete_pattern(optim_ps _ps, unsigned int _i)
{
    if (_i > _ps->np) {
        printf("error: optim_ps_delete_pattern(), index exceeds available patterns\n");
        exit(1);
    }

    _ps->np--;
    if (_ps->np == _i)
        return;

    unsigned int ix1 = (_ps->nx)*(_i);
    unsigned int ix2 = (_ps->nx)*(_i+1);
    memmove(&(_ps->x[ix1]), &(_ps->x[ix2]), (_ps->nx)*(_ps->np - _i)*sizeof(float));

    unsigned int iy1 = (_ps->ny)*(_i);
    unsigned int iy2 = (_ps->ny)*(_i+1);
    memmove(&(_ps->y[iy1]), &(_ps->y[iy2]), (_ps->ny)*(_ps->np - _i)*sizeof(float));
}
void optim_ps_clear(optim_ps _ps)
{
    _ps->np = 0;
}

void optim_ps_access(optim_ps _ps, unsigned int _i, float **_x, float **_y)
{
    if (_i > _ps->np) {
        printf("error: optim_ps_access(), index exceeds available patterns\n");
        exit(1);
    }

    *_x = &(_ps->x[(_ps->nx)*_i]);
    *_y = &(_ps->y[(_ps->ny)*_i]);
}

// protected

// increase memory size
void optim_ps_increase_mem(optim_ps _ps, unsigned int _n)
{
    _ps->na += _n;
    _ps->x = (float*) realloc((void*)(_ps->x), (_ps->nx)*(_ps->na)*sizeof(float));
    _ps->y = (float*) realloc((void*)(_ps->y), (_ps->ny)*(_ps->na)*sizeof(float));
}
