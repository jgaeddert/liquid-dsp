//
// Block interleaver, operates on bytes
//

#ifndef __LIQUID_INTERLEAVER_INTERNAL_H__
#define __LIQUID_INTERLEAVER_INTERNAL_H__

#include "liquid.h"

struct interleaver_s {
    unsigned int * p;   // byte permutation
    unsigned int len;   // number of bytes

    unsigned char * t;  // temporary buffer
};

//void interleaver_circshift_left(unsigned char *_x, unsigned int _n, unsigned int _s);
//void interleaver_circshift_right(unsigned char *_x, unsigned int _n, unsigned int _s);
void interleaver_compute_bit_permutation(interleaver _q, unsigned int *_p);


// 
// permutation functions
//
void interleaver_permute_forward(unsigned char * _x, unsigned int * _p, unsigned int _n);
void interleaver_permute_reverse(unsigned char * _x, unsigned int * _p, unsigned int _n);

void interleaver_permute_forward_mask(unsigned char * _x, unsigned int * _p, unsigned int _n, unsigned char _mask);
void interleaver_permute_reverse_mask(unsigned char * _x, unsigned int * _p, unsigned int _n, unsigned char _mask);

void interleaver_circshift_L4(unsigned char *_x, unsigned int _n);
void interleaver_circshift_R4(unsigned char *_x, unsigned int _n);

#endif // __LIQUID_INTERLEAVER_INTERNAL_H__
