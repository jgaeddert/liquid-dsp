#ifndef __GPORT2_AUTOTEST_H__
#define __GPORT2_AUTOTEST_H__

#include "../../../autotest/autotest.h"
#include "../src/buffer.h"

// Help function to keep code base small
void autotest_gport2_basic()
{
    gport2 p = gport2_create(8,sizeof(int));
    int w[5];
    int r[5];
    int test0[2] = {0,1};
    int test1[4] = {2,3,0,1};
    int test2[5] = {2,3,2,3,4};

    // initialize write array
    int i;
    for (i=0; i<5; i++)
        w[i] = i;
    // producer:    0, 1, 2, 3, 4

    gport2_produce(p,(void*)w,4);
    // port:        0, 1, 2, 3
    gport2_consume(p,(void*)r,2);
    // consumer:    0, 1
    // port:        2, 3
    CONTEND_SAME_DATA(r,test0,2*sizeof(int));

    gport2_produce(p,(void*)w,4);
    // port:        2, 3, 0, 1, 2, 3
    gport2_consume(p,(void*)r,4);
    // consumer:    2, 3, 0, 1
    // port:        2, 3
    CONTEND_SAME_DATA(r,test1,4*sizeof(int));

    gport2_produce(p,(void*)(w+2),3);
    // port:        2, 3, 2, 3, 4
    gport2_consume(p,(void*)r,5);
    // consumer:    2, 3, 2, 3, 4
    // port:        <empty>
    CONTEND_SAME_DATA(r,test2,5*sizeof(int));


    gport2_destroy(p);
}

#endif // __GPORT2_AUTOTEST_H__

