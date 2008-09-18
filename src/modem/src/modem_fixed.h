//
// Fixed-point modem
//

#ifndef __LIQUID_MODEM_FIXED_H__
#define __LIQUID_MODEM_FIXED_H__

#include <assert.h>
#include <limits.h>         // for UINT_MAX, etc.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#include "modem_common.h"
#include "libmad_fixed.h"
#include "trig_fixed.h"     // for liquid_atan2_q32, liquid_cabs_q32

/** \brief modem structure used for both modulation and demodulation, fixed-point (Q4.28)
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
typedef struct {
    modulation_scheme scheme;

    unsigned int m;     ///< bits per symbol
    unsigned int M;     ///< total symbols, \f$M=2^m\f$

    unsigned int m_i;   ///< bits per symbol, in-phase
    unsigned int M_i;   ///< total symbols, in-phase, \f$M_i=2^{m_i}\f$
    unsigned int m_q;   ///< bits per symbol, quadrature
    unsigned int M_q;   ///< total symbols, quadrature, \f$M_q=2^{m_q}\f$

    q32_t alpha;        ///< scaling factor to ensure \f$E\{|\bar{r}|^2\}=1\f$

    /** \brief Reference vector for demodulating linear arrays
     *
     * By storing these values in an array they do not need to be
     * calculated during run-time.  This speeds up the demodulation by
     * approximately 8%.
     */
    q32_t ref[MAX_MOD_BITS_PER_SYMBOL];

    q32_t * levels_i;
    q32_t * levels_q;
    unsigned int num_levels;

    q32_t state_i;      ///< received state vector, in-phase
    q32_t state_q;      ///< received state vector, quadrature
    q32_t state_theta;  ///< received state vector, angle

    q32_t res_i;        ///< residual error vector, in-phase
    q32_t res_q;        ///< residual error vector, quadrature

    q32_t phase_error;  ///< phase error after demodulation
    q32_t evm;          ///< error vector magnitude (EVM)

    q32_t d_phi;


} modem_q32;


/// create modulation scheme, allocating memory as necessary
modem_q32* modem_create_q32(modulation_scheme, unsigned int _bits_per_symbol);
void modem_init_q32(modem_q32* _mod, unsigned int _bits_per_symbol);

void free_modem_q32(modem_q32 * _mod);

// generic modem create routines
modem_q32* modem_create_pam_q32(unsigned int _bits_per_symbol);
modem_q32* modem_create_qam_q32(unsigned int _bits_per_symbol);
modem_q32* modem_create_psk_q32(unsigned int _bits_per_symbol);
modem_q32* modem_create_dpsk_q32(unsigned int _bits_per_symbol);
modem_q32* modem_create_arb_q32(unsigned int _bits_per_symbol);
modem_q32* modem_create_arb_mirrored_q32(unsigned int _bits_per_symbol);
modem_q32* modem_create_arb_rotated_q32(unsigned int _bits_per_symbol);

// specific modem create routines
modem_q32* modem_create_bpsk_q32();
modem_q32* modem_create_qpsk_q32();

/// initialize modem on array
void modem_arb_init_q32(modem_q32 *_mod, q32_t* _I, q32_t* _Q, unsigned int _len);

/// initialize modem on array on external file
void modem_arb_init_file_q32(modem_q32 *_mod, char* filename);

/// Scale modem to unit energy
void modem_arb_scale_q32(modem_q32 *_mod);

/// Balance I/Q signals
void modem_arb_balance_iq_q32(modem_q32 *_mod);

/// generic modulate function; simply queries modem scheme and calls
/// appropriate subroutine
void modulate_q32(modem_q32* _mod, unsigned int symbol_in, q32_t *I_out, q32_t *Q_out);

// generic modem modulate routines
void modulate_pam_q32(modem_q32* _mod, unsigned int symbol_in, q32_t *I_out, q32_t *Q_out);
void modulate_qam_q32(modem_q32* _mod, unsigned int symbol_in, q32_t *I_out, q32_t *Q_out);
void modulate_psk_q32(modem_q32* _mod, unsigned int symbol_in, q32_t *I_out, q32_t *Q_out);
void modulate_dpsk_q32(modem_q32* _mod, unsigned int symbol_in, q32_t *I_out, q32_t *Q_out);
void modulate_arb_q32(modem_q32 *_mod, unsigned int symbol_in, q32_t *I_out, q32_t *Q_out);
void modulate_arb_mirrored_q32(modem_q32 *_mod, unsigned int symbol_in, q32_t *I_out, q32_t *Q_out);
void modulate_arb_rotated_q32(modem_q32 *_mod, unsigned int symbol_in, q32_t *I_out, q32_t *Q_out);

// generic modem modulate routines
void modulate_bpsk_q32(modem_q32* _mod, unsigned int symbol_in, q32_t *I_out, q32_t *Q_out);
void modulate_qpsk_q32(modem_q32* _mod, unsigned int symbol_in, q32_t *I_out, q32_t *Q_out);

void demodulate_q32(modem_q32* _demod, q32_t I_in, q32_t Q_in, unsigned int *symbol_out);

// generic modem demodulate routines
void demodulate_pam_q32(modem_q32* _demod, q32_t I_in, q32_t Q_in, unsigned int *symbol_out);
void demodulate_qam_q32(modem_q32* _demod, q32_t I_in, q32_t Q_in, unsigned int *symbol_out);
void demodulate_psk_q32(modem_q32* _demod, q32_t I_in, q32_t Q_in, unsigned int *symbol_out);
void demodulate_dpsk_q32(modem_q32* _demod, q32_t I_in, q32_t Q_in, unsigned int *symbol_out);
void demodulate_arb_q32(modem_q32* _demod, q32_t I_in, q32_t Q_in, unsigned int *symbol_out);
void demodulate_arb_mirrored_q32(modem_q32* _demod, q32_t I_in, q32_t Q_in, unsigned int *symbol_out);
void demodulate_arb_rotated_q32(modem_q32* _demod, q32_t I_in, q32_t Q_in, unsigned int *symbol_out);

// specific modem demodulate routines
void demodulate_bpsk_q32(modem_q32* _demod, q32_t I_in, q32_t Q_in, unsigned int *symbol_out);
void demodulate_qpsk_q32(modem_q32* _demod, q32_t I_in, q32_t Q_in, unsigned int *symbol_out);

/// get demodulator phase error
void get_demodulator_phase_error_q32(modem_q32* _demod, q32_t* _phi);

/// get error vector magnitude
void get_demodulator_evm_q32(modem_q32* _demod, q32_t* _evm);

// demodulator helper functions

/** \brief Demodulates a linear symbol constellation using refereneced lookup table
 *
 * \param[in]   _v      value
 * \param[in]   _m      bits per symbol
 * \param[in]   _ref    array of thresholds
 * \param[out]  _s      demodulated symbol
 * \param[out]  _res    residual
 */
void demodulate_linear_array_ref_q32(
    q32_t _v,
    unsigned int _m,
    q32_t *_ref,
    unsigned int *_s,
    q32_t *_res);

#endif // __LIQUID_MODEM_FIXED_H__


