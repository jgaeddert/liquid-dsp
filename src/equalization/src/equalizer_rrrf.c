// 
// Floating-point equalizers
//

#include "liquid.internal.h"

#define EQRLS(name) LIQUID_CONCAT(eqrls_rrrf,name)

//#define TO float
//#define TC float
//#define TI float
#define T   float

#include "eqrls.c"
