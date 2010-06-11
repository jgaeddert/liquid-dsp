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
// Circular buffer autotest
//

#include "autotest/autotest.h"
#include "liquid.h"

// 
// Macro
//
#define CBUFFER_AUTOTEST_DEFINE_API(X,T)        \
    T v[] = {1, 2, 3, 4, 5, 6, 7, 8};           \
    T test1[] = {1, 2, 3, 4};                   \
    T test2[] = {3, 4, 1, 2, 3, 4, 5, 6, 7, 8}; \
    T test3[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; \
    T test4[] = {0, 0, 0, 0, 0, 0, 0, 1, 2, 3}; \
    T *r;                                       \
    unsigned int n;                             \
                                                \
    X() cb = X(_create)(CIRCULAR,10);           \
                                                \
    X(_write)(cb, v, 4);                        \
    n = 4;                                      \
    X(_read)(cb, &r, &n);                       \
                                                \
    CONTEND_EQUALITY(n,4);                      \
    CONTEND_SAME_DATA(r,test1,4*sizeof(T));     \
                                                \
    X(_release)(cb, 2);                         \
    X(_write)(cb, v, 8);                        \
    n = 10;                                     \
    X(_read)(cb, &r, &n);                       \
    CONTEND_EQUALITY(n,10);                     \
    CONTEND_SAME_DATA(r,test2,10*sizeof(T));    \
                                                \
    X(_zero)(cb);                               \
    n = 10;                                     \
    X(_read)(cb, &r, &n);                       \
    CONTEND_EQUALITY(n,10);                     \
    CONTEND_SAME_DATA(r,test3,10*sizeof(T));    \
                                                \
    X(_push)(cb, 1);                            \
    X(_push)(cb, 2);                            \
    X(_push)(cb, 3);                            \
    n = 10;                                     \
    X(_read)(cb, &r, &n);                       \
    CONTEND_EQUALITY(n,10);                     \
    CONTEND_SAME_DATA(r,test4,10*sizeof(T));    \
                                                \
    X(_destroy)(cb);

//
// AUTOTEST: circular float buffer
//
void autotest_fbuffer_circular()
{
    CBUFFER_AUTOTEST_DEFINE_API(BUFFER_MANGLE_FLOAT, float)
}


//
// AUTOTEST: circular complex float buffer
//
void autotest_cfbuffer_circular()
{
    CBUFFER_AUTOTEST_DEFINE_API(BUFFER_MANGLE_CFLOAT, float complex)
}

#if 0
//
// AUTOTEST: unsigned int buffer
//
void xautotest_uibuffer_circular()
{
    CBUFFER_AUTOTEST_DEFINE_API(BUFFER_MANGLE_UINT, unsigned int)
}
#endif

