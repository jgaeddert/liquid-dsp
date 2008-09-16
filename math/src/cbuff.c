//
// Circular buffer
//

#include <math.h>
#include <stdlib.h>

#include "cbuff.h"

// additional function prototypes

// circular buffer struct object
struct cbuff_s {
    float * v;                  // buffer array
    unsigned int v_len;         // length of array

    unsigned int buff_len;      // length of buffer
    unsigned int num_elements;  // number of elements in buffer

    float * read_ptr;           // read pointer
    float * write_ptr;          // write pointer

    // mutex/semaphore
};

// Create circular buffer object
cbuff cbuff_create(unsigned int _len)
{
    cbuff cb = (cbuff) malloc(sizeof(struct cbuff_s));
    cb->v_len = _len;
    cb->buff_len = 2*(cb->v_len);
    cb->num_elements = 0;

    cb->v = (float*) malloc( (cb->v_len)*sizeof(float) );

    cb->read_ptr = cb->v;
    cb->write_ptr = cb->v;

    return cb;
}

// Destroy half-band decimator object
void cbuff_destroy(cbuff _cb)
{
    free(_cb->v);
    free(_cb);
}

// Read _n elements
float * cbuff_read(cbuff _cb, unsigned int _n)
{
    return NULL;
}

// Release _n elements
void cbuff_release(cbuff _cb, unsigned int _n)
{
}

// Write _n elements
int cbuff_write(cbuff _cb, float * _x, unsigned int _n)
{
    return 0;
}

