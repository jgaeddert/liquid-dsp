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
// circular buffer
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "liquid.internal.h"

//
void CBUFFER(_linearize)(CBUFFER() _q);

//
struct CBUFFER(_s) {
    T * v;                      // allocated memory array
    unsigned int len;           // length of buffer
    unsigned int num_allocated; // number of elements allocated in memory
    unsigned int num_elements;  // number of elements currently in buffer
    unsigned int read_index;    // index to read
    unsigned int write_index;   // index to write
};

// create circular buffer object of a particular size
CBUFFER() CBUFFER(_create)(unsigned int _n)
{
    // create main object
    CBUFFER() q = (CBUFFER()) malloc(sizeof(struct CBUFFER(_s)));

    // set internal properties
    q->len = _n;

    // internal memory allocation
    q->num_allocated = 2*(q->len) - 1;

    // allocate internal memory array
    q->v = (T*) malloc((q->num_allocated)*sizeof(T));

    // reset object
    CBUFFER(_clear)(q);

    // return main object
    return q;
}

// destroy cbuffer object, freeing all internal memory
void CBUFFER(_destroy)(CBUFFER() _q)
{
    // free internal memory
    free(_q->v);

    // free main object
    free(_q);
}

// print cbuffer object properties
void CBUFFER(_print)(CBUFFER() _q)
{
    printf("cbuffer%s [size: %u, elements: %u]\n",
            EXTENSION,
            _q->len,
            _q->num_elements);

    unsigned int i;
    for (i=0; i<_q->num_elements; i++) {
        printf("%u", i);
        BUFFER_PRINT_LINE(_q,(_q->read_index+i)%(_q->len))
        printf("\n");
    }
}

// print cbuffer object properties and internal state
void CBUFFER(_debug_print)(CBUFFER() _q)
{
    printf("cbuffer%s [size: %u, elements: %u]\n",
            EXTENSION,
            _q->len,
            _q->num_elements);

    unsigned int i;
    for (i=0; i<_q->len; i++) {
        // print read index pointer
        if (i==_q->read_index)
            printf("<r>");
        else
            printf("   ");

        // print write index pointer
        if (i==_q->write_index)
            printf("<w>");
        else
            printf("   ");

        // print buffer value
        BUFFER_PRINT_LINE(_q,i)
        printf("\n");
    }
    printf("----------------------------------\n");

    // print excess buffer memory
    for (i=_q->len; i<_q->num_allocated; i++) {
        printf("      ");
        BUFFER_PRINT_LINE(_q,i)
        printf("\n");
    }
}

// clear internal buffer
void CBUFFER(_clear)(CBUFFER() _q)
{
    _q->read_index   = 0;
    _q->write_index  = 0;
    _q->num_elements = 0;
}

// get the number of elements currently in the buffer
unsigned int CBUFFER(_size)(CBUFFER() _q)
{
    return _q->num_elements;
}

// get the maximum number of elements the buffer can hold
unsigned int CBUFFER(_max_size)(CBUFFER() _q)
{
    return _q->len;
}

// read buffer contents
//  _q  : circular buffer object
//  _v  : output pointer
//  _nr : number of elements referenced by _v
void CBUFFER(_read)(CBUFFER()      _q,
                    T **           _v,
                    unsigned int * _n)
{
    // linearize tail end of buffer if necessary
    if (*_n > (_q->len - _q->read_index))
        CBUFFER(_linearize)(_q);
    
    // set output pointer appropriately
    *_v = _q->v + _q->read_index;
    *_n = _q->num_elements;
}

// release _n samples in the buffer
void CBUFFER(_release)(CBUFFER()    _q,
                       unsigned int _n)
{
    // advance read_index by _n making sure not to step on write_index
    if (_n > _q->num_elements) {
        printf("error: cbuffer%s_release(), cannot release more elements in buffer than exist\n", EXTENSION);
        return;
    }

    _q->read_index = (_q->read_index + _n) % _q->len;
    _q->num_elements -= _n;
}


// write samples to the buffer
//  _q  : circular buffer object
//  _v  : output array
//  _n  : number of samples to write
void CBUFFER(_write)(CBUFFER()    _q,
                     T *          _v,
                     unsigned int _n)
{
    //
    if (_n > (_q->len - _q->num_elements)) {
        printf("error: cbuffer%s_write(), cannot write more elements than are available\n", EXTENSION);
        return;
    }

    _q->num_elements += _n;
    // space available at end of buffer
    unsigned int k = _q->len - _q->write_index;
    //printf("n : %u, k : %u\n", _n, k);

    // check for condition where we need to wrap around
    if (_n > k) {
        memcpy(_q->v + _q->write_index, _v, k*sizeof(T));
        memcpy(_q->v, &_v[k], (_n-k)*sizeof(T));
        _q->write_index = _n - k;
    } else {
        memcpy(_q->v + _q->write_index, _v, _n*sizeof(T));
        _q->write_index += _n;
    }
}

// write a single sample into the buffer
//  _q  : circular buffer object
//  _v  : input sample
void CBUFFER(_push)(CBUFFER() _q,
                    T         _v)
{
    _q->v[_q->write_index] = _v;
    if (_q->num_elements < _q->len) {
        _q->num_elements++;
    } else {
        _q->read_index = (_q->read_index+1) % _q->len;
    }
    _q->write_index = (_q->write_index+1) % _q->len;
}

// internal linearization
void CBUFFER(_linearize)(CBUFFER() _q)
{
    // check to see if anything needs to be done
    if ( (_q->len - _q->read_index) > _q->num_elements)
        return;

    // perform memory copy
    memcpy(_q->v + _q->len, _q->v, (_q->write_index)*sizeof(T));
}
