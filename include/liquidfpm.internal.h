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

#ifndef __LIQUID_FPM_INTERNAL_H__
#define __LIQUID_FPM_INTERNAL_H__

#include "liquidfpm.h"
#include "config.h"

/* Internal API definition macro
 *
 * Q        :   name-mangling macro
 * CQ       :   name-mangling macro (complex)
 * T        :   primitive data type
 * TA       :   primitive data type (accumulator)
 * INTBITS  :   number of integer bits
 * FRACBITS :   number of fractional bits
 */
#define LIQUIDFPM_DEFINE_INTERNAL_API(Q,CQ,T,TA,INTBITS,FRACBITS)   \
                                                                    \
/* arithmetic */                                                    \
Q(_t) Q(_inv_newton)(Q(_t) _x, unsigned int _precision);            \
                                                                    \
/* trig */                                                          \
extern const Q(_t) Q(_cordic_Ak_tab)[];                             \
extern const Q(_t) Q(_cordic_k_inv);                                \
unsigned int Q(_quadrant_cordic)(Q(_t) _theta);                     \
Q(_t) Q(_sin_cordic)(Q(_t) _theta, unsigned int _n);                \
Q(_t) Q(_cos_cordic)(Q(_t) _theta, unsigned int _n);                \
void Q(_sincos_cordic)(Q(_t) _theta,                                \
                       Q(_t) * _sin,                                \
                       Q(_t) * _cos,                                \
                       unsigned int _n);                            \
void Q(_sincos_cordic_base)(Q(_t) _theta,                           \
                            Q(_t) * _sin,                           \
                            Q(_t) * _cos,                           \
                            unsigned int _n);                       \
void Q(_atan2_cordic)(Q(_t) _y,                                     \
                      Q(_t) _x,                                     \
                      Q(_t) * _r,                                   \
                      Q(_t) * _theta,                               \
                      unsigned int _n);                             \
void Q(_atan2_cordic_base)(Q(_t) _y,                                \
                           Q(_t) _x,                                \
                           Q(_t) * _r,                              \
                           Q(_t) * _theta,                          \
                           unsigned int _n);                        \
extern const Q(_t) Q(_sintab)[256];                                 \
Q(_t) Q(_sin_tab)(Q(_t) _theta);                                    \
Q(_t) Q(_cos_tab)(Q(_t) _theta);                                    \
void Q(_sincos_tab)(Q(_t) _theta,                                   \
                    Q(_t) * _sin,                                   \
                    Q(_t) * _cos);                                  \
extern const Q(_t) Q(_atan2tab)[256];                               \
Q(_t) Q(_atan2_frac)(Q(_t) _y, Q(_t) _x);                           \
                                                                    \
/* hyperbolic */                                                    \
                                                                    \
/* exponential and logarithmic */                                   \
extern const Q(_t) Q(_ln2);     /* log(2)   */                      \
extern const Q(_t) Q(_log10_2); /* log10(2) */                      \
extern const Q(_t) Q(_log2_e);  /* log2(e) */                       \
extern const Q(_t) Q(_log2_10); /* log2(10) */                      \
                                                                    \
extern const Q(_t) Q(_log2_frac_gentab)[256];                       \
Q(_t) Q(_log2_frac) (Q(_t) _x);                                     \
Q(_t) Q(_log_frac)  (Q(_t) _x);                                     \
Q(_t) Q(_log10_frac)(Q(_t) _x);                                     \
                                                                    \
extern const Q(_t) Q(_exp2_frac_gentab)[256];                       \
Q(_t) Q(_exp2_frac) (Q(_t) _x);                                     \
Q(_t) Q(_exp_frac)  (Q(_t) _x);                                     \
Q(_t) Q(_exp10_frac)(Q(_t) _x);                                     \
                                                                    \
extern const Q(_t) Q(_log2_shiftadd_Ak_tab)[];                      \
extern const unsigned int Q(_log2_shiftadd_nmax);                   \
Q(_t) Q(_log2_shiftadd)     (Q(_t) _x, unsigned int _n);            \
Q(_t) Q(_log2_shiftadd_base)(Q(_t) _x, unsigned int _n);            \
Q(_t) Q(_log_shiftadd)      (Q(_t) _x, unsigned int _n);            \
Q(_t) Q(_log10_shiftadd)    (Q(_t) _x, unsigned int _n);            \
                                                                    \
Q(_t) Q(_exp2_shiftadd)     (Q(_t) _x, unsigned int _n);            \
Q(_t) Q(_exp2_shiftadd_base)(Q(_t) _x, unsigned int _n);            \
Q(_t) Q(_exp_shiftadd)      (Q(_t) _x, unsigned int _n);            \
Q(_t) Q(_exp10_shiftadd)    (Q(_t) _x, unsigned int _n);            \
                                                                    \
                                                                    \
/* power */                                                         \
Q(_t) Q(_sqrt_newton) (Q(_t) _x, unsigned int _precision);          \
Q(_t) Q(_sqrt_logexp_frac)     (Q(_t) _x);                          \
Q(_t) Q(_sqrt_logexp_shiftadd) (Q(_t) _x, unsigned int _precision); \
Q(_t) Q(_cbrt_newton) (Q(_t) _x, unsigned int _precision);          \
Q(_t) Q(_hypot_cordic)(Q(_t) _x, Q(_t) _y, unsigned int _prec);     \
Q(_t) Q(_pow_frac)    (Q(_t) _b, Q(_t) _x);                         \
Q(_t) Q(_pow_shiftadd)(Q(_t) _b, Q(_t) _x, unsigned int _prec);     \
                                                                    \
/* error and gamma functions */                                     \

//
// q16
//
LIQUIDFPM_DEFINE_INTERNAL_API(LIQUIDFPM_MANGLE_Q16,
                              LIQUIDFPM_MANGLE_CQ16,
                              int16_t,
                              int32_t,
                              LIQUIDFPM_Q16_INTBITS,
                              LIQUIDFPM_Q16_FRACBITS)

//
// q32
//
LIQUIDFPM_DEFINE_INTERNAL_API(LIQUIDFPM_MANGLE_Q32,
                              LIQUIDFPM_MANGLE_CQ32,
                              int32_t,
                              int64_t,
                              LIQUIDFPM_Q32_INTBITS,
                              LIQUIDFPM_Q32_FRACBITS)

// generic conversion
static inline float qtype_fixed_to_float(int _x,
                                         unsigned int _intbits,
                                         unsigned int _fracbits)
{
    return (float) (_x) / (float)(1 << _fracbits);
};

static inline int qtype_float_to_fixed(float _x,
                                       unsigned int _intbits,
                                       unsigned int _fracbits)
{
    return (int) (_x * (float)(1 << _fracbits) + 0.5f);
};

static inline float qtype_angle_fixed_to_float(int _x,
                                               unsigned int _intbits,
                                               unsigned int _fracbits)
{
    return qtype_fixed_to_float(_x,_intbits,_fracbits) * (3.14159265358979 / (float)(1<<(_intbits-2)));
};

static inline int qtype_angle_float_to_fixed(float _x,
                                             unsigned int _intbits,
                                             unsigned int _fracbits)
{
    return qtype_float_to_fixed(_x,_intbits,_fracbits) / (3.14159265358979 / (float)(1<<(_intbits-2)));
};


#endif // __LIQUID_FPM_INTERNAL_H__
