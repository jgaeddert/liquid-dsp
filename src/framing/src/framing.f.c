//
// Framing API: floating-point
//

#include "framing.h"

// 
#define FRAMESYNC(name)     FRAMING_CONCAT(framesync,name)

#define PRINTVAL(x)         printf("%12.4e", x)

#define T                   float
#define WINDOW(name)        FRAMING_CONCAT(fwindow,name)
#define DOTPROD(name)       FRAMING_CONCAT(dotprod_rrrf,name)

// source files
#include "framesync.c"

