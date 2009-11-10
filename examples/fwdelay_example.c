//
// fwdelay_example.c
//

#include <stdio.h>

#include "liquid.h"

int main() {
    // create wdelay: 10 elements, initialized to 0
    // w: 0 0 0 0 0 0 0 0 0 0
    fwdelay w = fwdelay_create(10);
    float r; // reader

    // push several elements
    unsigned int i;
    for (i=0; i<14; i++) {
        fwdelay_push(w, 1.0f);
        fwdelay_read(w, &r);
        printf("%4u : %12.8f\n", i, r);
    }

    fwdelay_print(w);

    // clean it up
    fwdelay_destroy(w);

    printf("done.\n");
    return 0;
}


