//
// Block interleaver, operates on bytes
//

#ifndef __LIQUID_INTERLEAVER_INTERNAL_H__
#define __LIQUID_INTERLEAVER_INTERNAL_H__

#include "interleaver.h"

struct interleaver_s {
    unsigned int * p;   // byte permutation
    unsigned int len;   // number of bytes
};

void interleaver_circshift_left(unsigned char *_x, unsigned int _n, unsigned int _s);
void interleaver_circshift_right(unsigned char *_x, unsigned int _n, unsigned int _s);
void interleaver_compute_bit_permutation(interleaver _q, unsigned int *_p);
void interleaver_debug_print(interleaver _i);

void interleaver_permute_forward(unsigned char * _x, unsigned int * _p, unsigned int _n, unsigned char _mask);
void interleaver_permute_backward(unsigned char * _x, unsigned int * _p, unsigned int _n, unsigned char _mask);

#endif // __LIQUID_INTERLEAVER_INTERNAL_H__
