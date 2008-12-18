//
// Filter API: floating-point
//

#include "filter.h"

// 
#define FIR_FILTER(name)    FILTER_CONCAT(fir_filter,name)
#define FIRHILB(name)       FILTER_CONCAT(firhilb,name)
#define IIR_FILTER(name)    FILTER_CONCAT(iir_filter,name)
#define FIRPFB(name)        FILTER_CONCAT(firpfb,name)
#define INTERP(name)        FILTER_CONCAT(interp,name)
#define DECIM(name)         FILTER_CONCAT(decim,name)
#define RESAMP(name)        FILTER_CONCAT(resamp,name)
#define RESAMP2(name)       FILTER_CONCAT(resamp2,name)
#define SYMSYNC(name)       FILTER_CONCAT(symsync,name)

#define PRINTVAL(x)         printf("%12.4e", x)

#define T                   float
#define WINDOW(name)        FILTER_CONCAT(fwindow,name)
#define DOTPROD(name)       FILTER_CONCAT(fdotprod,name)

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

