//
// Float buffer
//

#include "buffer_internal.h"

#define BUFFER_TYPE_FLOAT

#define BUFFER(name)    LIQUID_CONCAT(fbuffer, name)
#define WINDOW(name)    LIQUID_CONCAT(fwindow, name)
#define PORT(name)      LIQUID_CONCAT(fport, name)

#define T float
#define BUFFER_PRINT_LINE(B,I) \
    printf("\t: %f", B->v[I]);

#include "buffer.c"
#include "window.c"
#include "port.c"

