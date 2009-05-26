// 
// Floating-point equalizers
//

#include "liquid.internal.h"

#define EQLMS(name)     LIQUID_CONCAT(eqlms_cccf,name)
#define EQRLS(name)     LIQUID_CONCAT(eqrls_cccf,name)

#define DOTPROD(name)   LIQUID_CONCAT(dotprod_cccf,name)
#define WINDOW(name)    LIQUID_CONCAT(cfwindow,name)
#define MATRIX(name)    LIQUID_CONCAT(cfmatrix,name)

#define T               float complex

#define PRINTVAL(V)     printf("%5.2f+j%5.2f ", crealf(V), cimagf(V));

#include "eqlms.c"
#include "eqrls.c"
