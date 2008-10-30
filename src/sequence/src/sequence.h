//
// Binary sequencing
//

#ifndef __LIQUID_SEQUENCE_H__
#define __LIQUID_SEQUENCE_H__

// Binary sequence (generic)

typedef struct binary_sequence_s * binary_sequence;

// Create a binary sequence of a specific length
binary_sequence binary_sequence_create(unsigned int num_bits);

// Free memory in a binary sequence
void binary_sequence_destroy(binary_sequence _bs);

// Clear binary sequence (set to 0's)
void binary_sequence_clear(binary_sequence _bs);

// Print sequence to the screen
void binary_sequence_print(binary_sequence _bs);

// Push bit into to back of a binary sequence
void binary_sequence_push(binary_sequence _bs, unsigned int b);

// Correlates two binary sequences together
int binary_sequence_correlate(binary_sequence _bs1, binary_sequence _bs2);

// Binary addition of two bit sequences
void binary_sequence_add(binary_sequence _bs1, binary_sequence _bs2, binary_sequence _bs3);

// Accumulates the 1's in a binary sequence
unsigned int binary_sequence_accumulate(binary_sequence _bs);


// M-Sequence

#define LIQUID_MAX_MSEQUENCE_LENGTH   4095

// default m-sequence generators:   g (hex)    m    n       g (octal)
#define LIQUID_MSEQUENCE_N3         0x0007  // 2    3       7
#define LIQUID_MSEQUENCE_N7         0x000B  // 3    7       13
#define LIQUID_MSEQUENCE_N15        0x0013  // 4    15      23
#define LIQUID_MSEQUENCE_N31        0x0025  // 5    31      45
#define LIQUID_MSEQUENCE_N63        0x0043  // 6    63      103
#define LIQUID_MSEQUENCE_N127       0x0089  // 7    127     211
#define LIQUID_MSEQUENCE_N255       0x011D  // 8    255     435
#define LIQUID_MSEQUENCE_N511       0x0211  // 9    511     1021
#define LIQUID_MSEQUENCE_N1023      0x0409  // 10   1023    2011
#define LIQUID_MSEQUENCE_N2047      0x0805  // 11   2047    4005
#define LIQUID_MSEQUENCE_N4095      0x1053  // 12   4095    10123

typedef struct msequence_s * msequence;

// Initialize msequence generator
void msequence_init(msequence _ms, unsigned int _m, unsigned int _g, unsigned int _a);

// Advance msequence on shift register
unsigned int msequence_advance(msequence _ms);

// Generate symbol
unsigned int msequence_generate_symbol(msequence _ms, unsigned int _bps);

// Reset msequence shift register to original state
void msequence_reset(msequence _ms);

// Initializes a binary_sequence object on a maximum length P/N sequence
void binary_sequence_init_msequence(binary_sequence _bs, msequence _ms);

#endif  // __LIQUID_SEQUENCE_H__

