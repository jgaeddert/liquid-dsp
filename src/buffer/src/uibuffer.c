//
// Buffer: unsigned int
//

#include "buffer_internal.h"

#define BUFFER_TYPE_UINT

#define BUFFER(name)    LIQUID_CONCAT(uibuffer, name)
#define WINDOW(name)    LIQUID_CONCAT(uiwindow, name)
#define PORT(name)      LIQUID_CONCAT(uiport, name)

#define T unsigned int
#define BUFFER_PRINT_LINE(B,I) \
    printf("\t: %u", B->v[I]);


#include "buffer.c"
#include "window.c"
#include "port.c"

