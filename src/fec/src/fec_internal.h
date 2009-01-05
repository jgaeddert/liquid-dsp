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
// FEC basic object
// 
struct fec_s {
    fec_scheme scheme;
    unsigned int dec_msg_len;
    unsigned int enc_msg_len;
    float rate;

    // encode/decode function pointers
    void (*encode_func)(fec _q, unsigned char * _msg_dec, unsigned char * _msg_enc);
    void (*decode_func)(fec _q, unsigned char * _msg_enc, unsigned char * _msg_dec);
};

extern const char * fec_scheme_str[4];

// Repeat (3)
fec fec_rep3_create(unsigned int _msg_len, void *_opts);
void fec_rep3_destroy(fec _q);
void fec_rep3_print(fec _q);
void fec_rep3_encode(fec _q, unsigned char * _msg_dec, unsigned char * _msg_enc);
void fec_rep3_decode(fec _q, unsigned char * _msg_enc, unsigned char * _msg_dec);

// Hamming(8,4)
unsigned char fec_hamming84_compute_syndrome(unsigned char _r);
unsigned char fec_hamming84_decode_symbol(unsigned char _r);

// Hamming(7,4)
unsigned char fec_hamming74_compute_syndrome(unsigned char _r);

#endif
