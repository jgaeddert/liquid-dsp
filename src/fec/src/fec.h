//
// FEC, etc.
//

#ifndef __LIQUID_FEC_H__
#define __LIQUID_FEC_H__

#include <stdbool.h>

//
// checksum
//
unsigned char checksum_generate_key(unsigned char *_data, unsigned int _n);
bool checksum_validate(unsigned char *_data, unsigned int _n, unsigned char _key);

//
// CRC
//
void crc32_generate_key(unsigned char *_data, unsigned int _n, unsigned char * _key);
bool crc32_validate(unsigned char *_data, unsigned int _n, unsigned char * _key);



#endif
