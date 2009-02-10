//
// Filter API: complex floating-point
//

#include "liquid.composite.h"

// 
#define FIR_FILTER(name)    LIQUID_CONCAT(cfir_filter,name)
#define IIR_FILTER(name)    LIQUID_CONCAT(ciir_filter,name)
#define FIRPFB(name)        LIQUID_CONCAT(cfirpfb,name)
#define INTERP(name)        LIQUID_CONCAT(cinterp,name)
#define DECIM(name)         LIQUID_CONCAT(cdecim,name)
#define RESAMP(name)        LIQUID_CONCAT(cresamp,name)
#define RESAMP2(name)       LIQUID_CONCAT(cresamp2,name)
#define SYMSYNC(name)       LIQUID_CONCAT(csymsync,name)
#define SYMSYNC2(name)      LIQUID_CONCAT(csymsync2,name)

#define PRINTVAL(x)         printf("%12.4e + %12.4ej", crealf(x), cimagf(x))

#define T                   float complex
#define WINDOW(name)        LIQUID_CONCAT(cfwindow,name)
#define DOTPROD(name)       LIQUID_CONCAT(dotprod_cccf,name)

// source files
#include "fir_filter.c"
#include "iir_filter.c"
#include "firpfb.c"
#include "interp.c"
#include "decim.c"
#include "resamp.c"
#include "resamp2.c"
#include "symsync.c"
#include "symsync2.c"

