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
// buffer autotest (static)
//

#include "autotest/autotest.h"
#include "liquid.h"

#define SBUFFER_AUTOTEST_DEFINE_API(X,T)        \
    T v[] = {1, 2, 3, 4, 5, 6, 7, 8};           \
    T test1[] = {1, 2, 3, 4};                   \
    T test2[] = {1, 2, 3, 4, 5, 6, 7, 8};       \
    T test3[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; \
    T *r;                                       \
    unsigned int n;                             \
                                                \
    X() cb = X(_create)(STATIC,10);             \
                                                \
    X(_write)(cb, v, 4);                        \
    n = 4;                                      \
    X(_read)(cb, &r, &n);                       \
                                                \
    CONTEND_EQUALITY(n,4);                      \
    CONTEND_SAME_DATA(r,test1,4*sizeof(T));     \
                                                \
    X(_release)(cb, 0);                         \
    X(_write)(cb, v, 8);                        \
    n = 8;                                      \
    X(_read)(cb, &r, &n);                       \
    CONTEND_EQUALITY(n,8);                      \
    CONTEND_SAME_DATA(r,test2,8*sizeof(T));     \
                                                \
    X(_zero)(cb);                               \
    n = 10;                                     \
    X(_read)(cb, &r, &n);                       \
    CONTEND_EQUALITY(n,10);                     \
    CONTEND_SAME_DATA(r,test3,10*sizeof(T));    \
                                                \
    X(_destroy)(cb);


//
// AUTOTEST: static float buffer
//
void autotest_fbuffer_static()
{
    SBUFFER_AUTOTEST_DEFINE_API(BUFFER_MANGLE_FLOAT, float)
}


//
// AUTOTEST: static complex float buffer
//
void autotest_cfbuffer_static()
{
    SBUFFER_AUTOTEST_DEFINE_API(BUFFER_MANGLE_CFLOAT, float complex)
}


#if 0
//
// AUTOTEST: static unsigned int buffer
//
void xautotest_uibuffer_static()
{
    SBUFFER_AUTOTEST_DEFINE_API(BUFFER_MANGLE_UINT, unsigned int)
}
#endif

