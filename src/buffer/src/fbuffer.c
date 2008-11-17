//
// Float buffer
//

#include "buffer_internal.h"

#define BUFFER_TYPE_FLOAT

#define X(name) LIQUID_CONCAT(fbuffer, name)
#define T float
#define BUFFER_PRINT_LINE(B,I) \
    printf("\t: %f", B->v[I]);

#include "buffer.c"

