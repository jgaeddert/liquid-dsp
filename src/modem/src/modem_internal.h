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
// Internal modem header file
//

#ifndef __MODEM_INTERNAL_H__
#define __MODEM_INTERNAL_H__

#include <complex.h>
#undef I
#define J _Complex_I

#include "liquid.h"

// PSK
#define PSK_ALPHA       1

// 'Square' QAM
#define QAM4_ALPHA      1/sqrt(2)
#define QAM8_ALPHA      1/sqrt(6)
#define QAM16_ALPHA     1/sqrt(10)
#define QAM32_ALPHA     1/sqrt(20)
#define QAM64_ALPHA     1/sqrt(42)
#define QAM128_ALPHA    1/sqrt(82)
#define QAM256_ALPHA    1/sqrt(170)
#define QAM1024_ALPHA   1/sqrt(682)
#define QAM4096_ALPHA   1/sqrt(2730)

// Rectangular QAM
#define RQAM4_ALPHA     QAM4_ALPHA
#define RQAM8_ALPHA     QAM8_ALPHA
#define RQAM16_ALPHA    QAM16_ALPHA
#define RQAM32_ALPHA    1/sqrt(26)
#define RQAM64_ALPHA    QAM64_ALPHA
#define RQAM128_ALPHA   1/sqrt(106)
#define RQAM256_ALPHA   QAM256_ALPHA
#define RQAM512_ALPHA   1/sqrt(426)
#define RQAM1024_ALPHA  QAM1024_ALPHA
#define RQAM2048_ALPHA  1/sqrt(1706)
#define RQAM4096_ALPHA  QAM4096_ALPHA

// ASK
#define ASK2_ALPHA      1
#define ASK4_ALPHA      1/sqrt(5)
#define ASK8_ALPHA      1/sqrt(21)
#define ASK16_ALPHA     1/sqrt(85)
#define ASK32_ALPHA     1/sqrt(341)

/** \brief modem structure used for both modulation and demodulation 
 *
 * The modem structure implements a variety of common modulation schemes,
 * including (differential) phase-shift keying, and (quadrature) amplitude
 * modulation.
 *
 * While the same modem structure may be used for both modulation and
 * demodulation for most schemes, it is important to use separate objects
 * for differential-mode modems (e.g. MOD_DPSK) as the internal state
 * will change after each symbol.  It is usually good practice to keep
 * separate instances of modulators and demodulators.
 */
struct modem_s {
    modulation_scheme scheme;

    unsigned int m;     ///< bits per symbol
    unsigned int M;     ///< total symbols, \f$M=2^m\f$

    unsigned int m_i;   ///< bits per symbol, in-phase
    unsigned int M_i;   ///< total symbols, in-phase, \f$M_i=2^{m_i}\f$
    unsigned int m_q;   ///< bits per symbol, quadrature
    unsigned int M_q;   ///< total symbols, quadrature, \f$M_q=2^{m_q}\f$

    float alpha;        ///< scaling factor to ensure \f$E\{|\bar{r}|^2\}=1\f$

    /** \brief Reference vector for demodulating linear arrays
     *
     * By storing these values in an array they do not need to be
     * calculated during run-time.  This speeds up the demodulation by
     * approximately 8%.
     */
    float ref[MAX_MOD_BITS_PER_SYMBOL];

    /// Complete symbol map
    float complex * symbol_map;

    float complex state;      ///< received state vector
    float state_theta;  ///< received state vector, angle

    float complex res;        ///< residual error vector

    float phase_error;  ///< phase error after demodulation
    float evm;          ///< error vector magnitude (EVM)

    float d_phi;

    // modulate function pointer
    void (*modulate_func)(modem _mod, unsigned int symbol_in, float complex *y);

    // demodulate function pointer
    void (*demodulate_func)(modem _demod, float complex x, unsigned int *symbol_out);
};
/**
  Here is a full modem example:
  \example modem_test_qam16.c
  */


// generic modem create routines
modem modem_create_ask(unsigned int _bits_per_symbol);
modem modem_create_qam(unsigned int _bits_per_symbol);
modem modem_create_psk(unsigned int _bits_per_symbol);
modem modem_create_dpsk(unsigned int _bits_per_symbol);
modem modem_create_arb(unsigned int _bits_per_symbol);
modem modem_create_arb_mirrored(unsigned int _bits_per_symbol);
modem modem_create_arb_rotated(unsigned int _bits_per_symbol);

// specific modem create routines
modem modem_create_bpsk(void);
modem modem_create_qpsk(void);

/// Scale arbitrary modem energy to unity
void modem_arb_scale(modem _mod);

/// Balance I/Q
void modem_arb_balance_iq(modem _mod);

// generic modem modulate routines
void modem_modulate_ask(modem _mod, unsigned int symbol_in, float complex *y);
void modem_modulate_qam(modem _mod, unsigned int symbol_in, float complex *y);
void modem_modulate_psk(modem _mod, unsigned int symbol_in, float complex *y);
void modem_modulate_dpsk(modem _mod, unsigned int symbol_in, float complex *y);
void modem_modulate_arb(modem _mod, unsigned int symbol_in, float complex *y);
//void modem_modulate_arb_mirrored(modem _mod, unsigned int symbol_in, float complex *y);
//void modem_modulate_arb_rotated(modem _mod, unsigned int symbol_in, float complex *y);

// specific modem modulate routines
void modem_modulate_bpsk(modem _mod, unsigned int symbol_in, float complex *y);
void modem_modulate_qpsk(modem _mod, unsigned int symbol_in, float complex *y);

// generic modem demodulate routines
void modem_demodulate_ask(modem _demod, float complex x, unsigned int *symbol_out);
void modem_demodulate_qam(modem _demod, float complex x, unsigned int *symbol_out);
void modem_demodulate_psk(modem _demod, float complex x, unsigned int *symbol_out);
void modem_demodulate_dpsk(modem _demod, float complex x, unsigned int *symbol_out);
void modem_demodulate_arb(modem _demod, float complex x, unsigned int *symbol_out);
//void modem_demodulate_arb_mirrored(modem _demod, float complex x, unsigned int *symbol_out);
//void modem_demodulate_arb_rotated(modem _demod, float complex x, unsigned int *symbol_out);

// specific modem demodulate routines
void modem_demodulate_bpsk(modem _demod, float complex x, unsigned int *symbol_out);
void modem_demodulate_qpsk(modem _demod, float complex x, unsigned int *symbol_out);

// get demodulator phase error
//void get_demodulator_phase_error(modem _demod, float* _phi);

// get error vector magnitude
//void get_demodulator_evm(modem _demod, float* _evm);

// demodulator helper functions

/** \brief Demodulates a linear symbol constellation using dynamic threshold calculation
 *
 * \param[in]   _v      value
 * \param[in]   _m      bits per symbol
 * \param[in]   _alpha  scaling factor
 * \param[out]  _s      demodulated symbol
 * \param[out]  _res    residual
 */
void modem_demodulate_linear_array(
    float _v,
    unsigned int _m,
    float _alpha,
    unsigned int *_s,
    float *_res);

/** \brief Demodulates a linear symbol constellation using refereneced lookup table
 *
 * \param[in]   _v      value
 * \param[in]   _m      bits per symbol
 * \param[in]   _ref    array of thresholds
 * \param[out]  _s      demodulated symbol
 * \param[out]  _res    residual
 */
void modem_demodulate_linear_array_ref(
    float _v,
    unsigned int _m,
    float *_ref,
    unsigned int *_s,
    float *_res);

#endif // __MODEM_INTERNAL_H__

