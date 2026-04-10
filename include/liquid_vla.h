/*
 * MSVC compatibility header for liquid-dsp
 *
 * Provides compatibility macros for:
 * - VLAs (Variable Length Arrays) - MSVC uses _alloca()
 * - void* to typed pointer casts (required in C++)
 * - Complex number operations
 * - div() function ambiguity
 * - Memory alignment for SIMD
 *
 * Usage:
 *   Instead of:  float arr[n];
 *   Use:         LIQUID_VLA(float, arr, n);
 *
 *   Instead of:  float arr[8] __attribute__((aligned(32)));
 *   Use:         LIQUID_ALIGNED(32) float arr[8];
 */

#ifndef LIQUID_VLA_H
#define LIQUID_VLA_H

#ifdef _MSC_VER
    /* MSVC: Use _alloca() for stack allocation */
    #include <malloc.h>
    #include <stdlib.h>

    /* VLA macro: allocates on stack using _alloca */
    #define LIQUID_VLA(type, name, size) \
        type* name = (type*)_alloca((size) * sizeof(type))

    /* VLA with initialization to zero */
    #define LIQUID_VLA_ZERO(type, name, size) \
        type* name = (type*)_alloca((size) * sizeof(type)); \
        memset(name, 0, (size) * sizeof(type))

    /* Cast macro for malloc/calloc returns (C++ requires explicit cast) */
    #define LIQUID_ALLOC(type, ptr) ((type)(ptr))

    /* Use C library div() to avoid C++ ambiguity */
    #define liquid_div(n, d)  ::div((int)(n), (int)(d))

    /* Memory alignment macro for SIMD
     * MSVC: __declspec(align(n)) must be placed BEFORE the type
     * Usage: LIQUID_DEFINE_ALIGNED_ARRAY(float, arr, 8, 32) = {0};
     */
    #define LIQUID_DEFINE_ALIGNED_ARRAY(type, name, size, alignment) \
        __declspec(align(alignment)) type name[size]

#else
    /* GCC/Clang: Use native VLAs */
    #define LIQUID_VLA(type, name, size) \
        type name[size]

    #define LIQUID_VLA_ZERO(type, name, size) \
        type name[size]; \
        memset(name, 0, (size) * sizeof(type))

    /* C allows implicit void* conversion */
    #define LIQUID_ALLOC(type, ptr) (ptr)

    /* Standard div() on non-MSVC */
    #define liquid_div(n, d)  div((n), (d))

    /* Memory alignment macro for SIMD
     * GCC/Clang: __attribute__((aligned(n))) is placed AFTER the declaration
     * Usage: LIQUID_DEFINE_ALIGNED_ARRAY(float, arr, 8, 32) = {0};
     */
    #define LIQUID_DEFINE_ALIGNED_ARRAY(type, name, size, alignment) \
        type name[size] __attribute__((aligned(alignment)))

#endif

#endif /* LIQUID_VLA_H */
