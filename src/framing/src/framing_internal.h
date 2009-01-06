//
// Framing
//

#ifndef __LIQUID_FRAMING_INTERNAL_H__
#define __LIQUID_FRAMING_INTERNAL_H__

#include <stdbool.h>

#include "../../random/src/scramble.h"      // data randomizer
#include "../../fec/src/fec.h"              // fec_scheme
#include "../../interleaver/src/interleaver.h"

#include "framing.h"

struct packetizer_s {
    unsigned int dec_msg_len;
    unsigned int enc_msg_len;

    unsigned int crc32_key;

    // fec (outer)
    fec_scheme fec0_scheme;
    fec fec0;
    unsigned int fec0_dec_msg_len;
    unsigned int fec0_enc_msg_len;
    unsigned char * fec0_src;
    unsigned char * fec0_dst;

    // interleaver (outer)
    interleaver intlv0;
    unsigned int intlv0_scheme;
    unsigned int intlv0_len;
    unsigned char * intlv0_src;
    unsigned char * intlv0_dst;

    // fec (inner)
    fec_scheme fec1_scheme;
    fec fec1;
    unsigned int fec1_dec_msg_len;
    unsigned int fec1_enc_msg_len;
    unsigned char * fec1_src;
    unsigned char * fec1_dst;

    // interleaver (inner)
    interleaver intlv1;
    unsigned int intlv1_scheme;
    unsigned int intlv1_len;
    unsigned char * intlv1_src;
    unsigned char * intlv1_dst;

    // buffers
    unsigned int buffer_len;
    unsigned char * buffer_0;
    unsigned char * buffer_1;
};

#endif // __LIQUID_FRAMING_INTERNAL_H__

