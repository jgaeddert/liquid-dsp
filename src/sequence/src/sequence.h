//
// binary sequence
//

#ifndef __LIQUID_SEQUENCE_H__
#define __LIQUID_SEQUENCE_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <inttypes.h>

#include "../../utility/src/utility.h" // for count_ones

//-----------------------------------------------------------------------------
//
// Binary Sequence
//
//-----------------------------------------------------------------------------

/*! \brief Sequence of bits packed into bytes
 *
 * \todo convert from 8-bit blocks to unsigned int blocks
 */
typedef struct {
    uint8_t * s;                ///< sequence array, memory pointer
    unsigned int num_bits;      ///< number of bits in sequence
    unsigned int num_bits_msb;  ///< number of bits in most-significant block
    uint8_t bit_mask_msb;       ///< bit mask for most-significant block
    unsigned int s_len;         ///< length of array, number of allocated blocks
} binary_sequence;

/// Create a binary sequence of a specific length
binary_sequence * binary_sequence_create(unsigned int num_bits);

/// Free memory in a binary sequence
void free_binary_sequence(binary_sequence *_bs);

/// Clear binary sequence (set to 0's)
static inline void binary_sequence_clear(binary_sequence * bs) {
    memset( bs->s, 0x00, (bs->s_len)*sizeof(uint8_t) );
}

/// Print sequence to the screen
void binary_sequence_print(binary_sequence * bs);

/// Push bit into to back of a binary sequence
void binary_sequence_push(binary_sequence * bs, unsigned int b);

/// Correlates two binary sequences together
int binary_sequence_correlate(binary_sequence * bs1, binary_sequence * bs2);

/// Binary addition of two bit sequences
void binary_sequence_add(binary_sequence * bs1, binary_sequence * bs2, binary_sequence * bs3);

/// Accumulates the 1's in a binary sequence
unsigned int binary_sequence_accumulate(binary_sequence * bs);


//-----------------------------------------------------------------------------
//
// M-Sequence
//
//-----------------------------------------------------------------------------

#define SIGPROCC_MAX_MSEQUENCE_LENGTH   4095

// default m-sequence generators:   g (hex)    m    n       g (octal)
#define SIGPROCC_MSEQUENCE_N3       0x0007  // 2    3       7
#define SIGPROCC_MSEQUENCE_N7       0x000B  // 3    7       13
#define SIGPROCC_MSEQUENCE_N15      0x0013  // 4    15      23
#define SIGPROCC_MSEQUENCE_N31      0x0025  // 5    31      45
#define SIGPROCC_MSEQUENCE_N63      0x0043  // 6    63      103
#define SIGPROCC_MSEQUENCE_N127     0x0089  // 7    127     211
#define SIGPROCC_MSEQUENCE_N255     0x011D  // 8    255     435
#define SIGPROCC_MSEQUENCE_N511     0x0211  // 9    511     1021
#define SIGPROCC_MSEQUENCE_N1023    0x0409  // 10   1023    2011
#define SIGPROCC_MSEQUENCE_N2047    0x0805  // 11   2047    4005
#define SIGPROCC_MSEQUENCE_N4095    0x1053  // 12   4095    10123

/** \brief Maximum length P/N sequence
 *
 * \cite Roger. L. Peterson, Rodger E. Ziemer, David E. Borth, "Introduction
 *       to Spread-Spectrum Communications." New Jersey: Prentice Hall, 1995
 */
typedef struct {
    unsigned int m;     ///< length generator polynomial, shift register
    unsigned int g;     ///< generator polynomial
    unsigned int a;     ///< initial shift register state, default: 1

    unsigned int n;     ///< length of sequence, \f$ n=2^m-1 \f$
    unsigned int v;     ///< shift register
    unsigned int b;     ///< return bit
} msequence;

/// Default msequence generator objects
extern msequence msequence_default[13];

/// Initialize msequence generator
void msequence_init(msequence *_ms, unsigned int _m, unsigned int _g, unsigned int _a);

/// Advance msequence on shift register
static inline unsigned int msequence_advance(msequence *_ms) {
    _ms->b = count_ones( _ms->v & _ms->g ) % 2;
    _ms->v <<= 1;       // shift register
    _ms->v |= _ms->b;   // push bit onto register
    _ms->v &= _ms->n;   // apply mask to register
    return _ms->b;      // return result
};

/// Generate symbol
unsigned int msequence_generate_symbol(msequence *_ms, unsigned int _bps);

/// Reset msequence shift register to original state
static inline void msequence_reset(msequence *_ms) { _ms->v = _ms->a; }

/// Initializes a binary_sequence object on a maximum length P/N sequence
void binary_sequence_init_msequence(binary_sequence* _bs, msequence* _ms);

#endif  // __LIQUID_SEQUENCE_H__

