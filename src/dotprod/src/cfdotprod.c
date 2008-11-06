// 
// Floating-point dot product
//

#include "dotprod_internal.h"

#define X(name) DOTPROD_CONCAT(cfdotprod,name)
#define T float complex

#include "dotprod.c"
