//
// Buffer: unsigned int
//

#include "buffer_internal.h"

#define BUFFER_TYPE_UINT

#define X(name) LIQUID_CONCAT(uibuffer, name)
#define T unsigned int
#define BUFFER_PRINT_LINE(B,I) \
    printf("\t: %u", B->v[I]);


#include "buffer.c"

