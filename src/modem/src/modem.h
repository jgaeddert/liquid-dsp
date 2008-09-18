//
//
//

#ifndef __LIQUID_MODEM_H__
#define __LIQUID_MODEM_H__

#include "modem_common.h"
//#include "liquid/types.h"

///\bug need better complex definition
#ifdef __cplusplus
#  include <complex>
#  define complex std::complex<float>
#else
#  include <complex.h>
#  ifdef I
#    undef I
#  endif
#endif

// define struct pointer
typedef struct modem_s * modem;

// create modulation scheme, allocating memory as necessary
modem modem_create(modulation_scheme, unsigned int _bits_per_symbol);

void free_modem(modem _mod);

// Initialize arbitrary modem constellation
void modem_arb_init(modem _mod, complex *_symbol_map, unsigned int _len);

// Initialize arbitrary modem constellation on data from external file
void modem_arb_init_file(modem _mod, char* filename);

// Generate random symbol
unsigned int modem_gen_rand_sym(modem _mod);

// Accessor functions
unsigned int modem_get_bps(modem _mod);

// generic modulate function; simply queries modem scheme and calls
// appropriate subroutine
void modulate(modem _mod, unsigned int symbol_in, complex *y);

void demodulate(modem _demod, complex x, unsigned int *symbol_out);

#endif // __LIQUID_MODEM_H__


