//
// Circular buffer
//

#ifndef __LIQUID_BUFFER_INTERNAL_H__
#define __LIQUID_BUFFER_INTERNAL_H__

#include "buffer.h"

#define buffer_fast_access(c,i) (c->v[(c->read_index+i)%(c->len)])
void buffer_linearize(buffer _b);

void buffer_c_read(buffer _b, float ** _v, unsigned int * _n);
void buffer_s_read(buffer _b, float ** _v, unsigned int * _n);

void buffer_c_write(buffer _b, float * _v, unsigned int _n);
void buffer_s_write(buffer _b, float * _v, unsigned int _n);

#endif // __LIQUID_BUFFER_INTERNAL_H__

