//
// autotest_example.h, example autotest header
//

#ifndef __AUTOTEST_EXAMPLE_H__
#define __AUTOTEST_EXAMPLE_H__

#include "autotest.h"

void autotest_general()
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

void autotest_variables()
{
    int x = 3, y = 5;

    CONTEND_EQUALITY(x,y);
    CONTEND_INEQUALITY(x,y);
}

void autotest_expressions()
{
    CONTEND_EXPRESSION(1==1);
    CONTEND_EXPRESSION(1==2);
}

void autotest_same_data()
{
    float x[] = {1, 2, 3, 4, 5};
    float y[] = {1, 2, 3, 4, 5};
    float z[] = {0, 2, 3, 4, 5};

    CONTEND_SAME_DATA(x,y,5);
    CONTEND_SAME_DATA(x,z,5);
}

void autotest_no_tests()
{
    int x = 3, y = 5, z;
    z = x + y;
}

#endif // __AUTOTEST_EXAMPLE_H__
