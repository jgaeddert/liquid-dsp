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

const char * crc_scheme_str[LIQUID_NUM_CRC_SCHEMES][2] = {
    // short name,  long name
    {"unknown",     "unknown"},
    {"none",        "none"},
    {"checksum",    "checksum (8-bit)"},
    {"crc8",        "CRC (8-bit)"},
    {"crc16",       "CRC (16-bit)"},
    {"crc24",       "CRC (24-bit)"},
    {"crc32",       "CRC (32-bit)"}
};

crc_scheme liquid_getopt_str2crc(const char * _str)
{
    // compare each string to short name
    unsigned int i;
    for (i=0; i<LIQUID_NUM_CRC_SCHEMES; i++) {
        if (strcmp(_str,crc_scheme_str[i][0])==0) {
            return i;
        }
    }

    fprintf(stderr,"warning: liquid_getopt_str2crc(), unknown/unsupported crc scheme : %s\n", _str);
    return CRC_UNKNOWN;
}

// get length of CRC (bytes)
unsigned int crc_get_length(crc_scheme _scheme)
{
    switch (_scheme) {
    case CRC_UNKNOWN:   return 0;
    case CRC_NONE:      return 0;
    case CRC_CHECKSUM:  return 1;
    case CRC_8:         return 1;
    case CRC_16:        return 2;
    case CRC_24:        return 3;
    case CRC_32:        return 4;
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
    case CRC_UNKNOWN:
        fprintf(stderr,"error: crc_generate_key(), cannot generate key with CRC type \"UNKNOWN\"\n");
        exit(-1);
    case CRC_NONE:      return 0;
    case CRC_CHECKSUM:  return checksum_generate_key(_msg, _n);
    case CRC_8:         return crc8_generate_key(_msg, _n);
    case CRC_16:        return crc16_generate_key(_msg, _n);
    case CRC_24:        return crc24_generate_key(_msg, _n);
    case CRC_32:        return crc32_generate_key(_msg, _n);
    default:
        fprintf(stderr,"error: crc_generate_key(), unknown/unsupported scheme: %d\n", _scheme);
        exit(1);
    }

    return 0;
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
    switch (_scheme) {
    case CRC_UNKNOWN:
        fprintf(stderr,"error: crc_validate_message(), cannot validate with CRC type \"UNKNOWN\"\n");
        exit(-1);
    case CRC_NONE:      return 1;
    case CRC_CHECKSUM:  return checksum_validate_message(_msg, _n, _key);
    case CRC_8:         return crc8_validate_message(_msg, _n, _key);
    case CRC_16:        return crc16_validate_message(_msg, _n, _key);
    case CRC_24:        return crc24_validate_message(_msg, _n, _key);
    case CRC_32:        return crc32_validate_message(_msg, _n, _key);
    default:
        fprintf(stderr,"error: crc_validate_message(), unknown/unsupported scheme: %d\n", _scheme);
        exit(1);
    }

    return 0;

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

// validate message using 8-bit checksum key
//
//  _scheme     :   error-detection scheme
//  _msg        :   input data message, [size: _n x 1]
//  _n          :   input data message size
//  _key        :   error-detection key
int checksum_validate_message(unsigned char *_data,
                              unsigned int _n,
                              unsigned int _key)
{
    unsigned int i, sum=0;
    for (i=0; i<_n; i++)
        sum += (unsigned int) (_data[i]);
    //sum &= 0x00ff;

    unsigned char check = ((sum&0x00ff)+_key)&0x0ff;
    //printf("check: 0x%0x\n", (unsigned int) check);
    return (check==0);
}


// 
// CRC-8
//

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
    unsigned int poly = reverse_byte(CRC8_POLY);
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


// validate message with 8-bit CRC
//  _msg    :   input data message [size: _n x 1]
//  _n      :   input data message size
//  _key    :   8-bit CRC key
int crc8_validate_message(unsigned char *_msg,
                          unsigned int _n,
                          unsigned int _key)
{
    return crc8_generate_key(_msg,_n)==_key;
}



// 
// CRC-16
//

// reverse integer with 16 bits of data
unsigned int reverse_uint16(unsigned int _x)
{
    unsigned int i, y=0;
    for (i=0; i<16; i++) {
        y <<= 1;
        y |= _x & 1;
        _x >>= 1;
    }
    return y;
}

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

// validate message with 16-bit CRC
//  _msg    :   input data message [size: _n x 1]
//  _n      :   input data message size
//  _key    :   16-bit CRC key
int crc16_validate_message(unsigned char *_msg,
                           unsigned int _n,
                           unsigned int _key)
{
    return crc16_generate_key(_msg,_n)==_key;
}



// 
// CRC-24
//

// reverse integer with 24 bits of data
unsigned int reverse_uint24(unsigned int _x)
{
    unsigned int i, y=0;
    for (i=0; i<24; i++) {
        y <<= 1;
        y |= _x & 1;
        _x >>= 1;
    }
    return y;
}

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

// validate message with 24-bit CRC
//  _msg    :   input data message [size: _n x 1]
//  _n      :   input data message size
//  _key    :   24-bit CRC key
int crc24_validate_message(unsigned char *_msg,
                           unsigned int _n,
                           unsigned int _key)
{
    return crc24_generate_key(_msg,_n)==_key;
}


// 
// CRC-32
//

// reverse integer with 32 bits of data
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

// validate message with 32-bit CRC
//  _msg    :   input data message [size: _n x 1]
//  _n      :   input data message size
//  _key    :   32-bit CRC key
int crc32_validate_message(unsigned char *_msg,
                           unsigned int _n,
                           unsigned int _key)
{
    return crc32_generate_key(_msg,_n)==_key;
}


