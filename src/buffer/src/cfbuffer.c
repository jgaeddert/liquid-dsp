//
// Complex Float buffer
//

#include "buffer_internal.h"

#define BUFFER_TYPE_CFLOAT

#define X(name) LIQUID_CONCAT(cfbuffer, name)
#define T float complex
#define BUFFER_PRINT_LINE(B,I) \
    printf("\t: %f + %f", crealf(B->v[I]), cimagf(B->v[I]));

#include "buffer.c"

