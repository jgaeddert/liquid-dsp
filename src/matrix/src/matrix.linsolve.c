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
// Solve linear system of equations
//

#include <string.h>

#include "liquid.internal.h"

// 
void MATRIX(_linsolve)(T * _A,
                       unsigned int _n,
                       T * _b,
                       T * _x,
                       void * _opts)
{
    T A_inv[_n*_n];
    memmove(A_inv, _A, _n*_n*sizeof(T));
    MATRIX(_inv)(A_inv,_n,_n);

    MATRIX(_mul)(A_inv, _n, _n,
                 _b,    _n,  1,
                 _x,    _n,  1);
}

