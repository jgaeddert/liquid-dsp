//
//
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "cbuffer.h"

struct cbuffer_s {
    float * v;
    unsigned int len;
    unsigned int N;
    unsigned int num_elements;

    unsigned int read_index;
    unsigned int write_index;

    // mutex/semaphore
};

#define cbuffer_fast_access(c,i) (c->v[(c->read_index+i)%(c->len)])
void cbuffer_linearize(cbuffer _cb);

cbuffer cbuffer_create(unsigned int _n)
{
    cbuffer cb = (cbuffer) malloc(sizeof(struct cbuffer_s));
    cb->len = _n;
    cb->N = 2*(cb->len) - 1;

    cb->v = (float*) malloc((cb->N)*sizeof(float));
    cb->num_elements = 0;
    cb->read_index = 0;
    cb->write_index = 0;

    return cb;
}

void cbuffer_destroy(cbuffer _cb)
{
    free(_cb->v);
    free(_cb);
}

void cbuffer_print(cbuffer _cb)
{
    printf("buffer [%u elements] :\n", _cb->num_elements);
    unsigned int i;
    for (i=0; i<_cb->num_elements; i++) {
        printf("%u\t: %f\n", i, cbuffer_fast_access(_cb,i));
    }
}

void cbuffer_debug_print(cbuffer _cb)
{
    printf("buffer [%u elements] :\n", _cb->num_elements);
    unsigned int i;
    for (i=0; i<_cb->len; i++) {
        // print read index pointer
        if (i==_cb->read_index)
            printf("<r>");

        // print write index pointer
        if (i==_cb->write_index)
            printf("<w>");

        // print buffer value
        printf("\t%u\t: %f\n", i, _cb->v[i]);
    }
    printf("----------------------------------\n");

    // print excess buffer memory
    for (i=_cb->len; i<_cb->N; i++) {
        printf("\t%u\t: %f\n", i, _cb->v[i]);
    }
}

void cbuffer_clear(cbuffer _cb)
{
    _cb->read_index = 0;
    _cb->write_index = 0;
    _cb->num_elements = 0;
}

void cbuffer_read(cbuffer _cb, float ** _v, unsigned int *_n)
{
    printf("cbuffer_read() trying to read %u elements (%u available)\n", *_n, _cb->num_elements);
    if (*_n > _cb->num_elements) {
        printf("error: cbuffer_read(), cannot read more elements than are available\n");
        *_v = NULL;
        *_n = 0;
        return;
    } else if (*_n > (_cb->len - _cb->read_index)) {
        //
        cbuffer_linearize(_cb);
    }
    *_v = _cb->v + _cb->read_index;
    *_n = _cb->num_elements;
}

void cbuffer_release(cbuffer _cb, unsigned int _n)
{
    // advance read_index by _n making sure not to step on write_index
    if (_n > _cb->num_elements) {
        printf("error: cbuffer_release(), cannot release more elements in buffer than exist\n");
        return;
    }

    _cb->read_index = (_cb->read_index + _n) % _cb->len;
    _cb->num_elements -= _n;
}

void cbuffer_write(cbuffer _cb, float * _v, unsigned int _n)
{
    //
    if (_n > (_cb->len - _cb->num_elements)) {
        printf("error: cbuffer_write(), cannot write more elements than are available\n");
        return;
    }

#if 0
    // TODO implement this without the loop using memmove
    unsigned int i;
    for (i=0; i<_n; i++) {
        cbuffer_fast_access(_cb,_cb->num_elements) = _v[i];
        _cb->num_elements++;
        _cb->write_index++;
        if (_cb->write_index == _cb->len)
            _cb->write_index = 0;

    }
#else // same as above but without the loop

    _cb->num_elements += _n;
    // space available at end of buffer
    unsigned int k = _cb->len - _cb->write_index;
    //printf("n : %u, k : %u\n", _n, k);

    // check for condition where we need to wrap around
    if (_n > k) {
        memcpy(_cb->v + _cb->write_index, _v, k*sizeof(float));
        memcpy(_cb->v, &_v[k], (_n-k)*sizeof(float));
        _cb->write_index = _n - k;
    } else {
        memcpy(_cb->v + _cb->write_index, _v, _n*sizeof(float));
        _cb->write_index += _n;
    }
#endif
}

//void cbuffer_force_write(cbuffer _cb, float * _v, unsigned int _n)

void cbuffer_linearize(cbuffer _cb)
{
    // check to see if anything needs to be done
    if ( (_cb->len - _cb->read_index) > _cb->num_elements)
        return;

    // perform memory copy
    memcpy(_cb->v + _cb->len, _cb->v, (_cb->write_index)*sizeof(float));
}

