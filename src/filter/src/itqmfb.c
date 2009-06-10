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
// iterative tree quadrature mirror filterbank (ITQMFB)
//
// References:
//   Vaidyanathan, P. P., "Multirate Systems and Filter Banks,"
//   1993, Prentice Hall, Section 5.8
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

// defined:
//  ITQMFB()        name-mangling macro
//  TO              output data type
//  TC              coefficient data type
//  TI              input data type
//  QMFB()          quadrature mirror filter bank (qmfb) macro
//  WINDOW()        window macro
//  DOTPROD()       dotprod macro
//  PRINTVAL()      print macro

struct ITQMFB(_s) {
    float * h;          // filter prototype
    unsigned int m;     // primitive filter length
    unsigned int h_len; // actual filter length: h_len = 4*m+1
    float beta;         // filter bandwidth/sidelobe suppression

    unsigned int num_layers;
    unsigned int num_banks;

    // QMFB input/output buffer(s)
    TI * buffer;
    unsigned int buffer_len;

    QMFB() * bank;
};

ITQMFB() ITQMFB(_create)(unsigned int _n, unsigned int _m, float _slsl)
{
    ITQMFB() f = (ITQMFB()) malloc(sizeof(struct ITQMFB(_s)));

    f->num_layers = _n;
    f->num_banks  = 1<<(_n-1);

    // allocate memory for banks
    f->bank = (QMFB()*) malloc((f->num_banks)*sizeof(QMFB()));

    // allocate memory for buffers
    f->buffer = (TI*) malloc((1)*sizeof(TI));

    // create banks
    unsigned int i;
    for (i=0; i<f->num_banks; i++)
        f->bank[i] = QMFB(_create)(_m, _slsl);

    return f;
}

ITQMFB() ITQMFB(_recreate)(ITQMFB() _f, unsigned int _h_len)
{
    // TODO implement this method
    printf("error: itqmfb_xxxt_recreate(), method not supported yet\n");
    exit(1);
    return NULL;
}

void ITQMFB(_destroy)(ITQMFB() _f)
{
    // destroy banks
    unsigned int i;
    for (i=0; i<_f->num_banks; i++)
        QMFB(_destroy)(_f->bank[i]);
    free(_f->bank);
    free(_f->buffer);
    free(_f);
}

void ITQMFB(_print)(ITQMFB() _f)
{
    printf("iterative tree quadrature mirror filterbank:\n");
}

void ITQMFB(_clear)(ITQMFB() _f)
{
    unsigned int i;
    for (i=0; i<_f->num_banks; i++)
        QMFB(_clear)(_f->bank[i]);
    for (i=0; i<_f->buffer_len; i++)
        _f->buffer[i] = 0;
}

void ITQMFB(_analysis_execute)(ITQMFB() _q,
                             TI   _x0,
                             TI   _x1,
                             TO * _y0,
                             TO * _y1)
{
}

void ITQMFB(_synthesis_execute)(ITQMFB() _q,
                              TI   _x0,
                              TI   _x1,
                              TO * _y0,
                              TO * _y1)
{
}

