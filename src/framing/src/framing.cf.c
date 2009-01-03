//
// Framing API: floating-point
//

#include "framing.h"

// 
#define FRAMESYNC(name)     FRAMING_CONCAT(cframesync,name)

#define PRINTVAL(x)         printf("%12.4e + j%12.4e", crealf(x), cimagf(x))

#define T                   float complex
#define ABS(X)              cabsf(X)
#define WINDOW(name)        FRAMING_CONCAT(cfwindow,name)
#define DOTPROD(name)       FRAMING_CONCAT(dotprod_cccf,name)

// source files
#include "framesync.c"

