/*
 * Copyright (c) 2007 - 2014 Joseph Gaeddert
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
// auto-correlator (delay cross-correlation)
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

// defined:
//  AUTOCORR()      name-mangling macro
//  TI              type (input)
//  TC              type (coefficients)
//  TO              type (output)
//  WINDOW()        window macro
//  DOTPROD()       dotprod macro
//  PRINTVAL()      print macro

struct AUTOCORR(_s) {
    unsigned int window_size;
    unsigned int delay;

    WINDOW() w;         // input buffer
    WINDOW() wdelay;    // input buffer with delay

    float * we2;        // energy buffer
    float e2_sum;       // running sum of energy
    unsigned int ie2;   // read index
};

// create auto-correlator object                            
//  _window_size    : size of the correlator window         
//  _delay          : correlator delay [samples]            
AUTOCORR() AUTOCORR(_create)(unsigned int _window_size,
                             unsigned int _delay)
{
    // create main object
    AUTOCORR() q = (AUTOCORR()) malloc(sizeof(struct AUTOCORR(_s)));

    // set user-based parameters
    q->window_size = _window_size;
    q->delay       = _delay;

    // create window objects
    q->w      = WINDOW(_create)(q->window_size);
    q->wdelay = WINDOW(_create)(q->window_size + q->delay);

    // allocate array for squared energy buffer
    q->we2 = (float*) malloc( (q->window_size)*sizeof(float) );

    // clear object
    AUTOCORR(_reset)(q);

    // return main object
    return q;
}

// destroy auto-correlator object, freeing internal memory
void AUTOCORR(_destroy)(AUTOCORR() _q)
{
    // destroy internal window objects
    WINDOW(_destroy)(_q->w);
    WINDOW(_destroy)(_q->wdelay);

    // free array for squared energy buffer
    free(_q->we2);

    // free main object memory
    free(_q);
}

// reset auto-correlator object's internals
void AUTOCORR(_reset)(AUTOCORR() _q)
{
    // clear/reset internal window buffers
    WINDOW(_clear)(_q->w);
    WINDOW(_clear)(_q->wdelay);
    
    // reset internal squared energy buffer
    _q->e2_sum = 0.0;
    unsigned int i;
    for (i=0; i<_q->window_size; i++)
        _q->we2[i] = 0.0;
    _q->ie2 = 0;    // reset read index to zero
}

// print auto-correlator parameters to stdout
void AUTOCORR(_print)(AUTOCORR() _q)
{
    printf("autocorr [%u window, %u delay]\n", _q->window_size, _q->delay);
}

// push sample into auto-correlator object
void AUTOCORR(_push)(AUTOCORR() _q, TI _x)
{
    // push input sample into buffers
    WINDOW(_push)(_q->w,      _x);          // non-delayed buffer
    WINDOW(_push)(_q->wdelay, conj(_x));    // delayed buffer

    // push |_x|^2 into buffer at appropriate location
    float e2 = creal( _x*conj(_x) );
    _q->e2_sum -= _q->we2[ _q->ie2 ];
    _q->e2_sum += e2;
    _q->we2[ _q->ie2 ] = e2;
    _q->ie2 = (_q->ie2+1) % _q->window_size;
}

// compute auto-correlation output
void AUTOCORR(_execute)(AUTOCORR() _q, TO *_rxx)
{
    // provide pointers for reading buffer
    TI * rw;        // input buffer read pointer
    TC * rwdelay;   // input buffer read pointer (with delay)

    // read buffers; set internal pointers appropriately
    WINDOW(_read)(_q->w,      &rw     );
    WINDOW(_read)(_q->wdelay, &rwdelay);

    // execute vector dot product on arrays, saving result to
    // user-supplied output pointer
    DOTPROD(_run4)(rw, rwdelay, _q->window_size, _rxx);
}

// compute auto-correlation on block of samples; the input
// and output arrays may have the same pointer
//  _q      :   auto-correlation object
//  _x      :   input array [size: _n x 1]
//  _n      :   number of input, output samples
//  _rxx    :   input array [size: _n x 1]
void AUTOCORR(_execute_block)(AUTOCORR()   _q,
                              TI *         _x,
                              unsigned int _n,
                              TO *         _rxx)
{
    unsigned int i;
    for (i=0; i<_n; i++) {
        // push input sample into auto-correlator
        AUTOCORR(_push)(_q, _x[i]);

        // compute output
        AUTOCORR(_execute)(_q, &_rxx[i]);
    }
}

// return sum of squares of buffered samples
float AUTOCORR(_get_energy)(AUTOCORR() _q)
{
    // value is already computed; simply return value
    return _q->e2_sum;
}

