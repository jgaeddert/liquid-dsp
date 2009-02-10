// 
// Floating-point dot product
//

#include "dotprod_internal.h"

#define X(name) DOTPROD_CONCAT(dotprod_rrrf,name)
#define TO float
#define TC float
#define TI float

#include "dotprod.c"
