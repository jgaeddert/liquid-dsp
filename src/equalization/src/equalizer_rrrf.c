// 
// Floating-point equalizers
//

#include "liquid.internal.h"

#define EQLMS(name)     LIQUID_CONCAT(eqlms_rrrf,name)
#define EQRLS(name)     LIQUID_CONCAT(eqrls_rrrf,name)

#define DOTPROD(name)   LIQUID_CONCAT(dotprod_rrrf,name)
#define WINDOW(name)    LIQUID_CONCAT(fwindow,name)

#define T               float

#define PRINTVAL(V)     printf("%5.2f ", V);

#include "eqlms.c"
#include "eqrls.c"
