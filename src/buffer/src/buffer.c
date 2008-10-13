//
//
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "buffer_internal.h"

struct buffer_s {
    float * v;
    unsigned int len;
    unsigned int N;
    unsigned int num_elements;

    unsigned int read_index;
    unsigned int write_index;

    buffer_type type;

    // mutex/semaphore
};

buffer buffer_create(buffer_type _type, unsigned int _n)
{
    buffer b = (buffer) malloc(sizeof(struct buffer_s));
    b->type = _type;
    b->len = _n;

    if (b->type == CIRCULAR)
        b->N = 2*(b->len) - 1;
    else
        b->N = b->len;

    b->v = (float*) malloc((b->N)*sizeof(float));
    b->num_elements = 0;
    b->read_index = 0;
    b->write_index = 0;

    return b;
}

void buffer_destroy(buffer _b)
{
    free(_b->v);
    free(_b);
}

void buffer_print(buffer _b)
{
    if (_b->type == CIRCULAR)
        printf("circular ");
    else
        printf("static ");
    printf("buffer [%u elements] :\n", _b->num_elements);
    unsigned int i;
    for (i=0; i<_b->num_elements; i++) {
        printf("%u\t: %f\n", i, buffer_fast_access(_b,i));
    }
}

void buffer_debug_print(buffer _b)
{
    if (_b->type == CIRCULAR)
        printf("circular ");
    else
        printf("static ");
    printf("buffer [%u elements] :\n", _b->num_elements);
    unsigned int i;
    for (i=0; i<_b->len; i++) {
        // print read index pointer
        if (i==_b->read_index)
            printf("<r>");

        // print write index pointer
        if (i==_b->write_index)
            printf("<w>");

        // print buffer value
        printf("\t%u\t: %f\n", i, _b->v[i]);
    }
    printf("----------------------------------\n");

    // print excess buffer memory
    for (i=_b->len; i<_b->N; i++) {
        printf("\t%u\t: %f\n", i, _b->v[i]);
    }
}

void buffer_clear(buffer _b)
{
    _b->read_index = 0;
    _b->write_index = 0;
    _b->num_elements = 0;
}

void buffer_read(buffer _b, float ** _v, unsigned int *_n)
{
    if (_b->type == CIRCULAR)
        buffer_c_read(_b, _v, _n);
    else
        buffer_s_read(_b, _v, _n);
}

void buffer_c_read(buffer _b, float ** _v, unsigned int *_n)
{
    printf("buffer_read() trying to read %u elements (%u available)\n", *_n, _b->num_elements);
    if (*_n > _b->num_elements) {
        printf("error: buffer_read(), cannot read more elements than are available\n");
        *_v = NULL;
        *_n = 0;
        return;
    } else if (*_n > (_b->len - _b->read_index)) {
        //
        buffer_linearize(_b);
    }
    *_v = _b->v + _b->read_index;
    *_n = _b->num_elements;
}

void buffer_s_read(buffer _b, float ** _v, unsigned int *_n)
{
    printf("buffer_s_read() reading %u elements\n", _b->num_elements);
    *_v = _b->v;
    *_n = _b->num_elements;
}

void buffer_release(buffer _b, unsigned int _n)
{
    // advance read_index by _n making sure not to step on write_index
    if (_n > _b->num_elements) {
        printf("error: buffer_release(), cannot release more elements in buffer than exist\n");
        return;
    }

    _b->read_index = (_b->read_index + _n) % _b->len;
    _b->num_elements -= _n;
}

void buffer_write(buffer _b, float * _v, unsigned int _n)
{
    if (_b->type == CIRCULAR)
        buffer_c_write(_b, _v, _n);
    else
        buffer_s_write(_b, _v, _n);
}

void buffer_c_write(buffer _b, float * _v, unsigned int _n)
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
        memcpy(_b->v + _b->write_index, _v, k*sizeof(float));
        memcpy(_b->v, &_v[k], (_n-k)*sizeof(float));
        _b->write_index = _n - k;
    } else {
        memcpy(_b->v + _b->write_index, _v, _n*sizeof(float));
        _b->write_index += _n;
    }
}

void buffer_s_write(buffer _b, float * _v, unsigned int _n)
{
    if (_n > (_b->len - _b->num_elements)) {
        printf("error: buffer_s_write(), cannot write more elements than are available\n");
        return;
    }

    memcpy(_b->v + _b->num_elements, _v, _n*sizeof(float));
    _b->num_elements += _n;
}

//void buffer_force_write(buffer _b, float * _v, unsigned int _n)

void buffer_linearize(buffer _b)
{
    // check to see if anything needs to be done
    if ( (_b->len - _b->read_index) > _b->num_elements)
        return;

    // perform memory copy
    memcpy(_b->v + _b->len, _b->v, (_b->write_index)*sizeof(float));
}

