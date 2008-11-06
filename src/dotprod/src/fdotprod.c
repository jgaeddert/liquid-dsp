// 
// Floating-point dot product
//

#include "dotprod_internal.h"

#define X(name) DOTPROD_CONCAT(fdotprod,name)
#define T float

#include "dotprod.c"
