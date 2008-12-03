//
//
//

#include <stdio.h>
#include <stdlib.h>

#include "newbuffer.h"

// create/destroy/print methods
// NOTE _n is suggested starting size; buffer will realloc when necessary
buffer buffer_create(unsigned int _n)
{
    buffer b = (buffer) malloc(sizeof(struct buffer_s));
    b->len = _n;
    b->N = 4*(b->len);

    b->v = (float*) malloc((b->N)*sizeof(float));

    return b;
}

void buffer_destroy(buffer _b)
{
    free(_b->v);
    free(_b);
}

void buffer_print(buffer _b)
{

}

void buffer_debug_print(buffer _b)
{

}

// accessor methods
unsigned int buffer_get_length(buffer _b)
{
    return _b->len;
}

// consumer methods
int buffer_consumer_lock_array(buffer _b, unsigned int *_n)
{
    *_n = 0;
    return 0;
}

int buffer_consumer_read(buffer _b, float *_v, unsigned int _n)
{
    
    return 0;
}

int buffer_consumer_release_array(buffer _b)
{

    return 0;
}

// producer methods
int buffer_producer_lock_array(buffer _b, unsigned int *_n)
{
    *_n = 0;
    return 0;
}

int buffer_producer_write(buffer _b, float *_v, unsigned int _n)
{

    return 0;
}

int buffer_producer_release_array(buffer _b)
{

    return 0;
}

