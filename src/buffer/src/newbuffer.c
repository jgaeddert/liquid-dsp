//
//
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "newbuffer.h"

#ifndef DEBUG
#define DEBUG
#endif

// create/destroy/print methods
// NOTE _n is suggested starting size; buffer will realloc when necessary
buffer buffer_create(unsigned int _n)
{
    buffer b = (buffer) malloc(sizeof(struct buffer_s));
    b->len = _n;
    //b->N = 2*(b->len) - 1;
    b->N = b->len;

    b->v = (float*) malloc((b->N)*sizeof(float));
    b->read_index = 0;
    b->write_index = 0;
    b->num_elements = 0;

    b->read_lock = false;
    b->write_lock = false;

    b->num_read_elements_available = 0;
    b->num_write_elements_available = 0;

    b->num_read_elements_locked = 0 ;
    b->num_write_elements_locked = 0;

    return b;
}

void buffer_destroy(buffer _b)
{
    free(_b->v);
    free(_b);
}

void buffer_print(buffer _b)
{
    printf("buffer [%u elements] :\n", _b->num_elements);
    unsigned int i;
    for (i=0; i<_b->num_elements; i++) {
        printf("  %3u : %f\n", i, _b->v[ (_b->read_index+i) % _b->len]);
    }
}

void buffer_debug_print(buffer _b)
{
    printf("buffer [%u elements] :\n", _b->num_elements);
    unsigned int i;
    for (i=0; i<_b->len; i++) {
        if (i==_b->read_index && i==_b->write_index)
            printf(" <r/w>");
        else if (i==_b->read_index)
            printf(" <r>  ");
        else if (i==_b->write_index)
            printf(" <w>  ");
        else
            printf("      ");

        printf("%3u : %f\n", i, _b->v[i]);
    }
}

// accessor methods
unsigned int buffer_get_length(buffer _b)
{
    return _b->len;
}

// consumer methods
int buffer_consumer_lock_array(buffer _b, unsigned int *_n)
{
    // try to lock array
    if (_b->read_lock) {
        printf("error: buffer_consumer_lock_array(), array locked\n");
        return error_buffer_locked;
    }

    _b->read_lock = true;

    _b->num_read_elements_available = (_b->write_index > _b->read_index) ?
        _b->write_index - _b->read_index :
        _b->len - (_b->read_index - _b->write_index);
    *_n = _b->num_read_elements_available;

    // linearize?

#ifdef DEBUG
    printf("consumer locked %u elements for reading\n", *_n);
#endif
    return 0;
}

int buffer_consumer_read(buffer _b, float *_v, unsigned int _n)
{
    if (_n > _b->num_read_elements_available) {
        printf("error: buffer_consumer_read(), not enough elements in buffer\n");
        return error_buffer_overread;
    }

    // linearize?

    //memcpy(_v, _b->v + _b->read_index, _n*sizeof(float));
    unsigned int i;
    for (i=0; i<_n; i++) {
        _v[i] = _b->v[_b->read_index];
        _b->read_index++;
        _b->read_index = (_b->read_index) % (_b->len);
    }
    _b->num_read_elements_available -= _n;
    _b->num_elements -= _n;

#ifdef DEBUG
    printf("consumer read %u elements\n", _n);
#endif
    return 0;
}

int buffer_consumer_release_array(buffer _b)
{
    if (!_b->read_lock) {
        printf("error: buffer_consumer_release_array(), array unlocked\n");
        return error_buffer_unlocked;
    }

    _b->read_lock = false;
#ifdef DEBUG
    printf("consumer released lock read\n");
#endif
    return 0;
}

// producer methods
int buffer_producer_lock_array(buffer _b, unsigned int *_n)
{
    if (_b->write_lock) {
        printf("error: buffer_producer_lock_array(), array is locked\n");
        return error_buffer_locked;
    }

    _b->write_lock = true;

    _b->num_write_elements_available = (_b->read_index > _b->write_index) ?
        _b->read_index - _b->write_index :
        _b->len - (_b->write_index - _b->read_index);

    *_n = _b->num_write_elements_available;
#ifdef DEBUG
    printf("producer locked %u elements for writing\n", *_n);
#endif
    return 0;
}

int buffer_producer_write(buffer _b, float *_v, unsigned int _n)
{
    if (_n > _b->num_write_elements_available) {
        printf("error: buffer_producer_write(), buffer over-written\n");
        return error_buffer_overwrite;
    }

    unsigned int i;
    for (i=0; i<_n; i++) {
        _b->v[_b->write_index] = _v[i];
        _b->write_index++;
        _b->write_index = (_b->write_index) % (_b->len);
    }
    _b->num_write_elements_available -= _n;
    _b->num_elements += _n;
#ifdef DEBUG
    printf("producer wrote %u elements (%u available)\n", _n, _b->num_write_elements_available);
#endif
    return 0;
}

int buffer_producer_release_array(buffer _b)
{
    if (!_b->write_lock) {
        printf("error: buffer_producer_release_array(), buffer already released\n");
        return error_buffer_unlocked;
    }

    _b->write_lock = false;

#ifdef DEBUG
    printf("producer released write lock\n");
#endif
    return 0;
}

