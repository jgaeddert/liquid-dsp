//
// Framing API: floating-point
//

#include "framing_internal.h"

// 
#define FRAMESYNC(name)     LIQUID_CONCAT(framesync_rrrf,name)

#define PRINTVAL(x)         printf("%12.4e", x)

#define TO                  float
#define TC                  float
#define TI                  float
#define ABS(X)              fabsf(X)
#define WINDOW(name)        LIQUID_CONCAT(fwindow,name)
#define DOTPROD(name)       LIQUID_CONCAT(dotprod_rrrf,name)

// source files
#include "framesync.c"

