//
// FEC, etc.
//

#ifndef __LIQUID_FEC_H__
#define __LIQUID_FEC_H__

#include <stdbool.h>

#define FEC_CONCAT(prefix,name) prefix ## name

#define LIQUID_FEC_MANGLE_CRC32(name)       FEC_CONCAT(fec_crc32,name)
#define LIQUID_FEC_MANGLE_CHECKSUM32(name)  FEC_CONCAT(fec_checksum32,name)

// Macro: crc/checksum
//  CRC : name-mangling macro
#define LIQUID_CRC_DEFINE_API(CRC) \
typedef struct CRC(_s) * CRC(); \
CRC() CRC(_create)(); \
void CRC(_destroy)(CRC() _crc); \
void CRC(_print)(CRC() _crc); \
void CRC(_generate_key)(CRC() _crc, unsigned char * _msg, unsigned int _msg_len); \
bool CRC(_validate_key)(CRC() _crc, unsigned char * _msg, unsigned int _msg_len);

LIQUID_CRC_DEFINE_API(LIQUID_FEC_MANGLE_CRC32)
LIQUID_CRC_DEFINE_API(LIQUID_FEC_MANGLE_CHECKSUM32)

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
// FEC
//

typedef enum {
    FEC_UNKNOWN=0,
    FEC_NONE,
    FEC_REP3,
    FEC_HAMMING74,
    FEC_HAMMING84
} fec_scheme;

extern const char * fec_scheme_str[5];

typedef struct fec_s * fec;

// object-independent methods
unsigned int fec_get_enc_msg_length(fec_scheme _scheme, unsigned int _msg_len);
float fec_get_rate(fec_scheme _scheme);

fec fec_create(fec_scheme _scheme, unsigned int _msg_len, void *_opts);
void fec_destroy(fec _q);
void fec_print(fec _q);

void fec_encode(fec _q, unsigned char * _msg_dec, unsigned char * _msg_enc);
void fec_decode(fec _q, unsigned char * _msg_enc, unsigned char * _msg_dec);

#endif // __LIQUID_FEC_H__

