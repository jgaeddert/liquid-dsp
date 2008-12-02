//
// Float complex window declaration
//

// Include header with API macros
#include "window.h"

// Define name-mangling macro (short version)
#define X(name) LIQUID_CONCAT(cfwindow, name)

// Define data type
#define T float complex

// Define output line print
#define WINDOW_PRINT_LINE(B,I) \
    printf("\t: %f +j %f", crealf(B->v[I]), cimagf(B->v[I]));

#include "window.c"

