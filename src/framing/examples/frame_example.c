//
//
//

#include <stdio.h>

#include "../src/framing.h"

int main() {
    frame f = frame_create();
    frame_setkey(f, FRAME_SRC0,    144);

    frame_print(f);

    // clean it up
    frame_destroy(f);

    printf("done.\n");
    return 0;
}

