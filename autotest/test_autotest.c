/*
 * Copyright (c) 2007, 2008, 2009, 2010 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010 Virginia Polytechnic
 *                                      Institute & State University
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
// autotest example
//

#include <stdio.h>
#include "autotest.h"

int main() {

    CONTEND_EQUALITY(1,1);
    CONTEND_EQUALITY(1,2);

    CONTEND_INEQUALITY(1,1);
    CONTEND_INEQUALITY(1,2);

    CONTEND_GREATER_THAN(1,1);
    CONTEND_GREATER_THAN(1,2);

    CONTEND_LESS_THAN(1,1);
    CONTEND_LESS_THAN(1,2);

    CONTEND_DELTA(1.0f, 1.0f, 0.1f);
    CONTEND_DELTA(1.0f, 2.0f, 0.1f);

    // unconditional pass
    AUTOTEST_PASS();

    // unconditional fail
    AUTOTEST_FAIL("unknown error");

    int x = 3, y = 5;

    CONTEND_INEQUALITY(x,y);

    float a[] = {1, 2, 3, 4};
    float b[] = {0, 2, 3, 4};
    CONTEND_SAME_DATA(a,b,4*sizeof(float));

    autotest_print_results();

    printf("done.\n");
    return 0;
}

