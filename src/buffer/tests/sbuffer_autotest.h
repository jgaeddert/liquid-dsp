//
// Circular buffer autotest
//

#include "autotest/autotest.h"
#include "liquid.composite.h"

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


//
// AUTOTEST: static unsigned int buffer
//
void autotest_uibuffer_static()
{
    SBUFFER_AUTOTEST_DEFINE_API(BUFFER_MANGLE_UINT, unsigned int)
}


