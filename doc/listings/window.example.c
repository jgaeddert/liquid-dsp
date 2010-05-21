#include <liquid/liquid.h>
// ...
{
    float v[] = {9, 8, 7, 6, 5, 4, 3, 2, 1, 0};

    // create window with 10 elements
    // 0 0 0 0 0 0 0 0 0 0
    fwindow w = fwindow_create(10);

    // push 4 elements into the window
    // 0 0 0 0 0 0 1 3 6 2
    fwindow_push(w, 1);
    fwindow_push(w, 3);
    fwindow_push(w, 6);
    fwindow_push(w, 2);

    // push 4 elements at a time
    // 0 0 1 3 6 2 9 8 7 6
    fwindow_write(w, v, 4);

    // recreate window (truncate to last 6 elements)
    // 6 2 9 8 7 6
    w = fwindow_recreate(w,6);

    // recreate window (extend to 12 elements)
    // 0 0 0 0 0 0 6 2 9 8 7 6
    w = fwindow_recreate(w,12);

    // read buffer (return pointer to aligned memory)
    float * r;
    fwindow_read(w, &r);

    // clean up allocated object
    fwindow_destroy(w);
}
