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
// Numerically-controlled oscillator, fixed-point precision
//

#ifndef __LIQUID_NCO_FIXED_H__
#define __LIQUID_NCO_FIXED_H__

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "libmad_fixed.h"
#include "trig_fixed.h"   // for sine lookup table

/*! \brief Constrain fixed-point nco phase to be within \f$(-\pi,\pi)\f$?
 *
 * Depending on the way processors handle integer math overflow, there
 * might be no need to constrain the internal phase of the nco.  Because
 * liquid maps \f$2\pi\f$ to LONG_MAX and \f$-2\pi\f$ to LONG_MIN the
 * natural integer overflow wraps the phase around.
 *
 * See: http://www.gnu.org/software/libtool/manual/autoconf/Integer-Overflow.html
 */
#define NCO_FIXED_CONSTRAIN_PHASE 0

/*! \brief Numerically-controlled oscillator, fixed-point (Q4.28)
 *
 * This implementation of the numerically-controlled oscillator (NCO) uses
 * an integer representation of both frequency and phase in conjunction with
 * the sine lookup table for efficient processing.  There is minimal tradeoff
 * with accuracy for most applications.
 */
typedef struct {
    q32_t theta;        ///< nco phase, \f$\theta\f$
    q32_t d_theta;      ///< nco frequency, \f$f=\Delta\theta\f$
} nco_q32;

/// Intialize fixed-point nco; reset phase state, frequency to zero
void nco_init_q32(nco_q32 *_nco);

static inline void nco_set_frequency_q32(nco_q32 *_nco, q32_t _f) {
    _nco->d_theta = _f;
}

static inline void nco_adjust_frequency_q32(nco_q32 *_nco, q32_t _df) {
    _nco->d_theta += _df;
}

void nco_set_phase_q32(nco_q32 *_nco, q32_t _phi);
void nco_adjust_phase_q32(nco_q32 *_nco, q32_t _dphi);

/// Increment internal phase state
void nco_step_q32(nco_q32 *_nco);

/// Constrain internal phase state to be in \f$(-\pi,\pi)\f$
void nco_constrain_phase_q32(nco_q32 *_nco);

// sin, cos functions
static inline q32_t nco_sin_q32(nco_q32 *_nco) {
    return sin_q32( _nco->theta );
}

static inline q32_t nco_cos_q32(nco_q32 *_nco) {
    return cos_q32( _nco->theta );
}

static inline void
nco_sincos_q32(nco_q32 *_nco, q32_t* _s, q32_t* _c) {
    sincos_q32( _nco->theta, _s, _c );
}

// mixing functions

/// Rotate Q4.28 input vector up by NCO angle, \f$\vec{y} = \vec{x}e^{j\theta}\f$
static inline void nco_mix_up_q32(
    nco_q32 *_nco,
    q32_t _xi,
    q32_t _xq,
    q32_t *_yi,
    q32_t *_yq)
{
    rotate_q32(_xi, _xq, _nco->theta, _yi, _yq);
}

/// Rotate Q4.28 input vector down by NCO angle, \f$\vec{y} = \vec{x}e^{-j\theta}\f$
static inline void nco_mix_down_q32(
    nco_q32 *_nco,
    q32_t _xi,
    q32_t _xq,
    q32_t *_yi,
    q32_t *_yq)
{
    rotate_q32(_xi, _xq, -(_nco->theta), _yi, _yq);
}

/// Rotate Q4.28 input vector array up by NCO angle, \f$\vec{y} = \vec{x}e^{j\theta}\f$
void nco_mix_block_up_q32(
    nco_q32 *_nco,
    q32_t *_xi,
    q32_t *_xq,
    unsigned int _N,
    q32_t *_yi,
    q32_t *_yq);

#endif /* __LIQUID_NCO_FIXED_H__ */

