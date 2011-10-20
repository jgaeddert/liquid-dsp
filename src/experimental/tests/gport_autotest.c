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

#include "autotest/autotest.h"
#include "liquid.experimental.h"

// 
// AUTOTEST: basic gport functionality
//
void autotest_gport_basic()
{
    gport p = gport_create(8,sizeof(int));
    int w[5];
    int r[5];
    int test0[2] = {0,1};
    int test1[4] = {2,3,0,1};
    int test2[4] = {2,2,3,4};
    int test3[5] = {0,1,2,3,4};

    // initialize write array
    int i;
    for (i=0; i<5; i++)
        w[i] = i;
    // producer:    0, 1, 2, 3, 4

    gport_produce(p,(void*)w,4);
    // port:        0, 1, 2, 3
    gport_consume(p,(void*)r,2);
    // consumer:    0, 1
    // port:        2, 3
    CONTEND_SAME_DATA(r,test0,2*sizeof(int));

    gport_produce(p,(void*)w,3);
    // port:        2, 3, 0, 1, 2
    gport_consume(p,(void*)r,4);
    // consumer:    2, 3, 0, 1
    // port:        2
    CONTEND_SAME_DATA(r,test1,4*sizeof(int));

    gport_produce(p,(void*)(w+2),3);
    // port:        2, 2, 3, 4
    gport_consume(p,(void*)r,4);
    // consumer:    2, 2, 3, 4
    // port:        <empty>
    CONTEND_SAME_DATA(r,test2,4*sizeof(int));

    gport_produce(p,(void*)w,5);
    // port:        0, 1, 2, 3, 4
    gport_consume(p,(void*)r,5);
    // consumer:    0, 1, 2, 3, 4
    // port:        <empty>
    CONTEND_SAME_DATA(r,test3,5*sizeof(int));


    gport_destroy(p);
}

// 
// AUTOTEST: consume_available
//
void autotest_gport_consume_available()
{
    gport p = gport_create(20,sizeof(int));
    int w[5];
    int r[5];
    int test0[4] = {0,1,2,3};
    int test1[3] = {0,1,2};
    int test2[5] = {3,4,0,1,2};
    int test3[2] = {3,4};

    // initialize write array
    int i;
    for (i=0; i<5; i++)
        w[i] = i;
    // producer:    0, 1, 2, 3, 4

    unsigned int n;

    gport_produce(p,(void*)w,4);
    // port:        0, 1, 2, 3
    gport_consume_available(p,(void*)r,5,&n);
    // consumer:    0, 1, 2, 3
    // port:        <empty>
    CONTEND_EQUALITY(n,4);
    CONTEND_SAME_DATA(r,test0,4*sizeof(int));

    gport_produce(p,(void*)w,5);
    gport_produce(p,(void*)w,5);
    // port:        0, 1, 2, 3, 4, 0, 1, 2, 3, 4
    gport_consume_available(p,(void*)r,3,&n);
    // consumer:    0, 1, 2
    // port:        3, 4, 0, 1, 2, 3, 4
    CONTEND_EQUALITY(n,3);
    CONTEND_SAME_DATA(r,test1,3*sizeof(int));

    gport_consume_available(p,(void*)r,5,&n);
    // consumer:    3, 4, 0, 1, 2
    // port:        3, 4
    CONTEND_EQUALITY(n,5);
    CONTEND_SAME_DATA(r,test2,5*sizeof(int));

    gport_consume_available(p,(void*)r,5,&n);
    // consumer:    3, 4
    // port:        <empty>
    CONTEND_EQUALITY(n,2);
    CONTEND_SAME_DATA(r,test3,2*sizeof(int));

    gport_destroy(p);
}


// 
// AUTOTEST: produce_available
//
void autotest_gport_produce_available()
{
    gport p = gport_create(8,sizeof(int));
    int w[5];
    int r[8];
    int test0[4] = {0,1,2,3};
    int test1[3] = {0,1,2};
    int test2[8] = {3,4,0,1,2,0,1,2};
    // consumer:    3,4,0,1,2,0,1,2

    // initialize write array
    int i;
    for (i=0; i<5; i++)
        w[i] = i;
    // producer:    0, 1, 2, 3, 4

    unsigned int np; // number of samples produced

    gport_produce_available(p,(void*)w,4,&np);
    // port:        0, 1, 2, 3
    gport_consume(p,(void*)r,4);
    // consumer:    0, 1, 2, 3
    // port:        <empty>
    CONTEND_EQUALITY(np,4);
    CONTEND_SAME_DATA(r,test0,4*sizeof(int));

    gport_produce_available(p,(void*)w,5,&np);
    CONTEND_EQUALITY(np,5);
    // port:        0, 1, 2, 3, 4
    gport_produce_available(p,(void*)w,5,&np);
    CONTEND_EQUALITY(np,3);
    // port:        0, 1, 2, 3, 4, 0, 1, 2
    gport_consume(p,(void*)r,3);
    // consumer:    0, 1, 2
    // port:        3, 4, 0, 1, 2
    CONTEND_SAME_DATA(r,test1,3*sizeof(int));

    gport_produce_available(p,(void*)w,5,&np);
    CONTEND_EQUALITY(np,3);
    // port:        3, 4, 0, 1, 2, 0, 1, 2
    gport_consume(p,(void*)r,8);
    // consumer:    3, 4, 0, 1, 2, 0, 1, 2
    // port:        <empty>
    CONTEND_SAME_DATA(r,test2,8*sizeof(int));

    gport_destroy(p);
}

