//
// Filter API: floating-point
//

#include "filter_internal.h"

// 
#define FIR_FILTER(name)    LIQUID_CONCAT(fir_filter_rrrf,name)
#define FIRHILB(name)       LIQUID_CONCAT(firhilb,name)
#define IIR_FILTER(name)    LIQUID_CONCAT(iir_filter_rrrf,name)
#define FIRPFB(name)        LIQUID_CONCAT(firpfb_rrrf,name)
#define INTERP(name)        LIQUID_CONCAT(interp_rrrf,name)
#define DECIM(name)         LIQUID_CONCAT(decim,name)
#define RESAMP(name)        LIQUID_CONCAT(resamp,name)
#define RESAMP2(name)       LIQUID_CONCAT(resamp2,name)
#define SYMSYNC(name)       LIQUID_CONCAT(symsync,name)
#define SYMSYNC2(name)      LIQUID_CONCAT(symsync2,name)

#define PRINTVAL(x)         printf("%12.4e", x)

#define T                   float   // general
#define TO                  float   // output
#define TC                  float   // coefficients
#define TI                  float   // input
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
