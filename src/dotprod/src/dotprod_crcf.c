// 
// Complex floating-point dot product
//

#include <complex.h>
#include "dotprod.h"

#define X(name) DOTPROD_CONCAT(dotprod_crcf,name)
#define TO float complex
#define TC float
#define TI float complex

#include "dotprod.c"
