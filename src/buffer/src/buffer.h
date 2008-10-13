//
// Circular buffer
//

#ifndef __MODULE_BUFFER_H__
#define __MODULE_BUFFER_H__

typedef struct buffer_s * buffer;

typedef enum {
    CIRCULAR=0,
    STATIC
} buffer_type;

buffer buffer_create(buffer_type _type, unsigned int _n);

void buffer_destroy(buffer _cb);

void buffer_print(buffer _cb);

void buffer_debug_print(buffer _cb);

void buffer_clear(buffer _cb);

// read at least *_n elements, return actual number available
void buffer_read(buffer _cb, float ** _v, unsigned int *_n);

void buffer_release(buffer _cb, unsigned int _n);

void buffer_write(buffer _cb, float * _v, unsigned int _n);
//void buffer_force_write(buffer _cb, float * _v, unsigned int _n);

#endif // __MODULE_BUFFER_H__

