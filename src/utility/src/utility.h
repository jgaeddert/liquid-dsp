//
// Miscellaneous utilities
//

#ifndef __LIQUID_UTILITY_H__
#define __LIQUID_UTILITY_H__

// TODO: calculate this using config.h?
#define SIZEOF_INT              4
#define SIZEOF_UNSIGNED_INT     SIZEOF_INT

// number of 1's in byte
extern unsigned int c_ones[256];

// Count the number of ones in an integer
unsigned int count_ones_static(unsigned int _x);

// Count the number of ones in an integer, inline insertion
#define count_ones_inline_uint2(x) (    \
    c_ones[  (x)      & 0xFF ] +        \
    c_ones[ ((x)>>8)  & 0xFF ])

#define count_ones_inline_uint4(x) (    \
    c_ones[  (x)      & 0xFF ] +        \
    c_ones[ ((x)>> 8) & 0xFF ] +        \
    c_ones[ ((x)>>16) & 0xFF ] +        \
    c_ones[ ((x)>>24) & 0xFF ])

#if SIZEOF_UNSIGNED_INT == 2
#  define count_ones(x) count_ones_inline_uint2(x)
#elif SIZEOF_UNSIGNED_INT == 4
#  define count_ones(x) count_ones_inline_uint4(x)
#endif

//#define count_ones(x) count_ones_static(x)


// number of leading zeros in byte
extern unsigned int leading_zeros[256];

// Count leading zeros in an integer
unsigned int count_leading_zeros(unsigned int _x);

// Most-significant bit index
unsigned int msb_index(unsigned int _x);


// packs bytes with one bit of information into a byte
void pack_bytes(
    unsigned char * input,
    unsigned int input_length,
    unsigned char * output,
    unsigned int output_length,
    unsigned int *num_written);

// unpacks bytes with 8 bits of information
void unpack_bytes(
    unsigned char * input,
    unsigned int input_length,
    unsigned char * output,
    unsigned int output_length,
    unsigned int *num_written);

// repacks bytes with arbitrary symbol sizes
void repack_bytes(
    unsigned char * input,
    unsigned int input_sym_size,
    unsigned int input_length,
    unsigned char * output,
    unsigned int output_sym_size,
    unsigned int output_length,
    unsigned int *num_written);


#endif // __LIQUID_UTILITY_H__
