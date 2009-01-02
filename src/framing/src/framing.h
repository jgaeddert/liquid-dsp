//
// Framing
//

#ifndef __LIQUID_FRAMING_H__
#define __LIQUID_FRAMING_H__

#include <stdbool.h>

#include "../sequence/src/sequence.h"   // p/n sequence
#include "../fec/src/fec.h"             // crc, fec
#include "../random/src/scramble.h"     // data randomizer

//
// Frame descriptor
//
typedef struct frame_s {
    // options
    unsigned int phasing_pattern_length;
    unsigned int pn_sequence_length;
    unsigned int header_length;

    // header length, with crc and encoded
    unsigned int header_length_enc;

    // entire frame header length
    unsigned int frame_header_length;

    // private member objects
    bsequence pn; // p/n synchronization sequence
    bsequence rx; // received sequence
    unsigned char crc32_key[4];
};
typedef struct frame_s * frame;

frame frame_create();
void frame_destroy(frame _f);

void frame_encode(frame _f, unsigned char * _header, unsigned char *_out);
bool frame_decode(frame _f, unsigned char * _in, unsigned char * _header);

#endif // __LIQUID_FRAMING_H__

