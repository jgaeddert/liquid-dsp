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
//
//

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include "liquid.internal.h"

#define MODEM_UNIFIED_DEMODULATE 0

// approximation to cargf() but faster
float cargf_demod_approx(float complex _x)
{
    float theta;
    float xi = crealf(_x);
    float xq = cimagf(_x);

    if (xi == 0.0f) {
        if (xq == 0.0f)
            return 0.0f;
        return xq > 0.0f ? M_PI_2 : -M_PI_2;
    } else {
        theta = xq / fabsf(xi);
    }

    if (theta >  M_PI_2)
        theta =  M_PI_2;
    else if (theta < -M_PI_2)
        theta = -M_PI_2;
    return theta;
}

// generic demodulation
void modem_demodulate(modem _demod,
                      float complex x,
                      unsigned int *symbol_out)
{
    _demod->demodulate_func(_demod, x, symbol_out);
}

// demodulate ASK
void modem_demodulate_ask(modem _demod,
                          float complex _x,
                          unsigned int * _symbol_out)
{
    unsigned int s;
    _demod->state = _x;
    float res_i;
    modem_demodulate_linear_array_ref(crealf(_x), _demod->m, _demod->ref, &s, &res_i);
    _demod->res = res_i + _Complex_I*cimagf(_x);
    *_symbol_out = gray_encode(s);

#if MODEM_UNIFIED_DEMODULATE==0
    // compute residuals
    float complex x_hat = _x + _demod->res;
    //_demod->phase_error = cabsf(x_hat)*cargf(x_hat*conjf(_x));
    _demod->phase_error = cargf_demod_approx(x_hat*conjf(_x));
    _demod->evm = cabsf(_demod->res);
#else
    // re-modulate symbol
    modem_modulate_ask(_demod, *_symbol_out, &_demod->x_hat);
#endif
}

// demodulate QAM
void modem_demodulate_qam(modem _demod,
                          float complex _x,
                          unsigned int * _symbol_out)
{
    unsigned int s_i, s_q;
    _demod->state = _x;
    float res_i, res_q;
    modem_demodulate_linear_array_ref(crealf(_x), _demod->m_i, _demod->ref, &s_i, &res_i);
    modem_demodulate_linear_array_ref(cimagf(_x), _demod->m_q, _demod->ref, &s_q, &res_q);
    _demod->res = res_i + _Complex_I*res_q;
    s_i = gray_encode(s_i);
    s_q = gray_encode(s_q);
    *_symbol_out = ( s_i << _demod->m_q ) + s_q;

#if MODEM_UNIFIED_DEMODULATE==0
    // compute residuals
    float complex x_hat = _x + _demod->res;
    //_demod->phase_error = cabsf(x_hat)*cargf(x_hat*conjf(_x));
    //_demod->phase_error = cimagf(x_hat*conjf(_x));
    _demod->phase_error = cargf_demod_approx(x_hat*conjf(_x));
    _demod->evm = cabsf(_demod->res);
#else
    // re-modulate symbol
    modem_modulate_qam(_demod, *_symbol_out, &_demod->x_hat);
#endif
}

// demodulate PSK
void modem_demodulate_psk(modem _demod,
                          float complex x,
                          unsigned int *_symbol_out)
{
    unsigned int s;
    float theta = cargf(x);
    _demod->state = x;
    _demod->state_theta = theta;

    // subtract phase offset, ensuring phase is in [-pi,pi)
    theta -= _demod->d_phi;
    if (theta < -M_PI)
        theta += 2*M_PI;

    modem_demodulate_linear_array_ref(theta, _demod->m, _demod->ref, &s, &(_demod->phase_error));
    *_symbol_out = gray_encode(s);

#if MODEM_UNIFIED_DEMODULATE==0
    // compute residuals
    // phase error computed as residual from demodulator
#else
    // re-modulate symbol
    modem_modulate_psk(_demod, *_symbol_out, &_demod->x_hat);
#endif
}

// demodulate BPSK
void modem_demodulate_bpsk(modem _demod,
                           float complex _x,
                           unsigned int * _symbol_out)
{
    *_symbol_out = (crealf(_x) > 0 ) ? 0 : 1;
    _demod->state = _x;

    // compute residuals
    float complex x_hat;
    modem_modulate_bpsk(_demod, *_symbol_out, &x_hat);
    _demod->res = x_hat - _x;
#if MODEM_UNIFIED_DEMODULATE==0
    _demod->evm = cabsf(_demod->res);
    //_demod->phase_error = cargf(_x*conjf(x_hat));
    //_demod->phase_error = cimagf(_x*conjf(x_hat));
    _demod->phase_error = cargf_demod_approx(_x*conjf(x_hat));
#else
    // re-modulate symbol
    modem_modulate_bpsk(_demod, *_symbol_out, &_demod->x_hat);
#endif
}

// demodulate QPSK
void modem_demodulate_qpsk(modem _demod,
                           float complex _x,
                           unsigned int * _symbol_out)
{
    *_symbol_out  = (crealf(_x) > 0 ) ? 0 : 1;
    *_symbol_out += (cimagf(_x) > 0 ) ? 0 : 2;
    _demod->state = _x;

    // compute residuals
    float complex x_hat;
    modem_modulate_qpsk(_demod, *_symbol_out, &x_hat);
    _demod->res = x_hat - _x;
#if MODEM_UNIFIED_DEMODULATE==0
    _demod->evm = cabsf(_demod->res);
    //_demod->phase_error = cargf(_x*conjf(x_hat));
    //_demod->phase_error = cimagf(_x*conjf(x_hat));
    _demod->phase_error = cargf_demod_approx(_x*conjf(x_hat));
#else
    // re-modulate symbol
    modem_modulate_qpsk(_demod, *_symbol_out, &_demod->x_hat);
#endif
}

// demodulate OOK
void modem_demodulate_ook(modem _demod,
                          float complex _x,
                          unsigned int * _symbol_out)
{
    *_symbol_out = (crealf(_x) > M_SQRT1_2 ) ? 0 : 1;
    _demod->state = _x;

    // compute residuals
    float complex x_hat;
    modem_modulate_ook(_demod, *_symbol_out, &x_hat);
    _demod->res = x_hat - _x;
#if MODEM_UNIFIED_DEMODULATE==0
    _demod->evm = cabsf(_demod->res);
    //_demod->phase_error = cargf(_x*conjf(x_hat));
    //_demod->phase_error = cimagf(_x*conjf(x_hat));
    _demod->phase_error = cargf_demod_approx(_x*conjf(x_hat));
#else
    // re-modulate symbol
    modem_modulate_ook(_demod, *_symbol_out, &_demod->x_hat);
#endif
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
    float complex r = 1.0f;
    switch (quad) {
    case 0: r =  1.0f;          break;  // rotate by  0
    case 1: r =  _Complex_I;    break;  // rotate by +pi/2
    case 2: r = -_Complex_I;    break;  // rotate by -pi/2
    case 3: r = -1.0f;          break;  // rotate by  pi
    default:
        // should never get to this point
        fprintf(stderr,"error: modem_demodulate_sqam32(), logic error\n");
        exit(1);
    }
    //printf(" x = %12.8f +j*%12.8f, quad = %1u, r = %12.8f + j*%12.8f\n",
    //        crealf(_x), cimagf(_x), quad, crealf(r), cimagf(r));
    float complex x_prime = _x * r; // de-rotate symbol to first quadrant
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

    _q->state = _x;

#if MODEM_UNIFIED_DEMODULATE==0
    // compute residuals
    x_hat *= conjf(r);
    _q->res = x_hat - _x;
    _q->evm = cabsf(_q->res);
    //_demod->phase_error = cargf(_x*conjf(x_hat));
    //_demod->phase_error = cimagf(_x*conjf(x_hat));
    _q->phase_error = cargf_demod_approx(_x*conjf(x_hat));
#else
    // re-modulate symbol
    modem_modulate_sqam32(_q, *_symbol_out, &_q->x_hat);
#endif
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
    float complex r = 1.0f;
    switch (quad) {
    case 0: r =  1.0f;          break;  // rotate by  0
    case 1: r =  _Complex_I;    break;  // rotate by +pi/2
    case 2: r = -_Complex_I;    break;  // rotate by -pi/2
    case 3: r = -1.0f;          break;  // rotate by  pi
    default:
        // should never get to this point
        fprintf(stderr,"error: modem_demodulate_sqam128(), logic error\n");
        exit(1);
    }
    //printf(" x = %12.8f +j*%12.8f, quad = %1u, r = %12.8f + j*%12.8f\n",
    //        crealf(_x), cimagf(_x), quad, crealf(r), cimagf(r));
    float complex x_prime = _x * r; // de-rotate symbol to first quadrant
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

    _q->state = _x;

#if MODEM_UNIFIED_DEMODULATE==0
    // compute residuals
    x_hat *= conjf(r);
    _q->res = x_hat - _x;
    _q->evm = cabsf(_q->res);
    //_demod->phase_error = cargf(_x*conjf(x_hat));
    //_demod->phase_error = cimagf(_x*conjf(x_hat));
    _q->phase_error = cargf_demod_approx(_x*conjf(x_hat));
#else
    // re-modulate symbol
    modem_modulate_sqam128(_q, *_symbol_out, &_q->x_hat);
#endif
}

void modem_demodulate_dpsk(modem _demod,
                           float complex _x,
                           unsigned int * _symbol_out)
{
    unsigned int s;
    float theta = cargf(_x);
    float d_theta = theta - _demod->state_theta;
    _demod->state = _x;
    _demod->state_theta = theta;

    // subtract phase offset, ensuring phase is in [-pi,pi)
    d_theta -= _demod->d_phi;
    if (d_theta > M_PI)
        d_theta -= 2*M_PI;
    else if (d_theta < -M_PI)
        d_theta += 2*M_PI;

    modem_demodulate_linear_array_ref(d_theta, _demod->m, _demod->ref, &s, &(_demod->phase_error));
    *_symbol_out = gray_encode(s);

#if MODEM_UNIFIED_DEMODULATE==0
    // compute residuals
#else
    // re-modulate symbol
    // TODO : check this line
    //modem_modulate_psk(_demod, *_symbol_out, &_demod->x_hat);
#endif
}

// demodulate arbitrary modem type
void modem_demodulate_arb(modem _mod,
                          float complex _x,
                          unsigned int * _symbol_out)
{
    //printf("modem_demodulate_arb() invoked with I=%d, Q=%d\n", x);
    
    unsigned int i, s=0;
    float d, d_min = 1e9;

    for (i=0; i<_mod->M; i++) {
        d = cabsf(_x - _mod->symbol_map[i]);

        //printf("  d[%u] = %u\n", i, d);

        if ( d < d_min ) {
            d_min = d;
            s = i;
        }
    }
    //printf(" > s = %d\n", *symbol_out);
    _mod->state = _x;
    *_symbol_out = s;

#if MODEM_UNIFIED_DEMODULATE==0
    // compute residuals
    float complex x_hat = _mod->symbol_map[s];
    _mod->res =  x_hat - _x;
    _mod->evm = cabsf(_mod->res);
    //_mod->phase_error = cabsf(x_hat)*cargf(_x*conjf(x_hat));
    _mod->phase_error = cargf_demod_approx(_x*conjf(x_hat));
#else
    // re-modulate symbol
    modem_modulate_arb(_mod, *_symbol_out, &_mod->x_hat);
#endif
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

    // compute resduals
    // TODO : find better, faster way to compute APSK residuals
    _mod->state = _x;
    modem_modulate(_mod, s_prime, &_mod->x_hat);

#if MODEM_UNIFIED_DEMODULATE==0
    //_mod->phase_error = _mod->apsk_r[p] * cargf(_x*conjf(x_hat));
    _mod->phase_error = cargf_demod_approx(_x*conjf(_mod->x_hat));
    //_mod->phase_error *= sqrtf(_mod->M) / (float)(_mod->apsk_p[p]);

    _mod->res = _x - _mod->x_hat;
#endif
}

// get demodulator phase error
float modem_get_demodulator_phase_error(modem _demod)
{
#if MODEM_UNIFIED_DEMODULATE==0
    return _demod->phase_error;
#else
    if (_demod->scheme == LIQUID_MODEM_DPSK) {
        return _demod->phase_error;
    }
    return cimagf(_demod->state*conjf(_demod->x_hat));
#endif
}

// get error vector magnitude
float modem_get_demodulator_evm(modem _demod)
{
#if MODEM_UNIFIED_DEMODULATE==0
    float r;
    switch (_demod->scheme) {
    case LIQUID_MODEM_UNKNOWN:
        fprintf(stderr,"warning: modem_get_demodulator_evm(), unknown scheme\n");
        return 0.0f;
    case LIQUID_MODEM_PSK:
    case LIQUID_MODEM_BPSK:
    case LIQUID_MODEM_QPSK:
    case LIQUID_MODEM_DPSK:
        // TODO : figure out more efficient way of calculating evm
        r = cabsf(_demod->state);
        _demod->evm = 1.0f + r*r - 2.0f*r*cos(_demod->phase_error);
        _demod->evm = sqrtf( fabsf(_demod->evm) );
        break;
    case LIQUID_MODEM_ASK:
    case LIQUID_MODEM_QAM:
    case LIQUID_MODEM_APSK:
    case LIQUID_MODEM_ARB:
        _demod->evm = cabsf(_demod->res);
        break;
    default:
        // nothing special to do
        ;
    }
    return _demod->evm;
#else
    if (_demod->scheme == LIQUID_MODEM_DPSK) {
        // TODO : figure out more efficient way of calculating evm
        float r = cabsf(_demod->state);
        _demod->evm = 1.0f + r*r - 2.0f*r*cos(_demod->phase_error);
        _demod->evm = sqrtf( fabsf(_demod->evm) );
        return _demod->evm;
    }
    return cabsf(_demod->x_hat - _demod->state);
#endif
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


