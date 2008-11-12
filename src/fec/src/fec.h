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
bool checksum_validate_message(unsigned char *_data, unsigned int _n, unsigned char _key);

//
// CRC
//
//void crc_generate_key(unsigned char *_msg, unsigned int _msg_len, unsigned char *_key, unsigned int _key_len);
unsigned int crc32_generate_key(unsigned char *_data, unsigned int _n);
bool crc32_validate_message(unsigned char *_data, unsigned int _n, unsigned int _key);

//
// Repeat code
//   Inefficient, yet simple redundancy
//
//typedef struct fec_rep3_s * fec_rep3;
void fec_rep3_encode(unsigned char *_msg_dec, unsigned int _msg_len, unsigned char * _msg_enc);
unsigned int fec_rep3_decode(unsigned char *_msg_enc, unsigned int _msg_len, unsigned char * _msg_dec);

//
// 1/2-rate (8,4) Hamming code
//
void fec_hamming84_encode(unsigned char *_msg_dec, unsigned int _msg_len, unsigned char *_msg_enc);
unsigned int
     fec_hamming84_decode(unsigned char *_msg_enc, unsigned int _msg_len, unsigned char *_msg_dec);

void fec_hamming74_encode(unsigned char *_msg_dec, unsigned int _msg_len, unsigned char *_msg_enc);
unsigned int
     fec_hamming74_decode(unsigned char *_msg_enc, unsigned int _msg_len, unsigned char *_msg_dec);

#endif
