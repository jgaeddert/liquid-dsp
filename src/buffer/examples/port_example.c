//
//
//

#include <stdio.h>

#include "../src/port.h"

int main() {
    float v[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    float *r;
    unsigned int i;

    port p = port_create(10);

    port_print(p);

    port_produce(p, v, 5);
    port_print(p);

    port_consume(p, &r, 3);
    printf("first consumer:\n");
    for (i=0; i<3; i++)
        printf("  r[%u] : %4.2f\n", i, r[i]);
    port_release(p, 2);

    port_consume(p, &r, 2);
    printf("second consumer:\n");
    for (i=0; i<3; i++)
        printf("  r[%u] : %4.2f\n", i, r[i]);
    port_release(p, 2);

    port_destroy(p);

    printf("done.\n");
    return 0;
}

