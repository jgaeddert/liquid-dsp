//
// Modulator/demodulator common header (type independent)
//

#ifndef __LIQUID_MODEM_COMMON_H__
#define __LIQUID_MODEM_COMMON_H__

// Maximum number of allowed bits per symbol
#define MAX_MOD_BITS_PER_SYMBOL 8

#ifndef LIQUID_VALIDATE_INPUT
#  define LIQUID_VALIDATE_INPUT
#endif

// Modulation schemes available
typedef enum {
    MOD_UNKNOWN=0,      // Unknown modulation scheme
    MOD_PSK,            // Phase-shift keying (PSK)
    MOD_BPSK,           // Specific: binary PSK
    MOD_QPSK,           // specific: quaternary PSK
    MOD_DPSK,           // differential PSK
    MOD_ASK,            // amplitude-shift keying
    MOD_QAM,            // quadrature amplitude-shift keying (QAM)
    MOD_ARB,            // arbitrary QAM

    /* \brief Arbitrary QAM, only the constellation points in the first
     * quadrant are defined, the rest are mirrored about the abscissa and
     * ordinate
     *
     * \image html Constellation64ARBM.png "64-QAM (arb, mirrored) constellation"
     */
    MOD_ARB_MIRRORED,

    /* \brief Arbitrary QAM, only the constellation points in the first
     * quadrant are defined, the rest are rotated about the origin to the
     * other three quadrants
     *
     * \image html Constellation64ARBR.png "64-QAM (arb, rotated) constellation"
     */
    MOD_ARB_ROTATED

} modulation_scheme;

// Modulation scheme string for printing purposes
extern const char* modulation_scheme_str[10];

// useful functions

// counts the number of different bits between two symbols
unsigned int count_bit_errors(unsigned int _s1, unsigned int _s2);

// converts binary-coded decimal (BCD) to gray, ensuring successive values
// differ by exactly one bit
unsigned int gray_encode(unsigned int symbol_in);

// converts a gray-encoded symbol to binary-coded decimal (BCD)
unsigned int gray_decode(unsigned int symbol_in);

#endif // __LIQUID_MODEM_COMMON_H__


