//
// Filter API: complex floating-point
//

#include "filter.h"

// 
#define FIR_FILTER(name)    FILTER_CONCAT(cfir_filter,name)
#define IIR_FILTER(name)    FILTER_CONCAT(ciir_filter,name)
#define FIRPFB(name)        FILTER_CONCAT(cfirpfb,name)
#define INTERP(name)        FILTER_CONCAT(cinterp,name)
#define DECIM(name)         FILTER_CONCAT(cdecim,name)
#define RESAMP(name)        FILTER_CONCAT(cresamp,name)
#define SYMSYNC(name)       FILTER_CONCAT(csymsync,name)

#define PRINTVAL(x)         printf("%12.4e + %12.4ej", crealf(x), cimagf(x))

#define T                   float complex
#define WINDOW(name)        FILTER_CONCAT(cfwindow,name)
#define DOTPROD(name)       FILTER_CONCAT(cfdotprod,name)

// source files
#include "fir_filter.c"
#include "iir_filter.c"
#include "firpfb.c"
#include "interp.c"
#include "decim.c"
#include "resamp.c"
#include "symsync.c"

