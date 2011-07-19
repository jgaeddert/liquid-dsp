/*
 * Copyright (c) 2007, 2009, 2011 Joseph Gaeddert
 * Copyright (c) 2007, 2009, 2011 Virginia Polytechnic Institute & State University
 *
 * This file is part of liquid.
 *
 * liquid is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * liquid is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with liquid.  If not, see <http://www.gnu.org/licenses/>.
 */

//
// cyclic redundancy check (and family)
// 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "liquid.internal.h"


// object-independent methods

const char * crc_scheme_str[LIQUID_CRC_NUM_SCHEMES][2] = {
    // short name,  long name
    {"unknown",     "unknown"},
    {"none",        "none"},
    {"checksum",    "checksum (8-bit)"},
    {"crc8",        "CRC (8-bit)"},
    {"crc16",       "CRC (16-bit)"},
    {"crc24",       "CRC (24-bit)"},
    {"crc32",       "CRC (32-bit)"}
};


// Print compact list of existing and available crc schemes
void liquid_print_crc_schemes()
{
    unsigned int i;
    unsigned int len = 10;

    // print all available MOD schemes
    printf("          ");
    for (i=0; i<LIQUID_CRC_NUM_SCHEMES; i++) {
        printf("%s", crc_scheme_str[i][0]);

        if (i != LIQUID_CRC_NUM_SCHEMES-1)
            printf(", ");

        len += strlen(crc_scheme_str[i][0]);
        if (len > 48 && i != LIQUID_CRC_NUM_SCHEMES-1) {
            len = 10;
            printf("\n          ");
        }
    }
    printf("\n");
}

crc_scheme liquid_getopt_str2crc(const char * _str)
{
    // compare each string to short name
    unsigned int i;
    for (i=0; i<LIQUID_CRC_NUM_SCHEMES; i++) {
        if (strcmp(_str,crc_scheme_str[i][0])==0) {
            return i;
        }
    }

    fprintf(stderr,"warning: liquid_getopt_str2crc(), unknown/unsupported crc scheme : %s\n", _str);
    return LIQUID_CRC_UNKNOWN;
}

// get length of CRC (bytes)
unsigned int crc_get_length(crc_scheme _scheme)
{
    switch (_scheme) {
    case LIQUID_CRC_UNKNOWN:   return 0;
    case LIQUID_CRC_NONE:      return 0;
    case LIQUID_CRC_CHECKSUM:  return 1;
    case LIQUID_CRC_8:         return 1;
    case LIQUID_CRC_16:        return 2;
    case LIQUID_CRC_24:        return 3;
    case LIQUID_CRC_32:        return 4;
    default:
        fprintf(stderr,"error: crc_get_length(), unknown/unsupported scheme: %d\n", _scheme);
        exit(1);
    }

    return 0;
}

// generate error-detection key
//
//  _scheme     :   error-detection scheme
//  _msg        :   input data message, [size: _n x 1]
//  _n          :   input data message size
unsigned int crc_generate_key(crc_scheme _scheme,
                              unsigned char * _msg,
                              unsigned int _n)
{
    switch (_scheme) {
    case LIQUID_CRC_UNKNOWN:
        fprintf(stderr,"error: crc_generate_key(), cannot generate key with CRC type \"UNKNOWN\"\n");
        exit(-1);
    case LIQUID_CRC_NONE:      return 0;
    case LIQUID_CRC_CHECKSUM:  return checksum_generate_key(_msg, _n);
    case LIQUID_CRC_8:         return crc8_generate_key(_msg, _n);
    case LIQUID_CRC_16:        return crc16_generate_key(_msg, _n);
    case LIQUID_CRC_24:        return crc24_generate_key(_msg, _n);
    case LIQUID_CRC_32:        return crc32_generate_key(_msg, _n);
    default:
        fprintf(stderr,"error: crc_generate_key(), unknown/unsupported scheme: %d\n", _scheme);
        exit(1);
    }

    return 0;
}

// 
// byte reversal
//

// reverse byte table
const unsigned char liquid_reverse_byte[256] = {
    0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0,
    0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0,
    0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8,
    0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8,
    0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4,
    0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4,
    0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec,
    0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc,
    0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2,
    0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2,
    0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea,
    0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa,
    0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6,
    0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6,
    0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee,
    0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe,
    0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1,
    0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1,
    0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9,
    0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9,
    0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5,
    0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5,
    0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed,
    0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd,
    0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3,
    0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3,
    0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb,
    0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb,
    0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7,
    0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7,
    0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef,
    0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff};

// slow implementation of byte reversal
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

// reverse integer with 16 bits of data
unsigned int reverse_uint16(unsigned int _x)
{
    return (liquid_reverse_byte[(_x     ) & 0xff] << 8) |
           (liquid_reverse_byte[(_x >> 8) & 0xff]     );
}

// reverse integer with 24 bits of data
unsigned int reverse_uint24(unsigned int _x)
{
    return (liquid_reverse_byte[(_x      ) & 0xff] << 16) |
           (liquid_reverse_byte[(_x >>  8) & 0xff] <<  8) |
           (liquid_reverse_byte[(_x >> 16) & 0xff]      );
}

// reverse integer with 32 bits of data
unsigned int reverse_uint32(unsigned int _x)
{
    return (liquid_reverse_byte[(_x      ) & 0xff] << 24) |
           (liquid_reverse_byte[(_x >>  8) & 0xff] << 16) |
           (liquid_reverse_byte[(_x >> 16) & 0xff] <<  8) |
           (liquid_reverse_byte[(_x >> 24) & 0xff]      );
}




// validate message using error-detection key
//
//  _scheme     :   error-detection scheme
//  _msg        :   input data message, [size: _n x 1]
//  _n          :   input data message size
//  _key        :   error-detection key
int crc_validate_message(crc_scheme _scheme,
                         unsigned char * _msg,
                         unsigned int _n,
                         unsigned int _key)
{
    if (_scheme == LIQUID_CRC_UNKNOWN) {
        fprintf(stderr,"error: crc_validate_message(), cannot validate with CRC type \"UNKNOWN\"\n");
        exit(-1);
    } else if (_scheme == LIQUID_CRC_NONE) {
        return 1;
    }

    return crc_generate_key(_scheme, _msg, _n) == _key;
}


//
// Checksum
//

// generate 8-bit checksum key
//
//  _scheme     :   error-detection scheme
//  _msg        :   input data message, [size: _n x 1]
//  _n          :   input data message size
unsigned int checksum_generate_key(unsigned char *_data,
                                   unsigned int _n)
{
    unsigned int i, sum=0;
    for (i=0; i<_n; i++)
        sum += (unsigned int) (_data[i]);
    //sum &= 0x00ff;

    // mask and convert to 2's complement
    unsigned char key = ~(sum&0x00ff) + 1;

    return key;
}


// 
// CRC-8
//

// generate 8-bit cyclic redundancy check key.
//
// slow method, operates one bit at a time
// algorithm from: http://www.hackersdelight.org/crc.pdf
//
//  _msg    :   input data message [size: _n x 1]
//  _n      :   input data message size
unsigned int crc8_generate_key(unsigned char *_msg,
                               unsigned int _n)
{
    unsigned int i, j, b, mask, key8=~0;
    unsigned int poly = liquid_reverse_byte[CRC8_POLY];
    for (i=0; i<_n; i++) {
        b = _msg[i];
        key8 ^= b;
        for (j=0; j<8; j++) {
            mask = -(key8 & 1);
            key8 = (key8>>1) ^ (poly & mask);
        }
    }
    return (~key8) & 0xff;
}


// 
// CRC-16
//

// generate 16-bit cyclic redundancy check key.
//
// slow method, operates one bit at a time
// algorithm from: http://www.hackersdelight.org/crc.pdf
//
//  _msg    :   input data message [size: _n x 1]
//  _n      :   input data message size
unsigned int crc16_generate_key(unsigned char *_msg,
                                unsigned int _n)
{
    unsigned int i, j, b, mask, key16=~0;
    unsigned int poly = reverse_uint16(CRC16_POLY);
    for (i=0; i<_n; i++) {
        b = _msg[i];
        key16 ^= b;
        for (j=0; j<8; j++) {
            mask = -(key16 & 1);
            key16 = (key16>>1) ^ (poly & mask);
        }
    }
    return (~key16) & 0xffff;
}


// 
// CRC-24
//

// generate 24-bit cyclic redundancy check key.
//
// slow method, operates one bit at a time
// algorithm from: http://www.hackersdelight.org/crc.pdf
//
//  _msg    :   input data message [size: _n x 1]
//  _n      :   input data message size
unsigned int crc24_generate_key(unsigned char *_msg,
                                unsigned int _n)
{
    unsigned int i, j, b, mask, key24=~0;
    unsigned int poly = reverse_uint24(CRC24_POLY);
    for (i=0; i<_n; i++) {
        b = _msg[i];
        key24 ^= b;
        for (j=0; j<8; j++) {
            mask = -(key24 & 1);
            key24 = (key24>>1) ^ (poly & mask);
        }
    }
    return (~key24) & 0xffffff;
}


// 
// CRC-32
//

// generate 32-bit cyclic redundancy check key.
//
// slow method, operates one bit at a time
// algorithm from: http://www.hackersdelight.org/crc.pdf
//
//  _msg    :   input data message [size: _n x 1]
//  _n      :   input data message size
unsigned int crc32_generate_key(unsigned char *_msg,
                                unsigned int _n)
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
    return (~key32) & 0xffffffff;
}

#if 0
void crc32_generate_key(unsigned char *_msg,
                        unsigned int _n,
                        unsigned char *_key)
{
    unsigned int key32 = crc32_generate_key32(_msg,_n);
    _key[0] = (key32 & 0xFF000000) >> 24;
    _key[1] = (key32 & 0x00FF0000) >> 16;
    _key[2] = (key32 & 0x0000FF00) >> 8;
    _key[3] = (key32 & 0x000000FF);
}
#endif

