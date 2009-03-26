//
// Framing API: floating-point
//

#include "framing_internal.h"

// 
#define PNSYNC(name)        LIQUID_CONCAT(pnsync_rrrf,name)
#define BSYNC(name)         LIQUID_CONCAT(bsync_rrrf,name)

#define PRINTVAL(x)         printf("%12.4e", x)

#define TO                  float
#define TC                  float
#define TI                  float
#define ABS(X)              fabsf(X)
#define WINDOW(name)        LIQUID_CONCAT(fwindow,name)
#define DOTPROD(name)       LIQUID_CONCAT(dotprod_rrrf,name)

#undef  TO_COMPLEX
#undef  TC_COMPLEX
#undef  TI_COMPLEX

// source files
#include "pnsync.c"
#include "bsync.c"

