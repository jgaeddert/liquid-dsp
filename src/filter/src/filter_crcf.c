//
// Filter API: complex floating-point
//

#include "liquid.internal.h"

// 
#define FIR_FILTER(name)    LIQUID_CONCAT(fir_filter_crcf,name)
#define IIR_FILTER(name)    LIQUID_CONCAT(iir_filter_crcf,name)
#define FIRPFB(name)        LIQUID_CONCAT(firpfb_crcf,name)
#define INTERP(name)        LIQUID_CONCAT(interp_crcf,name)
#define DECIM(name)         LIQUID_CONCAT(decim_crcf,name)
#define QMFB(name)          LIQUID_CONCAT(qmfb_crcf,name)
#define RESAMP(name)        LIQUID_CONCAT(resamp_crcf,name)
#define RESAMP2(name)       LIQUID_CONCAT(resamp2_crcf,name)
#define SYMSYNC(name)       LIQUID_CONCAT(symsync_crcf,name)
#define SYMSYNC2(name)      LIQUID_CONCAT(symsync2_crcf,name)

#define PRINTVAL(x)         printf("%12.4e + %12.4ej", crealf(x), cimagf(x))

#define T                   float complex   // general
#define TO                  float complex   // output
#define TC                  float           // coefficients
#define TI                  float complex   // input
#define WINDOW(name)        LIQUID_CONCAT(cfwindow,name)
#define DOTPROD(name)       LIQUID_CONCAT(dotprod_crcf,name)

// source files
#include "fir_filter.c"
#include "iir_filter.c"
#include "firpfb.c"
#include "interp.c"
#include "decim.c"
#include "qmfb.c"
#include "resamp.c"
#include "resamp2.c"
#include "symsync.c"
#include "symsync2.c"
