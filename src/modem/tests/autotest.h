#ifndef __AUTOTEST_H__
#define __AUTOTEST_H__

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define TS_ASSERT_EQUALS(a,b)                       \
    if ((a)!=(b)) printf("fail\n");

#define TS_ASSERT_DELTA(a,b,delta)                  \
    if (fabsf((a)-(b)) > delta) printf("fail\n");


#endif // __AUTOTEST_H__

