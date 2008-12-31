// 
// Complex floating-point dot product
//

#include <complex.h>
#include "dotprod.h"

#define X(name) DOTPROD_CONCAT(dotprod_cccf,name)
#define TO float complex
#define TC float complex
#define TI float complex

#include "dotprod.c"
