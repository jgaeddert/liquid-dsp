/*
 * Copyright (c) 2007, 2008, 2009, 2010 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010 Virginia Polytechnic
 *                                      Institute & State University
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
// modem_demodulate.c
//
// Definitions for linear demodulation of symbols.
//

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include "liquid.internal.h"

// generic demodulation
void modem_demodulate(modem _demod,
                      float complex x,
                      unsigned int *symbol_out)
{
    // invoke method specific to scheme (calculate symbol on the fly)
    _demod->demodulate_func(_demod, x, symbol_out);
}

// demodulate ASK
void modem_demodulate_ask(modem _demod,
                          float complex _x,
                          unsigned int * _symbol_out)
{
    // demodulate on linearly-spaced array
    unsigned int s;
    float res_i;
    modem_demodulate_linear_array_ref(crealf(_x), _demod->m, _demod->ref, &s, &res_i);

    // 'decode' output symbol (actually gray encoding)
    *_symbol_out = gray_encode(s);

    // re-modulate symbol and store state
    modem_modulate_ask(_demod, *_symbol_out, &_demod->x_hat);
    _demod->r = _x;
}

// demodulate QAM
void modem_demodulate_qam(modem _demod,
                          float complex _x,
                          unsigned int * _symbol_out)
{
    // demodulate in-phase component on linearly-spaced array
    unsigned int s_i;   // in-phase symbol
    float res_i;        // in-phase residual
    modem_demodulate_linear_array_ref(crealf(_x), _demod->m_i, _demod->ref, &s_i, &res_i);

    // demodulate quadrature component on linearly-spaced array
    unsigned int s_q;   // quadrature symbol
    float res_q;        // quadrature residual
    modem_demodulate_linear_array_ref(cimagf(_x), _demod->m_q, _demod->ref, &s_q, &res_q);

    // 'decode' output symbol (actually gray encoding)
    s_i = gray_encode(s_i);
    s_q = gray_encode(s_q);
    *_symbol_out = ( s_i << _demod->m_q ) + s_q;

    // re-modulate symbol (subtract residual) and store state
    _demod->x_hat = _x - (res_i + _Complex_I*res_q);
    _demod->r = _x;
}

// demodulate PSK
void modem_demodulate_psk(modem _demod,
                          float complex x,
                          unsigned int *_symbol_out)
{
    // compute angle and subtract phase offset, ensuring phase is in [-pi,pi)
    float theta = cargf(x);
    theta -= _demod->d_phi;
    if (theta < -M_PI)
        theta += 2*M_PI;

    // demodulate on linearly-spaced array
    unsigned int s;             // demodulated symbol
    float demod_phase_error;    // demodulation phase error
    modem_demodulate_linear_array_ref(theta, _demod->m, _demod->ref, &s, &demod_phase_error);

    // 'decode' output symbol (actually gray encoding)
    *_symbol_out = gray_encode(s);

    // re-modulate symbol and store state
    modem_modulate_psk(_demod, *_symbol_out, &_demod->x_hat);
    _demod->r = x;
}

// demodulate BPSK
void modem_demodulate_bpsk(modem _demod,
                           float complex _x,
                           unsigned int * _symbol_out)
{
    // slice directly to output symbol
    *_symbol_out = (crealf(_x) > 0 ) ? 0 : 1;

    // re-modulate symbol and store state
    modem_modulate_bpsk(_demod, *_symbol_out, &_demod->x_hat);
    _demod->r = _x;
}

// demodulate QPSK
void modem_demodulate_qpsk(modem _demod,
                           float complex _x,
                           unsigned int * _symbol_out)
{
    // slice directly to output symbol
    *_symbol_out  = (crealf(_x) > 0 ? 0 : 1) +
                    (cimagf(_x) > 0 ? 0 : 2);

    // re-modulate symbol and store state
    modem_modulate_qpsk(_demod, *_symbol_out, &_demod->x_hat);
    _demod->r = _x;
}

// demodulate OOK
void modem_demodulate_ook(modem _demod,
                          float complex _x,
                          unsigned int * _symbol_out)
{
    // slice directly to output symbol
    *_symbol_out = (crealf(_x) > M_SQRT1_2 ) ? 0 : 1;

    // re-modulate symbol and store state
    modem_modulate_ook(_demod, *_symbol_out, &_demod->x_hat);
    _demod->r = _x;
}

// demodulate 'square' 32-QAM
void modem_demodulate_sqam32(modem _q,
                             float complex _x,
                             unsigned int * _symbol_out)
{
    // determine quadrant and de-rotate to first quadrant
    // 10 | 00
    // ---+---
    // 11 | 01
    unsigned int quad = 2*(crealf(_x) < 0.0f) + (cimagf(_x) < 0.0f);
    
    float complex x_prime = _x;
    switch (quad) {
    case 0: x_prime = _x;           break;
    case 1: x_prime =  conjf(_x);   break;
    case 2: x_prime = -conjf(_x);   break;
    case 3: x_prime = -_x;          break;
    default:
        // should never get to this point
        fprintf(stderr,"error: modem_demodulate_sqam32(), logic error\n");
        exit(1);
    }
    //printf(" x = %12.8f +j*%12.8f, quad = %1u, r = %12.8f + j*%12.8f\n",
    //        crealf(_x), cimagf(_x), quad, crealf(r), cimagf(r));
    assert(crealf(x_prime) >= 0.0f);
    assert(cimagf(x_prime) >= 0.0f);

    // find symbol in map closest to x_prime
    float dmin = 0.0f;
    float d = 0.0f;
    float complex x_hat = 0.0f;
    unsigned int i;
    for (i=0; i<8; i++) {
        d = cabsf(x_prime - _q->symbol_map[i]);
        if (i==0 || d < dmin) {
            dmin = d;
            *_symbol_out = i;
            x_hat = _q->symbol_map[i];
        }
    }

    // add quadrant bits
    *_symbol_out |= (quad << 3);

    // re-modulate symbol and store state
    modem_modulate_sqam32(_q, *_symbol_out, &_q->x_hat);
    _q->r = _x;
}

// demodulate 'square' 128-QAM
void modem_demodulate_sqam128(modem _q,
                              float complex _x,
                              unsigned int * _symbol_out)
{
    // determine quadrant and de-rotate to first quadrant
    // 10 | 00
    // ---+---
    // 11 | 01
    unsigned int quad = 2*(crealf(_x) < 0.0f) + (cimagf(_x) < 0.0f);
    
    float complex x_prime = _x;
    switch (quad) {
    case 0: x_prime = _x;           break;
    case 1: x_prime =  conjf(_x);   break;
    case 2: x_prime = -conjf(_x);   break;
    case 3: x_prime = -_x;          break;
    default:
        // should never get to this point
        fprintf(stderr,"error: modem_demodulate_sqam128(), logic error\n");
        exit(1);
    }
    //printf(" x = %12.8f +j*%12.8f, quad = %1u, r = %12.8f + j*%12.8f\n",
    //        crealf(_x), cimagf(_x), quad, crealf(r), cimagf(r));
    assert(crealf(x_prime) >= 0.0f);
    assert(cimagf(x_prime) >= 0.0f);

    // find symbol in map closest to x_prime
    float dmin = 0.0f;
    float d = 0.0f;
    float complex x_hat = 0.0f;
    unsigned int i;
    for (i=0; i<32; i++) {
        d = cabsf(x_prime - _q->symbol_map[i]);
        if (i==0 || d < dmin) {
            dmin = d;
            *_symbol_out = i;
            x_hat = _q->symbol_map[i];
        }
    }

    // add quadrant bits
    *_symbol_out |= (quad << 5);

    // re-modulate symbol and store state
    modem_modulate_sqam128(_q, *_symbol_out, &_q->x_hat);
    _q->r = _x;
}

void modem_demodulate_dpsk(modem _demod,
                           float complex _x,
                           unsigned int * _symbol_out)
{
    // compute angle differencd
    float theta = cargf(_x);
    float d_theta = cargf(_x) - _demod->dpsk_phi;
    _demod->dpsk_phi = theta;

    // subtract phase offset, ensuring phase is in [-pi,pi)
    d_theta -= _demod->d_phi;
    if (d_theta > M_PI)
        d_theta -= 2*M_PI;
    else if (d_theta < -M_PI)
        d_theta += 2*M_PI;

    // demodulate on linearly-spaced array
    unsigned int s;             // demodulated symbol
    float demod_phase_error;    // demodulation phase error
    modem_demodulate_linear_array_ref(d_theta, _demod->m, _demod->ref, &s, &demod_phase_error);

    // 'decode' output symbol (actually gray encoding)
    *_symbol_out = gray_encode(s);

    // re-modulate symbol (accounting for differential rotation)
    // and store state
    _demod->x_hat = liquid_cexpjf(theta - demod_phase_error);
    _demod->r = _x;
}

// demodulate arbitrary modem type
void modem_demodulate_arb(modem _mod,
                          float complex _x,
                          unsigned int * _symbol_out)
{
    //printf("modem_demodulate_arb() invoked with I=%d, Q=%d\n", x);
    
    // search for symbol nearest to received sample
    unsigned int i;
    unsigned int s=0;
    float d;            // distance
    float d_min = 0.0f; // minimum distance

    for (i=0; i<_mod->M; i++) {
        // compute distance from received symbol to constellation point
        d = cabsf(_x - _mod->symbol_map[i]);

        // retain symbol with minimum distance
        if ( i==0 || d < d_min ) {
            d_min = d;
            s = i;
        }
    }

    // set output symbol
    *_symbol_out = s;

    // re-modulate symbol and store state
    modem_modulate_arb(_mod, *_symbol_out, &_mod->x_hat);
    _mod->r = _x;
}

// demodulate APSK
void modem_demodulate_apsk(modem _mod,
                           float complex _x,
                           unsigned int * _symbol_out)
{
    // compute amplitude
    float r = cabsf(_x);

    // determine which ring to demodulate with
    unsigned int i, p=0;
    for (i=0; i<_mod->apsk_num_levels-1; i++) {
        if (r < _mod->apsk_r_slicer[i]) {
            p = i;
            break;
        } else {
            p = _mod->apsk_num_levels-1;
        }
    }

    // find closest point in ring
    float theta = cargf(_x);
    if (theta < 0.0f) theta += 2.0f*M_PI;
    float dphi = 2.0f*M_PI / (float) _mod->apsk_p[p];
    unsigned int s_hat=0;
    float i_hat = (theta - _mod->apsk_phi[p]) / dphi;
    s_hat = roundf(i_hat);      // compute symbol (closest angle)
    s_hat %= _mod->apsk_p[p];   // ensure symbol is in range
    //printf("          i_hat : %12.8f (%3u)\n", i_hat, s_hat);

    // accumulate symbol points
    for (i=0; i<p; i++)
        s_hat += _mod->apsk_p[i];
    //assert(s_hat < _mod->M);

    // reverse symbol mapping
    unsigned int s_prime=0;
    for (i=0; i<_mod->M; i++) {
        if ( _mod->apsk_symbol_map[i] == s_hat) {
            s_prime = i;
            break;
        }
    }

#if 0
    printf("              x : %12.8f + j*%12.8f\n", crealf(_x), cimagf(_x));
    printf("              p : %3u\n", p);
    printf("          theta : %12.8f\n", theta);
    printf("           dmin : %12.8f\n", dmin);
    printf("              s : %3u > %3u\n", s_hat, s_prime);
#endif

    *_symbol_out = s_prime;

    // re-modulate symbol and store state
    modem_modulate(_mod, s_prime, &_mod->x_hat);
    _mod->r = _x;
}

// get demodulator's estimated transmit sample
void modem_get_demodulator_sample(modem _demod,
                                  float complex * _x_hat)
{
    *_x_hat = _demod->x_hat;
}

// get demodulator phase error
float modem_get_demodulator_phase_error(modem _demod)
{
    return cimagf(_demod->r*conjf(_demod->x_hat));
}

// get error vector magnitude
float modem_get_demodulator_evm(modem _demod)
{
    return cabsf(_demod->x_hat - _demod->r);
}


// Demodulate a linear symbol constellation using dynamic threshold calculation
//  _v      :   input value
//  _m      :   bits per symbol
//  _alpha  :   scaling factor
//  _s      :   demodulated symbol
//  _res    :   residual
void modem_demodulate_linear_array(float _v,
                                   unsigned int _m,
                                   float _alpha,
                                   unsigned int *_s,
                                   float *_res)
{
    unsigned int s=0;
    unsigned int i, k = _m;
    float ref=0.0f;
    for (i=0; i<_m; i++) {
        s <<= 1;
        s |= (_v > 0) ? 1 : 0;
        ref = _alpha * (1<<(k-1));
        _v += (_v < 0) ? ref : -ref;
        k--;
    }
    *_s = s;
    *_res = _v;
}

// Demodulate a linear symbol constellation using refereneced lookup table
//  _v      :   input value
//  _m      :   bits per symbol
//  _ref    :   array of thresholds
//  _s      :   demodulated symbol
//  _res    :   residual
void modem_demodulate_linear_array_ref(float _v,
                                       unsigned int _m,
                                       float *_ref,
                                       unsigned int *_s,
                                       float *_res)
{
    // initialize loop counter
    register unsigned int i;

    // initialize demodulated symbol
    register unsigned int s=0;

    for (i=0; i<_m; i++) {
        // prepare symbol for next demodulated bit
        s <<= 1;

        // compare received value to zero
        if ( _v > 0 ) {
            // shift '1' into symbol, subtract reference
            s |= 1;
            _v -= _ref[_m-i-1];
        } else {
            // shift '0' into symbol, add reference
            s |= 0;
            _v += _ref[_m-i-1];
        }
    }
    // return demodulated symbol
    *_s = s;

    // return residual
    *_res = _v;
}


