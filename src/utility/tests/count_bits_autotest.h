#ifndef __COUNT_BITS_AUTOTEST_H__
#define __COUNT_BITS_AUTOTEST_H__

#include "../../../autotest/autotest.h"
#include "../src/utility.h"

// 
// AUTOTEST: count number of ones in an integer
//
void autotest_count_ones() {
    CONTEND_EQUALITY( count_ones(0x0000), 0 );
    CONTEND_EQUALITY( count_ones(0x0001), 1 );
    CONTEND_EQUALITY( count_ones(0x0003), 2 );
    CONTEND_EQUALITY( count_ones(0xFFFF), 16 );
    CONTEND_EQUALITY( count_ones(0x00FF), 8 );
    CONTEND_EQUALITY( count_ones(0x5555), 8 );
}

// 
// AUTOTEST: count number of leading zeros in an integer
//
void autotest_count_leading_zeros() {
    // NOTE: this tests assumes a 4-byte integer

    CONTEND_EQUALITY( count_leading_zeros(0x00000000), 32 );

    CONTEND_EQUALITY( count_leading_zeros(0x00000001), 31 );
    CONTEND_EQUALITY( count_leading_zeros(0x00000002), 30 );
    CONTEND_EQUALITY( count_leading_zeros(0x00000004), 29 );
    CONTEND_EQUALITY( count_leading_zeros(0x00000008), 28 );

    CONTEND_EQUALITY( count_leading_zeros(0x00000010), 27 );
    CONTEND_EQUALITY( count_leading_zeros(0x00000020), 26 );
    CONTEND_EQUALITY( count_leading_zeros(0x00000040), 25 );
    CONTEND_EQUALITY( count_leading_zeros(0x00000080), 24 );

    CONTEND_EQUALITY( count_leading_zeros(0x00000100), 23 );
    CONTEND_EQUALITY( count_leading_zeros(0x00000200), 22 );
    CONTEND_EQUALITY( count_leading_zeros(0x00000400), 21 );
    CONTEND_EQUALITY( count_leading_zeros(0x00000800), 20 );

    CONTEND_EQUALITY( count_leading_zeros(0x00001000), 19 );
    CONTEND_EQUALITY( count_leading_zeros(0x00002000), 18 );
    CONTEND_EQUALITY( count_leading_zeros(0x00004000), 17 );
    CONTEND_EQUALITY( count_leading_zeros(0x00008000), 16 );

    CONTEND_EQUALITY( count_leading_zeros(0x00010000), 15 );
    CONTEND_EQUALITY( count_leading_zeros(0x00020000), 14 );
    CONTEND_EQUALITY( count_leading_zeros(0x00040000), 13 );
    CONTEND_EQUALITY( count_leading_zeros(0x00080000), 12 );

    CONTEND_EQUALITY( count_leading_zeros(0x00100000), 11 );
    CONTEND_EQUALITY( count_leading_zeros(0x00200000), 10 );
    CONTEND_EQUALITY( count_leading_zeros(0x00400000),  9 );
    CONTEND_EQUALITY( count_leading_zeros(0x00800000),  8 );

    CONTEND_EQUALITY( count_leading_zeros(0x01000000),  7 );
    CONTEND_EQUALITY( count_leading_zeros(0x02000000),  6 );
    CONTEND_EQUALITY( count_leading_zeros(0x04000000),  5 );
    CONTEND_EQUALITY( count_leading_zeros(0x08000000),  4 );

    CONTEND_EQUALITY( count_leading_zeros(0x10000000),  3 );
    CONTEND_EQUALITY( count_leading_zeros(0x20000000),  2 );
    CONTEND_EQUALITY( count_leading_zeros(0x40000000),  1 );
    CONTEND_EQUALITY( count_leading_zeros(0x80000000),  0 );
}

// 
// AUTOTEST: find location of most-significant bit
//
void autotest_msb_index() {
    // NOTE: this tests assumes a 4-byte integer

    CONTEND_EQUALITY( msb_index(0x00000000),  0 );

    CONTEND_EQUALITY( msb_index(0x00000001),  1 );
    CONTEND_EQUALITY( msb_index(0x00000002),  2 );
    CONTEND_EQUALITY( msb_index(0x00000004),  3 );
    CONTEND_EQUALITY( msb_index(0x00000008),  4 );

    CONTEND_EQUALITY( msb_index(0x00000010),  5 );
    CONTEND_EQUALITY( msb_index(0x00000020),  6 );
    CONTEND_EQUALITY( msb_index(0x00000040),  7 );
    CONTEND_EQUALITY( msb_index(0x00000080),  8 );

    CONTEND_EQUALITY( msb_index(0x00000100),  9 );
    CONTEND_EQUALITY( msb_index(0x00000200), 10 );
    CONTEND_EQUALITY( msb_index(0x00000400), 11 );
    CONTEND_EQUALITY( msb_index(0x00000800), 12 );

    CONTEND_EQUALITY( msb_index(0x00001000), 13 );
    CONTEND_EQUALITY( msb_index(0x00002000), 14 );
    CONTEND_EQUALITY( msb_index(0x00004000), 15 );
    CONTEND_EQUALITY( msb_index(0x00008000), 16 );

    CONTEND_EQUALITY( msb_index(0x00010000), 17 );
    CONTEND_EQUALITY( msb_index(0x00020000), 18 );
    CONTEND_EQUALITY( msb_index(0x00040000), 19 );
    CONTEND_EQUALITY( msb_index(0x00080000), 20 );

    CONTEND_EQUALITY( msb_index(0x00100000), 21 );
    CONTEND_EQUALITY( msb_index(0x00200000), 22 );
    CONTEND_EQUALITY( msb_index(0x00400000), 23 );
    CONTEND_EQUALITY( msb_index(0x00800000), 24 );

    CONTEND_EQUALITY( msb_index(0x01000000), 25 );
    CONTEND_EQUALITY( msb_index(0x02000000), 26 );
    CONTEND_EQUALITY( msb_index(0x04000000), 27 );
    CONTEND_EQUALITY( msb_index(0x08000000), 28 );

    CONTEND_EQUALITY( msb_index(0x10000000), 29 );
    CONTEND_EQUALITY( msb_index(0x20000000), 30 );
    CONTEND_EQUALITY( msb_index(0x40000000), 31 );
    CONTEND_EQUALITY( msb_index(0x80000000), 32 );
}


#endif // __COUNT_BITS_AUTOTEST_H__

