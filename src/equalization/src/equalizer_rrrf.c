// 
// Floating-point equalizers
//

#include "liquid.internal.h"

#define EQRLS(name)     LIQUID_CONCAT(eqrls_rrrf,name)
#define DOTPROD(name)   LIQUID_CONCAT(dotprod_rrrf,name)
#define WINDOW(name)    LIQUID_CONCAT(fwindow,name)
//#define MATRIX(name)    LIQUID_CONCAT(fmatrix,name)
#define T               float
//#define TO float
//#define TC float
//#define TI float
#define IS_COMPLEX      0

#include "eqrls.c"
