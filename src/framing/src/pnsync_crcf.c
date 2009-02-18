//
// Framing API: floating-point
//

#include "framing_internal.h"

// 
#define PNSYNC(name)        LIQUID_CONCAT(pnsync_crcf,name)

#define PRINTVAL(x)         printf("%12.4e + j%12.4e", crealf(x), cimagf(x))

#define TO                  float complex
#define TC                  float
#define TI                  float complex
#define ABS(X)              cabsf(X)
#define WINDOW(name)        LIQUID_CONCAT(cfwindow,name)
#define DOTPROD(name)       LIQUID_CONCAT(dotprod_crcf,name)

// source files
#include "pnsync.c"

