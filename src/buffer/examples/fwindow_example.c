//
//
//

#include <stdio.h>

#include "../src/buffer.h"

int main() {
    float v[] = {9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
    float *r; // reader
    unsigned int i;

    // create window
    // 0 0 0 0 0 0 0 0 0 0
    fwindow w = fwindow_create(10);

    // push 4 elements
    // 0 0 0 0 0 0 1 1 1 1
    fwindow_push(w, 1);
    fwindow_push(w, 1);
    fwindow_push(w, 1);
    fwindow_push(w, 1);

    // push 4 more elements
    // 0 0 1 1 1 1 9 8 7 6
    fwindow_write(w, v, 4);

    // push 4 more elements
    // 1 1 9 8 7 6 3 3 3 3
    fwindow_push(w, 3);
    fwindow_push(w, 3);
    fwindow_push(w, 3);
    fwindow_push(w, 3);

    // read
    fwindow_read(w, &r);

    // manual print
    printf("manual output:\n");
    for (i=0; i<10; i++)
        printf("%6u : %f\n", i, r[i]);

    fwindow_debug_print(w);

    fwindow_destroy(w);

    printf("done.\n");
    return 0;
}


