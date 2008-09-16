//
// Circular buffer
//

#ifndef __MODULE_CBUFFER_H__
#define __MODULE_CBUFFER_H__

typedef struct cbuffer_s * cbuffer;

cbuffer cbuffer_create(unsigned int _n);

void cbuffer_destroy(cbuffer _cb);

void cbuffer_print(cbuffer _cb);

void cbuffer_debug_print(cbuffer _cb);

void cbuffer_clear(cbuffer _cb);

// read at least *_n elements, return actual number available
void cbuffer_read(cbuffer _cb, float ** _v, unsigned int *_n);

void cbuffer_release(cbuffer _cb, unsigned int _n);

void cbuffer_write(cbuffer _cb, float * _v, unsigned int _n);
//void cbuffer_force_write(cbuffer _cb, float * _v, unsigned int _n);

#endif // __MODULE_CBUFFER_H__

