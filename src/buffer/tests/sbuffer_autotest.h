//
// Circular buffer autotest
//

#include "../../../autotest/autotest.h"
#include "../src/buffer_internal.h"

//
// AUTOTEST: static float buffer
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


//
// AUTOTEST: static complex float buffer
//
void autotest_cfbuffer_static()
{
    float complex v[] = {1, 2, 3, 4, 5, 6, 7, 8};
    float complex test1[] = {1, 2, 3, 4};
    float complex test2[] = {1, 2, 3, 4, 5, 6, 7, 8};
    float complex *r; // reader
    unsigned int n;

    cfbuffer cb = cfbuffer_create(STATIC,10);

    cfbuffer_write(cb, v, 4);
    n = 4;
    cfbuffer_read(cb, &r, &n);

    CONTEND_EQUALITY(n,4);
    CONTEND_SAME_DATA(r,test1,4*sizeof(float complex));

    cfbuffer_release(cb, 0);
    cfbuffer_write(cb, v, 8);
    n = 8;
    cfbuffer_read(cb, &r, &n);
    CONTEND_EQUALITY(n,8);
    CONTEND_SAME_DATA(r,test2,8*sizeof(float complex));

    cfbuffer_destroy(cb);
}


//
// AUTOTEST: static unsigned int buffer
//
void autotest_uibuffer_static()
{
    unsigned int v[] = {1, 2, 3, 4, 5, 6, 7, 8};
    unsigned int test1[] = {1, 2, 3, 4};
    unsigned int test2[] = {1, 2, 3, 4, 5, 6, 7, 8};
    unsigned int *r; // reader
    unsigned int n;

    uibuffer cb = uibuffer_create(STATIC,10);

    uibuffer_write(cb, v, 4);
    n = 4;
    uibuffer_read(cb, &r, &n);

    CONTEND_EQUALITY(n,4);
    CONTEND_SAME_DATA(r,test1,4*sizeof(unsigned int));

    uibuffer_release(cb, 0);
    uibuffer_write(cb, v, 8);
    n = 8;
    uibuffer_read(cb, &r, &n);
    CONTEND_EQUALITY(n,8);
    CONTEND_SAME_DATA(r,test2,8*sizeof(unsigned int));

    uibuffer_destroy(cb);
}


