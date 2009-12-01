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
    float *r;   // reader pointer
    float x;    // temporary value holder
    unsigned int i;

    float test0[10] = {0,0,0,0,0,0,0,0,0,0};
    float test1[10] = {0,0,0,0,0,0,1,1,1,1};
    float test2[10] = {0,0,1,1,1,1,9,8,7,6};
    float test3[10] = {1,1,9,8,7,6,3,3,3,3};
    float test4[10] = {7,6,3,3,3,3,5,5,5,5};
    float test5[6]  = {3,3,5,5,5,5};
    float test6[6]  = {5,5,5,5,6,7};
    float test7[10] = {0,0,0,0,5,5,5,5,6,7};
    float test8[10] = {0,0,0,0,0,0,0,0,0,0};

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

    // test indexing operation
    fwindow_index(w, 0, &x);    CONTEND_EQUALITY(x, 1);
    fwindow_index(w, 1, &x);    CONTEND_EQUALITY(x, 1);
    fwindow_index(w, 2, &x);    CONTEND_EQUALITY(x, 9);
    fwindow_index(w, 3, &x);    CONTEND_EQUALITY(x, 8);
    fwindow_index(w, 4, &x);    CONTEND_EQUALITY(x, 7);
    fwindow_index(w, 5, &x);    CONTEND_EQUALITY(x, 6);
    fwindow_index(w, 6, &x);    CONTEND_EQUALITY(x, 3);
    fwindow_index(w, 7, &x);    CONTEND_EQUALITY(x, 3);
    fwindow_index(w, 8, &x);    CONTEND_EQUALITY(x, 3);
    fwindow_index(w, 9, &x);    CONTEND_EQUALITY(x, 3);

    // push 4 more elements
    // 7 6 3 3 3 3 5 5 5 5
    fwindow_push(w, 5);
    fwindow_push(w, 5);
    fwindow_push(w, 5);
    fwindow_push(w, 5);

    fwindow_read(w, &r);
    CONTEND_SAME_DATA(r,test4,10*sizeof(float));
    if (_autotest_verbose)
        fwindow_debug_print(w);

    // recreate window (truncate to last 6 elements)
    // 3 3 5 5 5 5
    w = fwindow_recreate(w,6);
    fwindow_read(w, &r);
    CONTEND_SAME_DATA(r,test5,6*sizeof(float));

    // push 2 more elements
    // 5 5 5 5 6 7
    fwindow_push(w, 6);
    fwindow_push(w, 7);
    fwindow_read(w, &r);
    CONTEND_SAME_DATA(r,test6,6*sizeof(float));

    // recreate window (extend to 10 elements)
    // 0 0 0 0 5 5 5 5 6 7
    w = fwindow_recreate(w,10);
    fwindow_read(w,&r);
    CONTEND_SAME_DATA(r,test7,10*sizeof(float));

    // clear
    // 0 0 0 0 0 0 0 0 0 0
    fwindow_clear(w);

    fwindow_read(w, &r);
    CONTEND_SAME_DATA(r,test8,10*sizeof(float));

    if (_autotest_verbose) {
        // manual print
        printf("manual output:\n");
        for (i=0; i<10; i++)
            printf("%6u : %f\n", i, r[i]);

        fwindow_debug_print(w);
    }

    fwindow_destroy(w);

    printf("done.\n");
}

#endif // __LIQUID_WINDOW_AUTOTEST_H__

