//
//
//

#include <stdio.h>
#include "decim2.h"

int main() {
    decim2 d = decim2_create(11, 0.0f, 0.1f);

    float x[256];
    float y[128];

    decim2_execute(d, x, 256, y, 128);

    decim2_destroy(d);

    printf("done.\n");
    return 0;
}

