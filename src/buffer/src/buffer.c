//
//
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "buffer_internal.h"

struct X(_s) {
    T * v;
    unsigned int len;
    unsigned int N;
    unsigned int num_elements;

    unsigned int read_index;
    unsigned int write_index;

    buffer_type type;

    // mutex/semaphore
};

X() X(_create)(buffer_type _type, unsigned int _n)
{
    X() b = (X()) malloc(sizeof(struct X(_s)));
    b->type = _type;
    b->len = _n;

    if (b->type == CIRCULAR)
        b->N = 2*(b->len) - 1;
    else
        b->N = b->len;

    b->v = (T*) malloc((b->N)*sizeof(T));
    b->num_elements = 0;
    b->read_index = 0;
    b->write_index = 0;

    return b;
}

void X(_destroy)(X() _b)
{
    free(_b->v);
    free(_b);
}

void X(_print)(X() _b)
{
    if (_b->type == CIRCULAR)
        printf("circular ");
    else
        printf("static ");
    printf("X() [%u elements] :\n", _b->num_elements);
    unsigned int i;
    for (i=0; i<_b->num_elements; i++) {
        printf("%u", i);
        BUFFER_PRINT_LINE(_b,i)
        printf("\n");
    }
}

void X(_debug_print)(X() _b)
{
    if (_b->type == CIRCULAR)
        printf("circular ");
    else
        printf("static ");
    printf("X() [%u elements] :\n", _b->num_elements);
    unsigned int i;
    for (i=0; i<_b->len; i++) {
        // print read index pointer
        if (i==_b->read_index)
            printf("<r>");

        // print write index pointer
        if (i==_b->write_index)
            printf("<w>");

        // print X() value
        //printf("\t%u\t: %f\n", i, _b->v[i]);
        BUFFER_PRINT_LINE(_b,i)
        printf("\n");
    }
    printf("----------------------------------\n");

    // print excess X() memory
    for (i=_b->len; i<_b->N; i++) {
        //printf("\t%u\t: %f\n", i, _b->v[i]);
        BUFFER_PRINT_LINE(_b,i)
    }
}

void X(_clear)(X() _b)
{
    _b->read_index = 0;
    _b->write_index = 0;
    _b->num_elements = 0;
}

void X(_read)(X() _b, T ** _v, unsigned int *_n)
{
    if (_b->type == CIRCULAR)
        X(_c_read)(_b, _v, _n);
    else
        X(_s_read)(_b, _v, _n);
}

void X(_c_read)(X() _b, T ** _v, unsigned int *_n)
{
    printf("buffer_read() trying to read %u elements (%u available)\n", *_n, _b->num_elements);
    if (*_n > _b->num_elements) {
        printf("error: buffer_read(), cannot read more elements than are available\n");
        *_v = NULL;
        *_n = 0;
        return;
    } else if (*_n > (_b->len - _b->read_index)) {
        //
        X(_linearize)(_b);
    }
    *_v = _b->v + _b->read_index;
    *_n = _b->num_elements;
}

void X(_s_read)(X() _b, T ** _v, unsigned int *_n)
{
    printf("buffer_s_read() reading %u elements\n", _b->num_elements);
    *_v = _b->v;
    *_n = _b->num_elements;
}

void X(_release)(X() _b, unsigned int _n)
{
    if (_b->type == CIRCULAR)
        X(_c_release)(_b, _n);
    else
        X(_s_release)(_b, _n);
}


void X(_c_release)(X() _b, unsigned int _n)
{
    // advance read_index by _n making sure not to step on write_index
    if (_n > _b->num_elements) {
        printf("error: buffer_c_release(), cannot release more elements in X() than exist\n");
        return;
    }

    _b->read_index = (_b->read_index + _n) % _b->len;
    _b->num_elements -= _n;
}


void X(_s_release)(X() _b, unsigned int _n)
{
    X(_clear)(_b);
}

void X(_write)(X() _b, T * _v, unsigned int _n)
{
    if (_b->type == CIRCULAR)
        X(_c_write)(_b, _v, _n);
    else
        X(_s_write)(_b, _v, _n);
}

void X(_c_write)(X() _b, T * _v, unsigned int _n)
{
    //
    if (_n > (_b->len - _b->num_elements)) {
        printf("error: buffer_write(), cannot write more elements than are available\n");
        return;
    }

    _b->num_elements += _n;
    // space available at end of buffer
    unsigned int k = _b->len - _b->write_index;
    //printf("n : %u, k : %u\n", _n, k);

    // check for condition where we need to wrap around
    if (_n > k) {
        memcpy(_b->v + _b->write_index, _v, k*sizeof(T));
        memcpy(_b->v, &_v[k], (_n-k)*sizeof(T));
        _b->write_index = _n - k;
    } else {
        memcpy(_b->v + _b->write_index, _v, _n*sizeof(T));
        _b->write_index += _n;
    }
}

void X(_s_write)(X() _b, T * _v, unsigned int _n)
{
    if (_n > (_b->len - _b->num_elements)) {
        printf("error: buffer_s_write(), cannot write more elements than are available\n");
        return;
    }

    memcpy(_b->v + _b->num_elements, _v, _n*sizeof(T));
    _b->num_elements += _n;
}

//void X(_force_write)(X() _b, T * _v, unsigned int _n)

void X(_linearize)(X() _b)
{
    // check to see if anything needs to be done
    if ( (_b->len - _b->read_index) > _b->num_elements)
        return;

    // perform memory copy
    memcpy(_b->v + _b->len, _b->v, (_b->write_index)*sizeof(T));
}

