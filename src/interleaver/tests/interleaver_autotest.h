#ifndef __INTERLEAVER_AUTOTEST_H__
#define __INTERLEAVER_AUTOTEST_H__

#include <stdlib.h>

#include "../../../autotest/autotest.h"
#include "../src/interleaver.h"

// 
// AUTOTEST: interleave/deinterleave
//
void autotest_interleaver_general()
{
    unsigned int i, n = 8;
    unsigned char x[n];
    unsigned char y[n];
    unsigned char z[n];

    for (i=0; i<n; i++)
        x[i] = rand() & 0xFF;

    interleaver q = interleaver_create(n, INT_BLOCK);

    interleaver_interleave(q,x,y);
    interleaver_deinterleave(q,y,z);

    CONTEND_SAME_DATA(x, z, n);
}

#endif 

