// 
// Complex floating-point dot product
//

#include <complex.h>
#include "liquid.internal.h"

#define X(name) LIQUID_CONCAT(dotprod_crcf,name)
#define TO float complex
#define TC float
#define TI float complex

#include "dotprod.c"
