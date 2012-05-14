/*
 * Copyright (c) 2008, 2009, 2010, 2011, 2012 Joseph Gaeddert
 * Copyright (c) 2008, 2009, 2010, 2011, 2012 Virginia Polytechnic
 *                                Institute & State University
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

#ifndef __LIQUIDFPM_H__
#define __LIQUIDFPM_H__

#ifdef __cplusplus
extern "C" {
#   define LIQUIDFPM_USE_COMPLEX_H 0
#else
#   define LIQUIDFPM_USE_COMPLEX_H 1
#endif // __cplusplus

/*
 * Make sure the version and version number macros weren't defined by
 * some prevoiusly included header file.
 */
#ifdef LIQUIDFPM_VERSION
#  undef LIQUIDFPM_VERSION
#endif
#ifdef LIQUIDPM_VERSION_NUMBER
#  undef LIQUIDPM_VERSION_NUMBER
#endif

#include <inttypes.h>

/* concatenation function */
#define LIQUIDFPM_CONCAT(prefix, name) prefix ## name

/* 
 * Default: use the C99 complex data type, otherwise
 * define complex type compatible with the C++ complex standard,
 * otherwise resort to defining binary compatible array.
 */
#if LIQUIDFPM_USE_COMPLEX_H==1
#   include <complex.h>
#   define LIQUIDFPM_DEFINE_COMPLEX(R,C) typedef R _Complex C
#elif defined _GLIBCXX_COMPLEX
#   define LIQUIDFPM_DEFINE_COMPLEX(R,C) typedef std::complex<R> C
#else
#   define LIQUIDFPM_DEFINE_COMPLEX(R,C) typedef struct {R real; R imag;} C;
#endif
//#   define LIQUIDFPM_DEFINE_COMPLEX(R,C) typedef R C[2]

LIQUIDFPM_DEFINE_COMPLEX(float, liquidfpm_float_complex);

//
// q16 type
//

// name-mangling macros
#define LIQUIDFPM_MANGLE_Q16(name)  LIQUIDFPM_CONCAT(q16, name)
#define LIQUIDFPM_MANGLE_CQ16(name) LIQUIDFPM_CONCAT(cq16,name)

// type definition
#define LIQUIDFPM_Q16_BITS          (16)        // number of bits
#define LIQUIDFPM_Q16_INTBITS       (7)         // number of integer bits
#define LIQUIDFPM_Q16_FRACBITS      (9)         // number of fraction bits
#if (LIQUIDFPM_Q16_INTBITS + LIQUIDFPM_Q16_FRACBITS) != LIQUIDFPM_Q16_BITS
#  error "invalid q16 bit allocation"
#endif

// constants
#define q16_bits        (LIQUIDFPM_Q16_BITS)    // number of bits
#define q16_intbits     (LIQUIDFPM_Q16_INTBITS) // number of integer bits
#define q16_fracbits    (LIQUIDFPM_Q16_FRACBITS)// number of fraction bits
#define q16_min         (1)                     // minimum value
#define q16_max         (0x7fff)                // maximum value (positive)
#define q16_one         ((1<<(q16_fracbits))-1) // 'one' representation
#define q16_pi_by_4     (0x0fff)                // phase: pi/4
#define q16_pi_by_2     (0x1fff)                // phase: pi/2
#define q16_pi          (0x3fff)                // phase: pi
#define q16_2pi         (0x7fff)                // phase: 2*pi

// inline methods
#define q16_intpart(X)      ((X)>>LIQUIDFPM_Q16_FRACBITS)
#define q16_fracpart(X)     ((X) & ((1<<LIQUIDFPM_Q16_FRACBITS)-1))
#define q16_mul_inline(X,Y) ( ((int32_t)(X)*(int32_t)(Y))>>(LIQUIDFPM_Q16_FRACBITS) )
#define q16_div_inline(X,Y) ( (((int32_t)X)<<(LIQUIDFPM_Q16_FRACBITS)) / (Y) )


//
// q32 type
//

// name-mangling macros
#define LIQUIDFPM_MANGLE_Q32(name)  LIQUIDFPM_CONCAT(q32, name)
#define LIQUIDFPM_MANGLE_CQ32(name) LIQUIDFPM_CONCAT(cq32,name)

// type definition
#define LIQUIDFPM_Q32_BITS          (32)        // number of bits
#define LIQUIDFPM_Q32_INTBITS       (7)         // number of integer bits
#define LIQUIDFPM_Q32_FRACBITS      (25)        // number of fraction bits
#if (LIQUIDFPM_Q32_INTBITS + LIQUIDFPM_Q32_FRACBITS) != LIQUIDFPM_Q32_BITS
#  error "invalid q32 bit allocation"
#endif

// constants
#define q32_bits        (LIQUIDFPM_Q32_BITS)    // number of bits
#define q32_intbits     (LIQUIDFPM_Q32_INTBITS) // number of integer bits
#define q32_fracbits    (LIQUIDFPM_Q32_FRACBITS)// number of fraction bits
#define q32_min         (1)                     // minimum value
#define q32_max         (0x7fffffff)            // maximum value (positive)
#define q32_one         ((1<<(q32_fracbits))-1) // 'one' representation
#define q32_pi_by_4     (0x0fffffff)            // phase: pi/4
#define q32_pi_by_2     (0x1fffffff)            // phase: pi/2
#define q32_pi          (0x3fffffff)            // phase: pi
#define q32_2pi         (0x7fffffff)            // phase: 2*pi

// inline methods
#define q32_intpart(X)      ((X)>>LIQUIDFPM_Q32_FRACBITS)
#define q32_fracpart(X)     ((X) & ((1<<LIQUIDFPM_Q32_FRACBITS)-1))
#define q32_mul_inline(X,Y) ( ((int64_t)(X)*(int64_t)(Y))>>(LIQUIDFPM_Q32_FRACBITS) )
#define q32_div_inline(X,Y) ( (((int64_t)X)<<(LIQUIDFPM_Q32_FRACBITS)) / (Y) )


/* API definition macro
 *
 * Q        :   name-mangling macro (real)
 * CQ       :   name-mangling macro (complex)
 * T        :   primitive data type
 * TA       :   primitive data type (accumulator)
 * INTBITS  :   number of integer bits
 * FRACBITS :   number of fractional bits
 */
#define LIQUIDFPM_DEFINE_API(Q,CQ,T,TA,INTBITS,FRACBITS)            \
typedef T  Q(_t);                                                   \
typedef TA Q(_at);                                                  \
                                                                    \
/* constants */                                                     \
const static float Q(_angle_scalarf) =                              \
    (3.14159265358979/(float)(1<<(INTBITS-2)));                     \
                                                                    \
/* conversion */                                                    \
static inline float Q(_fixed_to_float)(Q(_t) _x)                    \
    { return (float) (_x) / (float)(1 << FRACBITS); };              \
static inline Q(_t) Q(_float_to_fixed)(float _x)                    \
    { return (Q(_t)) (_x * (float)(1 << FRACBITS) + 0.5f); };       \
static inline float Q(_angle_fixed_to_float)(Q(_t) _x)              \
    { return Q(_fixed_to_float(_x)) * Q(_angle_scalarf); };         \
static inline Q(_t) Q(_angle_float_to_fixed)(float _x)              \
    { return Q(_float_to_fixed(_x / Q(_angle_scalarf))); };         \
                                                                    \
/* arithmetic */                                                    \
static inline Q(_t) Q(_sign)(Q(_t) _x) {return (_x<0) ? -1 : 1;};   \
static inline Q(_t) Q(_abs) (Q(_t) _x) {return (_x<0) ? -_x : _x;}; \
static inline Q(_t) Q(_add) (Q(_t) _a, Q(_t) _b) {return _a + _b;}; \
static inline Q(_t) Q(_sub) (Q(_t) _a, Q(_t) _b) {return _a - _b;}; \
Q(_t) Q(_mul)(Q(_t) _a, Q(_t) _b);                                  \
Q(_t) Q(_div)(Q(_t) _a, Q(_t) _b);                                  \
Q(_t) Q(_ceil)(Q(_t) _x);                                           \
Q(_t) Q(_floor)(Q(_t) _x);                                          \
Q(_t) Q(_inv)(Q(_t) _x, unsigned int _n); /* compute 1/_x */        \
Q(_t) Q(_ratio)(int _a, int _b, unsigned int _n); /* compute a/b */ \
                                                                    \
/* trig */                                                          \
Q(_t) Q(_cos)(Q(_t) _theta);                                        \
Q(_t) Q(_sin)(Q(_t) _theta);                                        \
Q(_t) Q(_tan)(Q(_t) _theta);                                        \
Q(_t) Q(_acos)(Q(_t) _x);                                           \
Q(_t) Q(_asin)(Q(_t) _x);                                           \
Q(_t) Q(_atan)(Q(_t) _x);                                           \
Q(_t) Q(_atan2)(Q(_t) _y, Q(_t) _x);                                \
void  Q(_sincos)(Q(_t) _theta, Q(_t) * _sin, Q(_t) * _cos);         \
                                                                    \
/* hyperbolic */                                                    \
Q(_t) Q(_cosh) (Q(_t) _x);                                          \
Q(_t) Q(_sinh) (Q(_t) _x);                                          \
Q(_t) Q(_tanh) (Q(_t) _x);                                          \
Q(_t) Q(_acosh)(Q(_t) _x);                                          \
Q(_t) Q(_asinh)(Q(_t) _x);                                          \
Q(_t) Q(_atanh)(Q(_t) _x);                                          \
                                                                    \
/* exponential and logarithmic */                                   \
Q(_t) Q(_exp)  (Q(_t) _x);                                          \
Q(_t) Q(_exp2) (Q(_t) _x);                                          \
Q(_t) Q(_expm1)(Q(_t) _x);                                          \
Q(_t) Q(_log)  (Q(_t) _x);                                          \
Q(_t) Q(_log2) (Q(_t) _x);                                          \
Q(_t) Q(_log10)(Q(_t) _x);                                          \
Q(_t) Q(_log1p)(Q(_t) _x);                                          \
                                                                    \
/* power */                                                         \
Q(_t) Q(_sqrt) (Q(_t) _x);                                          \
Q(_t) Q(_cbrt) (Q(_t) _x);                                          \
Q(_t) Q(_hypot)(Q(_t) _x, Q(_t) _y);                                \
Q(_t) Q(_pow)  (Q(_t) _x, Q(_t) _y);                                \
                                                                    \
/* error and gamma functions */                                     \
Q(_t) Q(_erf)   (Q(_t) _z);                                         \
Q(_t) Q(_erfc)  (Q(_t) _z);                                         \
Q(_t) Q(_lgamma)(Q(_t) _z);                                         \
Q(_t) Q(_tgamma)(Q(_t) _z);                                         \
                                                                    \
/* mathematical constants */                                        \
extern const Q(_t) Q(_angle_scalar);/* qtype_pi/3.14159 */          \
extern const Q(_t) Q(_E);           /* e                */          \
extern const Q(_t) Q(_LOG2E);       /* log2(e)          */          \
extern const Q(_t) Q(_LOG10E);      /* log10(e)         */          \
extern const Q(_t) Q(_LN2);         /* log(2)           */          \
extern const Q(_t) Q(_LN10);        /* log(10)          */          \
extern const Q(_t) Q(_PI);          /* pi (true value)  */          \
extern const Q(_t) Q(_PI_2);        /* pi/2             */          \
extern const Q(_t) Q(_PI_4);        /* pi/4             */          \
extern const Q(_t) Q(_1_PI);        /* 1/pi             */          \
extern const Q(_t) Q(_2_PI);        /* 2/pi             */          \
extern const Q(_t) Q(_2_SQRTPI);    /* 2/sqrt(pi)       */          \
extern const Q(_t) Q(_SQRT2);       /* sqrt(2)          */          \
extern const Q(_t) Q(_SQRT1_2);     /* 1/sqrt(2)        */          \
                                                                    \
                                                                    \
                                                                    \
/* API definition macro (complex types) */                          \
typedef struct {Q(_t) real; Q(_t) imag;} CQ(_t);                    \
                                                                    \
/* conversion */                                                    \
liquidfpm_float_complex CQ(_fixed_to_float)(CQ(_t) _x);             \
CQ(_t) CQ(_float_to_fixed)(liquidfpm_float_complex _x);             \
                                                                    \
/* arithmetic */                                                    \
CQ(_t) CQ(_add)       (CQ(_t) _a, CQ(_t) _b);                       \
CQ(_t) CQ(_sub)       (CQ(_t) _a, CQ(_t) _b);                       \
CQ(_t) CQ(_mul)       (CQ(_t) _a, CQ(_t) _b);                       \
CQ(_t) CQ(_mul_scalar)(CQ(_t) _a,  Q(_t) _b);                       \
CQ(_t) CQ(_div)       (CQ(_t) _a, CQ(_t) _b);                       \
CQ(_t) CQ(_div_scalar)(CQ(_t) _a,  Q(_t) _b);                       \
CQ(_t) CQ(_inv)       (CQ(_t) _x); /* compute 1/_x */               \
                                                                    \
/* basic operations */                                              \
Q(_t) CQ(_cabs)(CQ(_t) _x);                                         \
Q(_t) CQ(_cabs2)(CQ(_t) _x);                                        \
static inline Q(_t) CQ(_carg)(CQ(_t) _x)                            \
    { return Q(_atan2)(_x.imag, _x.real); };                        \
static inline Q(_t) CQ(_real)(CQ(_t) _a) {return _a.real;};         \
static inline Q(_t) CQ(_imag)(CQ(_t) _a) {return _a.imag;};         \
CQ(_t) CQ(_conj) (CQ(_t) _a);                                       \
CQ(_t) CQ(_cproj)(CQ(_t) _a);                                       \
                                                                    \
/* exponential operations */                                        \
CQ(_t) CQ(_cexp) (CQ(_t) _x);                                       \
CQ(_t) CQ(_clog) (CQ(_t) _x);                                       \
CQ(_t) CQ(_csqrt)(CQ(_t) _x);                                       \
CQ(_t) CQ(_cpow) (CQ(_t) _b, CQ(_t) _x);                            \
                                                                    \
/* trigonometric operations */                                      \
CQ(_t) CQ(_csin) (CQ(_t) _x);                                       \
CQ(_t) CQ(_ccos) (CQ(_t) _x);                                       \
CQ(_t) CQ(_ctan) (CQ(_t) _x);                                       \
CQ(_t) CQ(_casin)(CQ(_t) _x);                                       \
CQ(_t) CQ(_cacos)(CQ(_t) _x);                                       \
CQ(_t) CQ(_catan)(CQ(_t) _x);                                       \
                                                                    \
/* hyperbolic operations */                                         \
CQ(_t) CQ(_csinh) (CQ(_t) _x);                                      \
CQ(_t) CQ(_ccosh) (CQ(_t) _x);                                      \
CQ(_t) CQ(_ctanh) (CQ(_t) _x);                                      \
CQ(_t) CQ(_casinh)(CQ(_t) _x);                                      \
CQ(_t) CQ(_cacosh)(CQ(_t) _x);                                      \
CQ(_t) CQ(_catanh)(CQ(_t) _x);                                      \
                                                                    \
/* liquid-fpm specific functions */                                 \
CQ(_t) CQ(_cexpj)(Q(_t) _theta);    /* exp(j*theta) */              \


//
// define q16 interfaces
//
LIQUIDFPM_DEFINE_API(LIQUIDFPM_MANGLE_Q16,
                     LIQUIDFPM_MANGLE_CQ16,
                     int16_t,
                     int32_t,
                     LIQUIDFPM_Q16_INTBITS,
                     LIQUIDFPM_Q16_FRACBITS)

//
// define q32 interfaces
//
LIQUIDFPM_DEFINE_API(LIQUIDFPM_MANGLE_Q32,
                     LIQUIDFPM_MANGLE_CQ32,
                     int32_t,
                     int64_t,
                     LIQUIDFPM_Q32_INTBITS,
                     LIQUIDFPM_Q32_FRACBITS)


#ifdef __cplusplus
}   /* extern "C" */
#endif /* __cplusplus */

#endif // __LIQUIDFPM_H__
