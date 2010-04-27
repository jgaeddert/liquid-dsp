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

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#include "metadata.h"

// Defined
//  PORT()      :   name-mangling macro
//  T           :   data type
//  BUFFER()    :   buffer macro

struct PORT(_s) {
    gport gp;
};


PORT() PORT(_create)(unsigned int _n)
{
    PORT() p = (PORT()) malloc(sizeof(struct PORT(_s)));
    p->gp = gport_create(_n,sizeof(T));

    return p;
}

void PORT(_destroy)(PORT() _p)
{
    gport_destroy(_p->gp);
    free(_p);
}

void PORT(_print)(PORT() _p)
{
    gport_print(_p->gp);
}

void PORT(_produce)(PORT() _p, T * _w, unsigned int _n)
{
    gport_produce(_p->gp,(void*)_w,_n);
}

void PORT(_consume)(PORT() _p, T * _r, unsigned int _n)
{
    gport_consume(_p->gp,(void*)_r,_n);
}


