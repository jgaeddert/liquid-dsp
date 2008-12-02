//
// Float window declaration
//

// Include header with API macros
#include "window.h"

// Define name-mangling macro (short version)
#define X(name) LIQUID_CONCAT(fwindow, name)

// Define data type
#define T float

// Define output line print
#define WINDOW_PRINT_LINE(B,I) \
    printf("\t: %f", B->v[I]);

#include "window.c"

