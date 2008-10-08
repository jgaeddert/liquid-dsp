//
// autotest_example.h, example autotest header
//

#ifndef __AUTOTEST_EXAMPLE_H__
#define __AUTOTEST_EXAMPLE_H__

#include "autotest.h"

void autotest_example_01()
{
    CONTEND_EQUALITY(1,1);
    CONTEND_EQUALITY(1,2);

    CONTEND_INEQUALITY(1,1);
    CONTEND_INEQUALITY(1,2);

    CONTEND_GREATER_THAN(1,1);
    CONTEND_GREATER_THAN(1,2);

    CONTEND_LESS_THAN(1,1);
    CONTEND_LESS_THAN(1,2);
}

void autotest_example_02()
{
    int x = 3, y = 5;

    CONTEND_EQUALITY(x,y);
    CONTEND_INEQUALITY(x,y);
}

void autotest_example_03()
{
    int x = 3, y = 5, z;
    z = x + y;
}

#endif // __AUTOTEST_EXAMPLE_H__
