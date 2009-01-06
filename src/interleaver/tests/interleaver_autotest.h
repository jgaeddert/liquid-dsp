#ifndef __INTERLEAVER_AUTOTEST_H__
#define __INTERLEAVER_AUTOTEST_H__

#include <stdlib.h>

#include "../../../autotest/autotest.h"
#include "../src/interleaver.h"

// 
// AUTOTESTS: interleave/deinterleave
//
void intlv_test(unsigned int _n, int _type)
{
    unsigned int i;
    unsigned char x[_n];
    unsigned char y[_n];
    unsigned char z[_n];

    for (i=0; i<_n; i++)
        x[i] = rand() & 0xFF;

    interleaver q = interleaver_create(_n, _type);

    interleaver_interleave(q,x,y);
    interleaver_deinterleave(q,y,z);

    CONTEND_SAME_DATA(x, z, _n);
}

void autotest_interleaver_block_8()         { intlv_test(8,     INT_BLOCK); }
void autotest_interleaver_block_16()        { intlv_test(16,    INT_BLOCK); }
void autotest_interleaver_block_64()        { intlv_test(64,    INT_BLOCK); }
void autotest_interleaver_block_256()       { intlv_test(256,   INT_BLOCK); }

void autotest_interleaver_sequence_8()      { intlv_test(8,     INT_SEQUENCE); }
void autotest_interleaver_sequence_16()     { intlv_test(16,    INT_SEQUENCE); }
void autotest_interleaver_sequence_64()     { intlv_test(64,    INT_SEQUENCE); }
void autotest_interleaver_sequence_256()    { intlv_test(256,   INT_SEQUENCE); }

#endif 

