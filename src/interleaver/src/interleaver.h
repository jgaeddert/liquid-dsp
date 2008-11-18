//
// Block interleaver, operates on bytes
//

#ifndef __LIQUID_INTERLEAVER_H__
#define __LIQUID_INTERLEAVER_H__

typedef struct interleaver_s * interleaver;
typedef enum {
    INT_BLOCK=0,
    INT_SEQUENCE
} interleaver_type;

// create interleaver
//   _n     : number of bytes
//   _type  : type of re-ordering
interleaver interleaver_create(unsigned int _n, interleaver_type _type);

void interleaver_destroy(interleaver _i);

void interleaver_print(interleaver _i);

void interleaver_init_block(interleaver _i);

void interleaver_init_sequence(interleaver _i);

void interleaver_interleave(interleaver _i, unsigned char * _x, unsigned char * _y);

void interleaver_deinterleave(interleaver _i, unsigned char * _y, unsigned char * _x);

void interleaver_debug_print(interleaver _i);

#endif // __LIQUID_INTERLEAVER_H__
