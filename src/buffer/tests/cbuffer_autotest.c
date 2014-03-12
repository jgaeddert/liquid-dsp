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

// 
// Macro
//
#define CBUFFER_AUTOTEST_DEFINE_API(X,T)                \
    /* input array of values                        */  \
    T v[] = {1, 2, 3, 4, 5, 6, 7, 8};                   \
                                                        \
    /* output test arrays                           */  \
    T test1[] = {1, 2, 3, 4};                           \
    T test2[] = {3, 4, 1, 2, 3, 4, 5, 6, 7, 8};         \
    T test3[] = {3, 4, 5, 6, 7, 8};                     \
    T test4[] = {3, 4, 5, 6, 7, 8, 1, 2, 3};            \
    T *r;           /* output read pointer          */  \
    unsigned int n; /* output sample size           */  \
                                                        \
    /* create new circular buffer with 10 elements  */  \
    X() cb = X(_create)(10);                            \
                                                        \
    /* write 4 elements to the buffer               */  \
    X(_write)(cb, v, 4);                                \
                                                        \
    /* try to read 4 elements                       */  \
    n = 4;                                              \
    X(_read)(cb, &r, &n);                               \
    CONTEND_EQUALITY(n,4);                              \
    CONTEND_SAME_DATA(r,test1,4*sizeof(T));             \
                                                        \
    /* release two elements, write 8 more, read 10  */  \
    X(_release)(cb, 2);                                 \
    X(_write)(cb, v, 8);                                \
    n = 10;                                             \
    X(_read)(cb, &r, &n);                               \
    CONTEND_EQUALITY(n,10);                             \
    CONTEND_SAME_DATA(r,test2,10*sizeof(T));            \
                                                        \
    /* release four elements, and try reading 10    */  \
    X(_release)(cb, 4);                                 \
    n = 10;                                             \
    X(_read)(cb, &r, &n);                               \
    CONTEND_EQUALITY(n,6);                              \
    CONTEND_SAME_DATA(r,test3,6*sizeof(T));             \
                                                        \
    /* test pushing multiple elements               */  \
    X(_push)(cb, 1);                                    \
    X(_push)(cb, 2);                                    \
    X(_push)(cb, 3);                                    \
    n = 10;                                             \
    X(_read)(cb, &r, &n);                               \
    CONTEND_EQUALITY(n,9);                              \
    CONTEND_SAME_DATA(r,test4,9*sizeof(T));             \
                                                        \
    /* buffer should not be full */                     \
    CONTEND_EXPRESSION( X(_is_full)(cb)==0 );           \
                                                        \
    /* add one more element; buffer should be full */   \
    X(_push)(cb, 1);                                    \
    CONTEND_EXPRESSION( X(_is_full)(cb)==1 );           \
                                                        \
    /* memory leaks are evil                        */  \
    X(_destroy)(cb);                                    \


//
// AUTOTEST: circular float buffer
//
void autotest_cbufferf_circular()
{
    CBUFFER_AUTOTEST_DEFINE_API(CBUFFER_MANGLE_FLOAT, float)
}


//
// AUTOTEST: circular complex float buffer
//
void autotest_cbuffercf_circular()
{
    CBUFFER_AUTOTEST_DEFINE_API(CBUFFER_MANGLE_CFLOAT, float complex)
}

