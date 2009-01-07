//
// Framing
//

#ifndef __LIQUID_FRAMING_H__
#define __LIQUID_FRAMING_H__

#include "sequence.h"
#include "modem_common.h"
#include "fec.h"

#include <stdbool.h>
#include <complex.h>

#define FRAMING_CONCAT(prefix,name) prefix ## name

//
// Frame descriptor
//
typedef enum {
    FRAME_UNKNOWN=0,

    FRAME_SRC0,
    FRAME_SRC1,
    FRAME_DST0,
    FRAME_DST1,
    FRAME_MOD_SCHEME,
    FRAME_MOD_BPS,
    FRAME_FEC_INNER_SCHEME,
    FRAME_FEC_OUTER_SCHEME,
    FRAME_INTLV_INNER_SCHEME,
    FRAME_INTLV_OUTER_SCHEME,
    FRAME_PROTOCOL,
    FRAME_MSG_LENGTH,
    FRAME_NUM_SYMBOLS
} frame_keyid;

typedef struct frame_s * frame;

frame frame_create();
void frame_destroy(frame _f);
void frame_print(frame _f);

void frame_setkey(frame _f, frame_keyid _id, unsigned int _value);

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


// 
// Packetizer
//

typedef struct packetizer_s * packetizer;

packetizer packetizer_create(unsigned int _dec_msg_len, fec_scheme _fec0, fec_scheme _fec1);
void packetizer_destroy(packetizer _p);
void packetizer_print(packetizer _p);

unsigned int packetizer_get_packet_length(packetizer _p);

void packetizer_encode(packetizer _p, unsigned char * _msg, unsigned char * _pkt);
bool packetizer_decode(packetizer _p, unsigned char * _pkt, unsigned char * _msg);

#endif // __LIQUID_FRAMING_H__

