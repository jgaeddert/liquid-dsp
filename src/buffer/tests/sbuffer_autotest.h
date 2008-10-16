//
// Circular buffer autotest
//

#include "../../../autotest/autotest.h"
#include "../src/buffer_internal.h"

//
// AUTOTEST:
//
void autotest_buffer_static()
{
    float v[] = {1, 2, 3, 4, 5, 6, 7, 8};
    float test1[] = {1, 2, 3, 4};
    float test2[] = {1, 2, 3, 4, 5, 6, 7, 8};
    float *r; // reader
    unsigned int n;

    buffer cb = buffer_create(STATIC,10);

    buffer_write(cb, v, 4);
    n = 4;
    buffer_read(cb, &r, &n);

    CONTEND_EQUALITY(n,4);
    CONTEND_SAME_DATA(r,test1,4*sizeof(float));

    buffer_release(cb, 0);
    buffer_write(cb, v, 8);
    n = 8;
    buffer_read(cb, &r, &n);
    CONTEND_EQUALITY(n,8);
    CONTEND_SAME_DATA(r,test2,8*sizeof(float));

    buffer_destroy(cb);
}


