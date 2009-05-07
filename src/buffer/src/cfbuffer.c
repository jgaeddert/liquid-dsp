//
// Complex Float buffer
//

#include "liquid.internal.h"

#define BUFFER_TYPE_CFLOAT

#define BUFFER(name)    LIQUID_CONCAT(cfbuffer, name)
#define WINDOW(name)    LIQUID_CONCAT(cfwindow, name)
#define PORT(name)      LIQUID_CONCAT(cfport, name)

#define T float complex
#define BUFFER_PRINT_LINE(B,I) \
    printf("\t: %f + %f", crealf(B->v[I]), cimagf(B->v[I]));
#define BUFFER_PRINT_VALUE(V) \
    printf("\t: %12.4e + %12.4e", crealf(V), cimagf(V));

#include "buffer.c"
#include "window.c"
#include "port.c"

