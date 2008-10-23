//
// Cyclic redundancy check
// 

#include "fec_internal.h"

// slow implementation
unsigned char reverse_byte(unsigned char _x)
{
    unsigned char y = 0x00;
    unsigned int i;
    for (i=0; i<8; i++) {
        y <<= 1;
        y |= _x & 1;
        _x >>= 1;
    }
    return y;
}

unsigned int reverse_uint32(unsigned int _x)
{
    unsigned int i, y=0;
    for (i=0; i<32; i++) {
        y <<= 1;
        y |= _x & 1;
        _x >>= 1;
    }
    return y;
}

// slow method, operates one bit at a time
// algorithm from: http://www.hackersdelight.org/crc.pdf
unsigned int crc32_generate_key(unsigned char *_msg, unsigned int _n)
{
    unsigned int i, j, b, mask, key32=~0;
    unsigned int poly = reverse_uint32(CRC32_POLY);
    for (i=0; i<_n; i++) {
        b = _msg[i];
        key32 ^= b;
        for (j=0; j<8; j++) {
            mask = -(key32 & 1);
            key32 = (key32>>1) ^ (poly & mask);
        }
    }
    return ~key32;
}

#if 0
void crc32_generate_key(unsigned char *_msg, unsigned int _n, unsigned char *_key)
{
    unsigned int key32 = crc32_generate_key32(_msg,_n);
    _key[0] = (key32 & 0xFF000000) >> 24;
    _key[1] = (key32 & 0x00FF0000) >> 16;
    _key[2] = (key32 & 0x0000FF00) >> 8;
    _key[3] = (key32 & 0x000000FF);
}
#endif

bool crc32_validate_message(unsigned char *_msg, unsigned int _n, unsigned int _key)
{
    return crc32_generate_key(_msg,_n)==_key;
}
