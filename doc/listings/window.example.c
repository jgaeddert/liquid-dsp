#include <liquid/liquid.h>
// ...
{
    // initialize array for writing
    float v[] = {9, 8, 7, 6, 5, 4, 3, 2, 1, 0};

    // create window with 10 elements
    fwindow w = fwindow_create(10);
    // window[10] : {0 0 0 0 0 0 0 0 0 0}

    // push 4 elements into the window
    fwindow_push(w, 1);
    fwindow_push(w, 3);
    fwindow_push(w, 6);
    fwindow_push(w, 2);
    // window[10] : {0 0 0 0 0 0 1 3 6 2}

    // push 4 elements at a time
    fwindow_write(w, v, 4);
    // window[10] : {0 0 1 3 6 2 9 8 7 6}

    // recreate window (truncate to last 6 elements)
    w = fwindow_recreate(w,6);
    // window[6] : {6 2 9 8 7 6}

    // recreate window (extend to 12 elements)
    w = fwindow_recreate(w,12);
    // window[12] : {0 0 0 0 0 0 6 2 9 8 7 6}

    // read buffer (return pointer to aligned memory)
    float * r;
    fwindow_read(w, &r);
    // r[12] : {0 0 0 0 0 0 6 2 9 8 7 6}

    // clean up allocated object
    fwindow_destroy(w);
}
