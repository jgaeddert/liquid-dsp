//
//
//

#include <stdio.h>

#include "../src/newbuffer.h"

int main() {
    float v[] = {1, 2, 3, 4, 5, 6, 7, 8};
    float r[10]; // reader
    unsigned int i, n;

    buffer b = buffer_create(10);

    // write elements to the array
    buffer_producer_lock_array(b, &n);
    printf("buffer: producer locked %u elements\n", n);
    buffer_producer_write(b, v, 4);
    // b : 1 2 3 4
    buffer_producer_write(b, v, 4);
    // b : 1 2 3 4 1 2 3 4
    buffer_producer_release_array(b);

    // read all available elements
    buffer_consumer_lock_array(b, &n);
    printf("buffer: consumer locked %u elements\n", n);
    for (i=0; i<n; i++)
        printf("  v[%2u] : %f\n", i, r[i]);
    buffer_consumer_read(b, r, 3);
    // b : 4 1 2 3 4
    buffer_consumer_release_array(b);

    // print buffer state
    buffer_debug_print(b);

    buffer_destroy(b);

    printf("done.\n");
    return 0;
}


