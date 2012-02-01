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
 * T        :   primitive data type
 * TA       :   primitive data type (accumulator)
 * INTBITS  :   number of integer bits
 * FRACBITS :   number of fractional bits
 */
#define LIQUIDFPM_DEFINE_INTERNAL_API(Q,T,TA,INTBITS,FRACBITS)      \
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
                                                                    \
/* hyperbolic */                                                    \
                                                                    \
/* exponential and logarithmic */                                   \
extern const Q(_t) Q(_log2_frac_gentab)[256];                       \
Q(_t) Q(_log2_frac) (Q(_t) _x);                                     \
                                                                    \
/* power */                                                         \
                                                                    \
/* error and gamma functions */                                     \

LIQUIDFPM_DEFINE_INTERNAL_API(LIQUIDFPM_MANGLE_Q16, int16_t, int32_t, LIQUIDFPM_Q16_INTBITS, LIQUIDFPM_Q16_FRACBITS)
LIQUIDFPM_DEFINE_INTERNAL_API(LIQUIDFPM_MANGLE_Q32, int32_t, int64_t, LIQUIDFPM_Q32_INTBITS, LIQUIDFPM_Q32_FRACBITS)

#endif // __LIQUID_FPM_INTERNAL_H__
