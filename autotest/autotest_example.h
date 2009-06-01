/*
 * Copyright (c) 2007, 2009 Joseph Gaeddert
 * Copyright (c) 2007, 2009 Virginia Polytechnic Institute & State University
 *
 * This file is part of liquid.
 *
 * liquid is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * liquid is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with liquid.  If not, see <http://www.gnu.org/licenses/>.
 */

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
    float x[] = {1, 2, 3, 4};
    float y[] = {1, 2, 3, 4};
    float z[] = {0, 2, 3, 4};

    CONTEND_SAME_DATA(x,y,4*sizeof(float));
    CONTEND_SAME_DATA(x,z,4*sizeof(float));
}

void autotest_no_tests()
{
    int x = 3, y = 5, z;
    z = x + y;
}

#endif // __AUTOTEST_EXAMPLE_H__
