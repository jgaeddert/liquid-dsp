/*
 * Copyright (c) 2007 - 2014 Joseph Gaeddert
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
// circular buffer autotest
//

#include "autotest/autotest.h"
#include "liquid.h"

// floating point
void autotest_cbufferf()
{
    // input array of values
    float v[] = {1, 2, 3, 4, 5, 6, 7, 8};

    // output test arrays
    float test1[] = {1, 2, 3, 4};
    float test2[] = {3, 4, 1, 2, 3, 4, 5, 6, 7, 8};
    float test3[] = {3, 4, 5, 6, 7, 8};
    float test4[] = {3, 4, 5, 6, 7, 8, 1, 2, 3};
    float *r;                   // output read pointer
    unsigned int num_requested; // number of samples requested
    unsigned int num_read;      // number of samples read

    // create new circular buffer with 10 elements
    cbufferf q = cbufferf_create(10);

    // write 4 elements to the buffer
    cbufferf_write(q, v, 4);

    // try to read 4 elements
    num_requested = 4;
    cbufferf_read(q, num_requested, &r, &num_read);
    CONTEND_EQUALITY(num_read,4);
    CONTEND_SAME_DATA(r,test1,4*sizeof(float));

    // release two elements, write 8 more, read 10
    cbufferf_release(q, 2);
    cbufferf_write(q, v, 8);
    num_requested = 10;
    cbufferf_read(q, num_requested, &r, &num_read);
    CONTEND_EQUALITY(num_read,10);
    CONTEND_SAME_DATA(r,test2,10*sizeof(float));

    // release four elements, and try reading 10
    cbufferf_release(q, 4);
    num_requested = 10;
    cbufferf_read(q, num_requested, &r, &num_read);
    CONTEND_EQUALITY(num_read,6);
    CONTEND_SAME_DATA(r,test3,6*sizeof(float));

    // test pushing multiple elements
    cbufferf_push(q, 1);
    cbufferf_push(q, 2);
    cbufferf_push(q, 3);
    num_requested = 10;
    cbufferf_read(q, num_requested, &r, &num_read);
    CONTEND_EQUALITY(num_read,9);
    CONTEND_SAME_DATA(r,test4,9*sizeof(float));

    // buffer should not be full
    CONTEND_EXPRESSION( cbufferf_is_full(q)==0 );

    // add one more element; buffer should be full
    cbufferf_push(q, 1);
    CONTEND_EXPRESSION( cbufferf_is_full(q)==1 );

    // memory leaks are evil
    cbufferf_destroy(q);
}



// complex float complexing point
void autotest_cbuffercf()
{
    // input array of values
    float complex v[] = {1.0 - 1.0 * _Complex_I,
                         2.0 + 2.0 * _Complex_I,
                         3.0 - 3.0 * _Complex_I,
                         4.0 + 4.0 * _Complex_I,
                         5.0 - 5.0 * _Complex_I,
                         6.0 + 6.0 * _Complex_I,
                         7.0 - 7.0 * _Complex_I,
                         8.0 + 8.0 * _Complex_I};

    // output test arrays
    float complex test1[] = {1.0 - 1.0 * _Complex_I,
                             2.0 + 2.0 * _Complex_I,
                             3.0 - 3.0 * _Complex_I,
                             4.0 + 4.0 * _Complex_I};
    float complex test2[] = {3.0 - 3.0 * _Complex_I,
                             4.0 + 4.0 * _Complex_I,
                             1.0 - 1.0 * _Complex_I,
                             2.0 + 2.0 * _Complex_I,
                             3.0 - 3.0 * _Complex_I,
                             4.0 + 4.0 * _Complex_I,
                             5.0 - 5.0 * _Complex_I,
                             6.0 + 6.0 * _Complex_I,
                             7.0 - 7.0 * _Complex_I,
                             8.0 + 8.0 * _Complex_I};
    float complex test3[] = {3.0 - 3.0 * _Complex_I,
                             4.0 + 4.0 * _Complex_I,
                             5.0 - 5.0 * _Complex_I,
                             6.0 + 6.0 * _Complex_I,
                             7.0 - 7.0 * _Complex_I,
                             8.0 + 8.0 * _Complex_I};
    float complex test4[] = {3.0 - 3.0 * _Complex_I,
                             4.0 + 4.0 * _Complex_I,
                             5.0 - 5.0 * _Complex_I,
                             6.0 + 6.0 * _Complex_I,
                             7.0 - 7.0 * _Complex_I,
                             8.0 + 8.0 * _Complex_I,
                             1.0 - 1.0 * _Complex_I,
                             2.0 + 2.0 * _Complex_I,
                             3.0 - 3.0 * _Complex_I};
    float complex *r;           // output read pointer
    unsigned int num_requested; // number of samples requested
    unsigned int num_read;      // number of samples read

    // create new circular buffer with 10 elements
    cbuffercf q = cbuffercf_create(10);

    // write 4 elements to the buffer
    cbuffercf_write(q, v, 4);

    // try to read 4 elements
    num_requested = 4;
    cbuffercf_read(q, num_requested, &r, &num_read);
    CONTEND_EQUALITY(num_read,4);
    CONTEND_SAME_DATA(r,test1,4*sizeof(float complex));

    // release two elements, write 8 more, read 10
    cbuffercf_release(q, 2);
    cbuffercf_write(q, v, 8);
    num_requested = 10;
    cbuffercf_read(q, num_requested, &r, &num_read);
    CONTEND_EQUALITY(num_read,10);
    CONTEND_SAME_DATA(r,test2,10*sizeof(float complex));

    // release four elements, and try reading 10
    cbuffercf_release(q, 4);
    num_requested = 10;
    cbuffercf_read(q, num_requested, &r, &num_read);
    CONTEND_EQUALITY(num_read,6);
    CONTEND_SAME_DATA(r,test3,6*sizeof(float complex));

    // test pushing multiple elements
    cbuffercf_push(q, 1.0 - 1.0 * _Complex_I);
    cbuffercf_push(q, 2.0 + 2.0 * _Complex_I);
    cbuffercf_push(q, 3.0 - 3.0 * _Complex_I);
    num_requested = 10;
    cbuffercf_read(q, num_requested, &r, &num_read);
    CONTEND_EQUALITY(num_read,9);
    CONTEND_SAME_DATA(r,test4,9*sizeof(float complex));

    // buffer should not be full
    CONTEND_EXPRESSION( cbuffercf_is_full(q)==0 );

    // add one more element; buffer should be full
    cbuffercf_push(q, 1.0 - 1.0 * _Complex_I);
    CONTEND_EXPRESSION( cbuffercf_is_full(q)==1 );

    // memory leaks are evil
    cbuffercf_destroy(q);
}

