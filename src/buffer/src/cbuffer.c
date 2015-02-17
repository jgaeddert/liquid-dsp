/*
 * Copyright (c) 2007 - 2015 Joseph Gaeddert
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

//
// circular buffer
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "liquid.internal.h"

// linearize buffer (if necessary)
void CBUFFER(_linearize)(CBUFFER() _q);

// cbuffer object
struct CBUFFER(_s) {
    // allocated memory array
    T * v;

    // length of buffer
    unsigned int max_size;

    // maximum number of elements that can be read at any given time
    unsigned int max_read;

    // number of elements allocated in memory
    unsigned int num_allocated;
    
    // number of elements currently in buffer
    unsigned int num_elements;
    
    // index to read
    unsigned int read_index;
    
    // index to write
    unsigned int write_index;
};

// create circular buffer object of a particular size
CBUFFER() CBUFFER(_create)(unsigned int _max_size)
{
    // create main object
    CBUFFER() q = CBUFFER(_create_max)(_max_size, _max_size);

    // return main object
    return q;
}

// create circular buffer object of a particular size and
// specify the maximum number of elements that can be read
// at any given time.
CBUFFER() CBUFFER(_create_max)(unsigned int _max_size,
                               unsigned int _max_read)
{
    // create main object
    CBUFFER() q = (CBUFFER()) malloc(sizeof(struct CBUFFER(_s)));

    // set internal properties
    q->max_size = _max_size;
    q->max_read = _max_read;

    // internal memory allocation
    q->num_allocated = q->max_size + q->max_read - 1;

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
    printf("cbuffer%s [max size: %u, max read: %u, elements: %u]\n",
            EXTENSION,
            _q->max_size,
            _q->max_read,
            _q->num_elements);

    unsigned int i;
    for (i=0; i<_q->num_elements; i++) {
        printf("%u", i);
        BUFFER_PRINT_LINE(_q,(_q->read_index+i)%(_q->max_size))
        printf("\n");
    }
}

// print cbuffer object properties and internal state
void CBUFFER(_debug_print)(CBUFFER() _q)
{
    printf("cbuffer%s [max size: %u, max read: %u, elements: %u]\n",
            EXTENSION,
            _q->max_size,
            _q->max_read,
            _q->num_elements);

    unsigned int i;
    for (i=0; i<_q->max_size; i++) {
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
    for (i=_q->max_size; i<_q->num_allocated; i++) {
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
    return _q->max_size;
}

// get the maximum number of elements that can be read from
// the buffer at any given time.
unsigned int CBUFFER(_max_read)(CBUFFER() _q)
{
    return _q->max_read;
}

// return number of elements available for writing
unsigned int CBUFFER(_space_available)(CBUFFER() _q)
{
    return _q->max_size - _q->num_elements;
}

// is buffer full?
int CBUFFER(_is_full)(CBUFFER() _q)
{
    return (_q->num_elements == _q->max_size ? 1 : 0);
}

// write a single sample into the buffer
//  _q  : circular buffer object
//  _v  : input sample
void CBUFFER(_push)(CBUFFER() _q,
                    T         _v)
{
    // ensure buffer isn't already full
    if (_q->num_elements == _q->max_size) {
        fprintf(stderr,"warning: cbuffer%s_push(), no space available\n",
                EXTENSION);
        return;
    }

    // add sample at write index
    _q->v[_q->write_index] = _v;

    // update write index
    _q->write_index = (_q->write_index+1) % _q->max_size;

    // increment number of elements
    _q->num_elements++;
}

// write samples to the buffer
//  _q  : circular buffer object
//  _v  : output array
//  _n  : number of samples to write
void CBUFFER(_write)(CBUFFER()    _q,
                     T *          _v,
                     unsigned int _n)
{
    // ensure number of samples to write doesn't exceed space available
    if (_n > (_q->max_size - _q->num_elements)) {
        printf("warning: cbuffer%s_write(), cannot write more elements than are available\n", EXTENSION);
        return;
    }

    _q->num_elements += _n;
    // space available at end of buffer
    unsigned int k = _q->max_size - _q->write_index;
    //printf("n : %u, k : %u\n", _n, k);

    // check for condition where we need to wrap around
    if (_n > k) {
        memmove(_q->v + _q->write_index, _v, k*sizeof(T));
        memmove(_q->v, &_v[k], (_n-k)*sizeof(T));
        _q->write_index = _n - k;
    } else {
        memmove(_q->v + _q->write_index, _v, _n*sizeof(T));
        _q->write_index += _n;
    }
}

// remove and return a single element from the buffer
//  _q  : circular buffer object
//  _v  : pointer to sample output
void CBUFFER(_pop)(CBUFFER()    _q,
                   T *          _v)
{
    // ensure there is at least one element
    if (_q->num_elements == 0) {
        fprintf(stderr,"warning: cbuffer%s_pop(), no elements available\n",
                EXTENSION);
        return;
    }

    // set return value
    if (_v != NULL)
        *_v = _q->v[ _q->read_index ];

    // increment read index
    _q->read_index = (_q->read_index + 1) % _q->max_size;

    // decrement number of elements in the buffer
    _q->num_elements--;
}

// read buffer contents
//  _q              : circular buffer object
//  _num_requested  : number of elements requested
//  _v              : output pointer
//  _nr             : number of elements referenced by _v
void CBUFFER(_read)(CBUFFER()      _q,
                    unsigned int   _num_requested,
                    T **           _v,
                    unsigned int * _num_read)
{
    // adjust number requested depending upon availability
    if (_num_requested > _q->num_elements)
        _num_requested = _q->num_elements;
    
    // restrict maximum number of elements to originally specified value
    if (_num_requested > _q->max_read)
        _num_requested = _q->max_read;

    // linearize tail end of buffer if necessary
    if (_num_requested > (_q->max_size - _q->read_index))
        CBUFFER(_linearize)(_q);
    
    // set output pointer appropriately
    *_v        = _q->v + _q->read_index;
    *_num_read = _num_requested;
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

    _q->read_index = (_q->read_index + _n) % _q->max_size;
    _q->num_elements -= _n;
}


//
// internal methods
//

// internal linearization
void CBUFFER(_linearize)(CBUFFER() _q)
{
#if 0
    // check to see if anything needs to be done
    if ( (_q->max_size - _q->read_index) > _q->num_elements)
        return;
#endif

    //printf("cbuffer linearize: [%6u : %6u], num elements: %6u, read index: %6u, write index: %6u\n",
    //        _q->max_size, _q->max_read-1, _q->num_elements, _q->read_index, _q->write_index);

    // move maximum amount
    memmove(_q->v + _q->max_size, _q->v, (_q->max_read-1)*sizeof(T));
}

