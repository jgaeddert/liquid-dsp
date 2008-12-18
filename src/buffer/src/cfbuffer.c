//
// Complex Float buffer
//

#include "buffer_internal.h"

#define BUFFER_TYPE_CFLOAT

#define BUFFER(name)    LIQUID_CONCAT(cfbuffer, name)
#define WINDOW(name)    LIQUID_CONCAT(cfwindow, name)
#define PORT(name)      LIQUID_CONCAT(cfport, name)

#define T float complex
#define BUFFER_PRINT_LINE(B,I) \
    printf("\t: %f + %f", crealf(B->v[I]), cimagf(B->v[I]));

#include "buffer.c"
#include "window.c"
#include "port.c"

