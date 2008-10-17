//
// Circular buffer autotest
//

#include "../../../autotest/autotest.h"
#include "../src/buffer_internal.h"

//
// AUTOTEST:
//
void autotest_fbuffer_static()
{
    float v[] = {1, 2, 3, 4, 5, 6, 7, 8};
    float test1[] = {1, 2, 3, 4};
    float test2[] = {1, 2, 3, 4, 5, 6, 7, 8};
    float *r; // reader
    unsigned int n;

    fbuffer cb = fbuffer_create(STATIC,10);

    fbuffer_write(cb, v, 4);
    n = 4;
    fbuffer_read(cb, &r, &n);

    CONTEND_EQUALITY(n,4);
    CONTEND_SAME_DATA(r,test1,4*sizeof(float));

    fbuffer_release(cb, 0);
    fbuffer_write(cb, v, 8);
    n = 8;
    fbuffer_read(cb, &r, &n);
    CONTEND_EQUALITY(n,8);
    CONTEND_SAME_DATA(r,test2,8*sizeof(float));

    fbuffer_destroy(cb);
}


