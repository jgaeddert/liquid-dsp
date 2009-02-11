//
// Framing API: floating-point
//

#include "framing_internal.h"

// 
#define FRAMESYNC(name)     LIQUID_CONCAT(cframesync,name)

#define PRINTVAL(x)         printf("%12.4e + j%12.4e", crealf(x), cimagf(x))

#define T                   float complex
#define ABS(X)              cabsf(X)
#define WINDOW(name)        LIQUID_CONCAT(cfwindow,name)
#define DOTPROD(name)       LIQUID_CONCAT(dotprod_cccf,name)

// source files
#include "framesync.c"

