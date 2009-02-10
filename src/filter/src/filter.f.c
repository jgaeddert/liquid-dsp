//
// Filter API: floating-point
//

#include "liquid.composite.h"

// 
#define FIR_FILTER(name)    LIQUID_CONCAT(fir_filter,name)
#define FIRHILB(name)       LIQUID_CONCAT(firhilb,name)
#define IIR_FILTER(name)    LIQUID_CONCAT(iir_filter,name)
#define FIRPFB(name)        LIQUID_CONCAT(firpfb,name)
#define INTERP(name)        LIQUID_CONCAT(interp,name)
#define DECIM(name)         LIQUID_CONCAT(decim,name)
#define RESAMP(name)        LIQUID_CONCAT(resamp,name)
#define RESAMP2(name)       LIQUID_CONCAT(resamp2,name)
#define SYMSYNC(name)       LIQUID_CONCAT(symsync,name)
#define SYMSYNC2(name)      LIQUID_CONCAT(symsync2,name)

#define PRINTVAL(x)         printf("%12.4e", x)

#define T                   float
#define WINDOW(name)        LIQUID_CONCAT(fwindow,name)
#define DOTPROD(name)       LIQUID_CONCAT(dotprod_rrrf,name)

// source files
#include "fir_filter.c"
#include "firhilb.c"
#include "iir_filter.c"
#include "firpfb.c"
#include "interp.c"
#include "decim.c"
#include "resamp.c"
#include "resamp2.c"
#include "symsync.c"
#include "symsync2.c"

