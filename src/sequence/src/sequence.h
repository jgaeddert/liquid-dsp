//
// Binary sequencing
//

#ifndef __LIQUID_SEQUENCE_H__
#define __LIQUID_SEQUENCE_H__

// Binary sequence (generic)

typedef struct bsequence_s * bsequence;

// Create a binary sequence of a specific length
bsequence bsequence_create(unsigned int num_bits);

// Free memory in a binary sequence
void bsequence_destroy(bsequence _bs);

// Clear binary sequence (set to 0's)
void bsequence_clear(bsequence _bs);

// Print sequence to the screen
void bsequence_print(bsequence _bs);

// Push bit into to back of a binary sequence
void bsequence_push(bsequence _bs, unsigned int b);

// Correlates two binary sequences together
int bsequence_correlate(bsequence _bs1, bsequence _bs2);

// Binary addition of two bit sequences
void bsequence_add(bsequence _bs1, bsequence _bs2, bsequence _bs3);

// Accumulates the 1's in a binary sequence
unsigned int bsequence_accumulate(bsequence _bs);

// accessor functions
unsigned int bsequence_get_length(bsequence _bs);


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

msequence msequence_create(unsigned int _m);
void msequence_destroy(msequence _m);

// Initialize msequence generator
void msequence_init(msequence _ms, unsigned int _m, unsigned int _g, unsigned int _a);

// Advance msequence on shift register
unsigned int msequence_advance(msequence _ms);

// Generate symbol
unsigned int msequence_generate_symbol(msequence _ms, unsigned int _bps);

// Reset msequence shift register to original state
void msequence_reset(msequence _ms);

// Initializes a bsequence object on a maximum length P/N sequence
void bsequence_init_msequence(bsequence _bs, msequence _ms);

// accessor methods
unsigned int msequence_get_length(msequence _ms);

#endif  // __LIQUID_SEQUENCE_H__

