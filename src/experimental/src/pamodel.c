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
// 
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.internal.h"

struct pamodel_s {
    float alpha;
};

pamodel pamodel_create(float _alpha)
{
    pamodel q = (pamodel) malloc(sizeof(struct pamodel_s));

    q->alpha = _alpha;

    return q;
}

void pamodel_destroy(pamodel _q)
{
    free(_q);
}

void pamodel_print(pamodel _q)
{
    printf("pamodel:\n");
}

void pamodel_execute(pamodel _q, float complex _x, float complex *_y)
{
    float theta = cargf(_x);
    float r     = cabsf(_x);
    *_y = (cosf(theta) + _Complex_I*sinf(theta))*tanhf(r * _q->alpha) / (_q->alpha);
}

