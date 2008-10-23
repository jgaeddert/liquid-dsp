//
// FEC, etc.
//

#ifndef __LIQUID_FEC_INTERNAL_H__
#define __LIQUID_FEC_INTERNAL_H__

#include <stdbool.h>
#include "fec.h"

// checksum

//
// CRC
//
#define CRC32_POLY 0x04C11DB7
extern unsigned char crc32_tab[256];
unsigned char reverse_byte(unsigned char _x);
unsigned int reverse_uint32(unsigned int _x);


#endif
