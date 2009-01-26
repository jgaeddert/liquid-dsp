//
// Index of most-significant bit
//
// Examples:
//  0x00000000  :   0
//  0x00000001  :   1
//  0x00000002  :   2
//  ...
//  0x00010000  :   17
//  0x00020000  :   17
//  0x00040000  :   17
//  ...
//  0x80000000  :   32
//

#include "utility.h"

unsigned int msb_index(unsigned int _x)
{
    unsigned int bits;

#if defined __i386__ || defined __amd64__ || defined __x86_64__
    if (!_x) return 0;
    __asm volatile("bsrl %1,%0\n"
        : "=r" (bits)
        : "c" (_x)
    );
    return bits + 1;
#elif 0
    // slow method; look one bit at a time
    for (bits = 0; _x != 0 && bits < 32; _x >>= 1, ++bits)
        ;
    return bits;
#else
    // look for first non-zero byte
    unsigned int i, b;
    bits = 8*SIZEOF_UNSIGNED_INT;
    for (i=SIZEOF_UNSIGNED_INT*8; i>0; i-=8) {
        b = (_x >> (i-8)) & 0xFF;
        if ( b )
            return bits - leading_zeros[b];
        else
            bits -= 8;
    }
    return 0;

#endif
}


