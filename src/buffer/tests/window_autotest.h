#ifndef __LIQUID_WINDOW_AUTOTEST_H__
#define __LIQUID_WINDOW_AUTOTEST_H__

#include "autotest/autotest.h"
#include "liquid.h"

//
// AUTOTEST: fwindow
//
void autotest_fwindow()
{
    float v[] = {9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
    float *r; // reader
    unsigned int i;

    float test0[10] = {0,0,0,0,0,0,0,0,0,0};
    float test1[10] = {0,0,0,0,0,0,1,1,1,1};
    float test2[10] = {0,0,1,1,1,1,9,8,7,6};
    float test3[10] = {1,1,9,8,7,6,3,3,3,3};
    float test4[10] = {0,0,0,0,0,0,0,0,0,0};

    // create window
    // 0 0 0 0 0 0 0 0 0 0
    fwindow w = fwindow_create(10);

    fwindow_read(w, &r);
    CONTEND_SAME_DATA(r,test0,10*sizeof(float));

    // push 4 elements
    // 0 0 0 0 0 0 1 1 1 1
    fwindow_push(w, 1);
    fwindow_push(w, 1);
    fwindow_push(w, 1);
    fwindow_push(w, 1);

    fwindow_read(w, &r);
    CONTEND_SAME_DATA(r,test1,10*sizeof(float));

    // push 4 more elements
    // 0 0 1 1 1 1 9 8 7 6
    fwindow_write(w, v, 4);

    fwindow_read(w, &r);
    CONTEND_SAME_DATA(r,test2,10*sizeof(float));

    // push 4 more elements
    // 1 1 9 8 7 6 3 3 3 3
    fwindow_push(w, 3);
    fwindow_push(w, 3);
    fwindow_push(w, 3);
    fwindow_push(w, 3);

    fwindow_read(w, &r);
    CONTEND_SAME_DATA(r,test3,10*sizeof(float));

    // clear
    // 0 0 0 0 0 0 0 0 0 0
    fwindow_clear(w);

    fwindow_read(w, &r);
    CONTEND_SAME_DATA(r,test4,10*sizeof(float));

    // manual print
    printf("manual output:\n");
    for (i=0; i<10; i++)
        printf("%6u : %f\n", i, r[i]);

    fwindow_debug_print(w);

    fwindow_destroy(w);

    printf("done.\n");
}

#endif // __LIQUID_WINDOW_AUTOTEST_H__

