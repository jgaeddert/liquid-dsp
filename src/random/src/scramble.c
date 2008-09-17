//
// Finite impulse response filter design
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "scramble.h"

void scramble_data(unsigned char * _x, unsigned int _len)
{
    // for now apply static mask
    unsigned char mask = 0xb4;
    unsigned int i;
    for (i=0; i<_len; i++)
        _x[i] ^= mask;
}

void unscramble_data(unsigned char * _x, unsigned int _len)
{
    // for now apply simple static mask (re-run scramble)
    scramble_data(_x,_len);
}

