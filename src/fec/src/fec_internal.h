//
// FEC, etc.
//

#ifndef __LIQUID_FEC_INTERNAL_H__
#define __LIQUID_FEC_INTERNAL_H__

#include <stdbool.h>
#include "fec.h"

extern unsigned char c_ones_mod2[256];

// checksum

//
// CRC
//
#define CRC32_POLY 0x04C11DB7
extern unsigned char crc32_tab[256];
unsigned char reverse_byte(unsigned char _x);
unsigned int reverse_uint32(unsigned int _x);

// 
// Hamming(8,4)
//
unsigned char fec_hamming84_compute_syndrome(unsigned char _r);
unsigned char fec_hamming84_decode_symbol(unsigned char _r);

// 
// Hamming(7,4)
//
unsigned char fec_hamming74_compute_syndrome(unsigned char _r);

#endif
