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

#ifndef M_PI
#  define M_PI 3.14159265358979
#endif

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
    return qtype_fixed_to_float(_x,_intbits,_fracbits) * (M_PI / (float)(1<<(_intbits-2)));
};

static inline int qtype64_angle_float_to_fixed(float _x,
                                               unsigned int _intbits,
                                               unsigned int _fracbits)
{
    return qtype_float_to_fixed(_x,_intbits,_fracbits) / (M_PI / (float)(1<<(_intbits-2)));
};

//
// bit-wise utilities
//

// leading zeros (8-bit)
extern const unsigned char liquidfpm_lz8[256];

// count leading zeros
#define clz8(_byte) (lz8[_byte])
unsigned int liquidfpm_clz(unsigned int _x);

// index of most significant bit
unsigned int liquidfpm_msb_index(unsigned int _x);

/* Internal API definition macro
 *
 * Q        :   name-mangling macro
 * T        :   primitive data type
 * INTBITS  :   number of integer bits
 * FRACBITS :   number of fractional bits
 */
#define LIQUIDFPM_DEFINE_INTERNAL_API(Q,T,INTBITS,FRACBITS)         \
                                                                    \
/* auto-generated look-up tables */                                 \
extern const Q(_t) Q(_sin_table)[256];                              \
                                                                    \
/* arithmetic */                                                    \
Q(_t) Q(_div_inv_newton)(Q(_t) _x, Q(_t) _y, unsigned int _n);      \
Q(_t) Q(_inv_newton)(Q(_t), unsigned int _n);                       \
Q(_t) Q(_inv_logexp)(Q(_t), unsigned int _n);                       \
Q(_t) Q(_sqrt_newton)(Q(_t), unsigned int _n);                      \
Q(_t) Q(_sqrt_logexp)(Q(_t), unsigned int _n);                      \
                                                                    \
/* trig */                                                          \
extern const Q(_t) Q(_cordic_Ak_tab)[32];                           \
extern const Q(_t) Q(_cordic_k_inv);                                \
extern const Q(_t) Q(_atan2_pwpoly_tab)[16][3];                     \
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
                                                                    \
/* log (old) */                                                     \
extern const Q(_t) Q(_log2_fraction_table)[256];                    \
                                                                    \
/* */                                                               \
extern const Q(_t) Q(_log2_shiftadd_Ak_tab)[32];                    \
extern const unsigned int Q(_log2_shiftadd_nmax);                   \
extern const Q(_t) Q(_ln2);     /* log(2)   */                      \
extern const Q(_t) Q(_log10_2); /* log10(2) */                      \
extern const Q(_t) Q(_log2_e);  /* log2(e) */                       \
extern const Q(_t) Q(_log2_10); /* log2(10) */                      \
                                                                    \
/* log2 shift|add */                                                \
Q(_t) Q(_log2_shiftadd)(Q(_t) _x, unsigned int _n);                 \
Q(_t) Q(_log2_shiftadd_base)(Q(_t) _x, unsigned int _n);            \
Q(_t) Q(_log_shiftadd)(Q(_t) _x, unsigned int _n);                  \
Q(_t) Q(_log10_shiftadd)(Q(_t) _x, unsigned int _n);                \
                                                                    \
/* exp2 shift|add */                                                \
Q(_t) Q(_exp2_shiftadd)(Q(_t) _x, unsigned int _n);                 \
Q(_t) Q(_exp2_shiftadd_base)(Q(_t) _x, unsigned int _n);            \
Q(_t) Q(_exp_shiftadd)(Q(_t) _x, unsigned int _n);                  \
Q(_t) Q(_exp10_shiftadd)(Q(_t) _x, unsigned int _n);                \
                                                                    \
Q(_t) Q(_pow_shiftadd)(Q(_t) _b, Q(_t) _x, unsigned int _n);        \
                                                                    \
/* hyperbolic trig */                                               \
extern const Q(_t) Q(_sinhcosh_cordic_Ak_tab)[32];                  \
extern const Q(_t) Q(_sinhcosh_cordic_kp_inv);                      \
void Q(_sinhcosh_cordic)(Q(_t) _x,                                  \
                         Q(_t) *_sinh,                              \
                         Q(_t) *_cosh,                              \
                         unsigned int _n);                          \
                                                                    \
/* math: transcendentals */                                         \
/* external constants (gentab/math.transcendentals.c) */            \
extern const Q(_t) Q(_log2pi);  /* log(2*pi)    */                  \
extern const Q(_t) Q(_inv_12);  /* 1/12         */                  \
extern const Q(_t) Q(_inv_pi);  /* 1/pi         */                  \
extern const Q(_t) Q(_pi2_6);   /* pi^2/6       */                  \
extern const Q(_t) Q(_pi4_120); /* pi^4/120     */                  \
Q(_t) Q(_loglogbesseli0)(Q(_t) _z);

LIQUIDFPM_DEFINE_INTERNAL_API(LIQUIDFPM_MANGLE_Q32, int32_t, 4, 28)

#endif // __LIQUID_FPM_INTERNAL_H__
