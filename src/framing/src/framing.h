//
// Framing
//

#ifndef __LIQUID_FRAMING_H__
#define __LIQUID_FRAMING_H__

#include <stdbool.h>
#include <complex.h>

#include "../../sequence/src/sequence.h"   // p/n sequence
#include "../../fec/src/fec.h"             // crc, fec
#include "../../random/src/scramble.h"     // data randomizer

#define FRAMING_CONCAT(prefix,name) prefix ## name

//
// Frame descriptor
//
struct frame_s {
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

//
// Frame synchronizer
//
#define FRAMESYNC_MANGLE_FLOAT(name)    FRAMING_CONCAT(framesync,name)
#define FRAMESYNC_MANGLE_CFLOAT(name)   FRAMING_CONCAT(cframesync,name)

// Macro:
//  X   : name-mangling macro
//  T   : data type
#define LIQUID_FRAMESYNC_DEFINE_API(FRAMESYNC,T)            \
typedef struct FRAMESYNC(_s) * FRAMESYNC();                 \
                                                            \
FRAMESYNC() FRAMESYNC(_create)(unsigned int _n, T * _v);    \
FRAMESYNC() FRAMESYNC(_create_msequence)(msequence _ms);    \
void FRAMESYNC(_destroy)(FRAMESYNC() _fs);                  \
void FRAMESYNC(_print)(FRAMESYNC() _fs);                    \
T FRAMESYNC(_correlate)(FRAMESYNC() _fs, T _sym);

LIQUID_FRAMESYNC_DEFINE_API(FRAMESYNC_MANGLE_FLOAT, float)
LIQUID_FRAMESYNC_DEFINE_API(FRAMESYNC_MANGLE_CFLOAT, float complex)

#endif // __LIQUID_FRAMING_H__

