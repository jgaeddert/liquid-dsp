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
//
//

#include <stdio.h>
#include <math.h>
#include "liquid.internal.h"

float complex cargf_demod_approx(float complex _x)
{
    float theta = cimagf(_x) / fabsf(crealf(_x));
    if (theta >  M_PI_2)
        theta =  M_PI_2;
    else if (theta < -M_PI_2)
        theta = -M_PI_2;
    return theta;
}

void modem_demodulate(
    modem _demod,
    float complex x,
    unsigned int *symbol_out)
{
    _demod->demodulate_func(_demod, x, symbol_out);
}

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

    // compute residuals
    float complex x_hat = _x + _demod->res;
    //_demod->phase_error = cabsf(x_hat)*cargf(x_hat*conjf(_x));
    _demod->phase_error = cargf_demod_approx(x_hat*conjf(_x));
    _demod->evm = cabsf(_demod->res);
}

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

    // compute residuals
    float complex x_hat = _x + _demod->res;
    //_demod->phase_error = cabsf(x_hat)*cargf(x_hat*conjf(_x));
    //_demod->phase_error = cimagf(x_hat*conjf(_x));
    _demod->phase_error = cargf_demod_approx(x_hat*conjf(_x));
    _demod->evm = cabsf(_demod->res);
}


void modem_demodulate_psk(modem _demod,
                          float complex x,
                          unsigned int *symbol_out)
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
    *symbol_out = gray_encode(s);

    // compute residuals
    // phase error computed as residual from demodulator
}

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
    _demod->evm = cabsf(_demod->res);
    //_demod->phase_error = cargf(_x*conjf(x_hat));
    //_demod->phase_error = cimagf(_x*conjf(x_hat));
    _demod->phase_error = cargf_demod_approx(_x*conjf(x_hat));
}

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
    _demod->evm = cabsf(_demod->res);
    //_demod->phase_error = cargf(_x*conjf(x_hat));
    //_demod->phase_error = cimagf(_x*conjf(x_hat));
    _demod->phase_error = cargf_demod_approx(_x*conjf(x_hat));
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

    // compute residuals
}

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

    // compute residuals
    float complex x_hat = _mod->symbol_map[s];
    _mod->res =  x_hat - _x;
    _mod->evm = cabsf(_mod->res);
    //_mod->phase_error = cabsf(x_hat)*cargf(_x*conjf(x_hat));
    _mod->phase_error = cargf_demod_approx(_x*conjf(x_hat));
}

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
    float complex x_hat;
    modem_modulate_apsk(_mod, s_prime, &x_hat);

    //_mod->phase_error = _mod->apsk_r[p] * cargf(_x*conjf(x_hat));
    _mod->phase_error = cargf_demod_approx(_x*conjf(x_hat));
    //_mod->phase_error *= sqrtf(_mod->M) / (float)(_mod->apsk_p[p]);

    _mod->res = _x - x_hat;
}

#if 0
void modem_demodulate_arb_mirrored(modem _mod, float I_in, float Q_in, unsigned int *symbol_out)
{
    //printf("modem_demodulate_arb_mirrored() invoked with I=%d, Q=%d\n", x);
    
    unsigned int i;
    unsigned int s_quad=0;
    unsigned int s_q1=0;
    float d, d_min = 1e9;

    // determine quadrant, demodulate 2 most-significant bits appropriately
    // IQ:
    // -----
    // 01 00
    // 11 10
    if ( I_in < 0 )
        s_quad |= 0x01;

    if ( Q_in < 0 )
        s_quad |= 0x02;

    // mirrored
    I_in = fabsf(I_in);
    Q_in = fabsf(Q_in);

    for (i=0; i<_mod->M; i++) {
        d = liquid_cabsf(
            I_in - _mod->levels_i[i],
            Q_in - _mod->levels_q[i]);

        //printf("  d[%u] = %u\n", i, d);

        if ( d < d_min ) {
            d_min = d;
            s_q1 = i;
        }
    }
    //printf(" > s = %d\n", *symbol_out);
    _mod->res_i = I_in - _mod->levels_i[s_q1];
    _mod->res_q = Q_in - _mod->levels_q[s_q1];
    _mod->state_i = I_in;
    _mod->state_q = Q_in;

    // output symbol : <q(1) q(0)> <b(m-2-1) b(m-2-2) ... b(0)>
    *symbol_out = (s_quad << (_mod->m - 2)) | s_q1;
}
#endif

#if 0
void modem_modem_demodulate_arb_rotated(modem _mod, float I_in, float Q_in, unsigned int *symbol_out)
{
    //printf("demodulate_arb_rotated() invoked with I=%d, Q=%d\n", x);
    
    unsigned int i;
    unsigned int s_quad=0;
    unsigned int s_q1=0;
    float d, d_min = 1e9;

    // determine quadrant, demodulate 2 most-significant bits appropriately
    // IQ:
    // -----
    // 01 00
    // 11 10
    if ( I_in < 0 )
        s_quad |= 0x01;

    if ( Q_in < 0 )
        s_quad |= 0x02;

    // rotated
    float I = I_in;
    float Q = Q_in;
    if (s_quad == 0) {
        I_in =  I;
        Q_in =  Q;
    } else if (s_quad == 1) {
        I_in =  Q;
        Q_in =-I;
    } else if (s_quad == 3) {
        I_in = -I;
        Q_in = -Q;
    } else if (s_quad == 2) {
        I_in = -Q;
        Q_in =  I;
    }

    for (i=0; i<_mod->M; i++) {
        d = liquid_cabsf(
            I_in - _mod->levels_i[i],
            Q_in - _mod->levels_q[i]);

        //printf("  d[%u] = %u\n", i, d);

        if ( d < d_min ) {
            d_min = d;
            s_q1 = i;
        }
    }
    //printf(" > s = %d\n", *symbol_out);
    _mod->res_i = I_in - _mod->levels_i[s_q1];
    _mod->res_q = Q_in - _mod->levels_q[s_q1];
    _mod->state_i = I_in;
    _mod->state_q = Q_in;

    // output symbol : <q(1) q(0)> <b(m-2-1) b(m-2-2) ... b(0)>
    *symbol_out = (s_quad << (_mod->m - 2)) | s_q1;
}
#endif

// get demodulator phase error
void get_demodulator_phase_error(modem _demod, float* _phi)
{
    *_phi = _demod->phase_error;
}

// get error vector magnitude
void get_demodulator_evm(modem _demod, float* _evm)
{
    float r;
    switch (_demod->scheme) {
    case MOD_PSK:
    case MOD_BPSK:
    case MOD_QPSK:
    case MOD_DPSK:
        ///\todo figure out more efficient way of calculating evm
        r = cabsf(_demod->state);
        _demod->evm = 1.0f + r*r - 2.0f*r*cos(_demod->phase_error);
        _demod->evm = sqrtf( fabsf(_demod->evm) );
        break;
    case MOD_ASK:
    case MOD_QAM:
    case MOD_APSK:
    case MOD_APSK8:
    case MOD_APSK16:
    case MOD_APSK32:
    case MOD_APSK64:
    case MOD_APSK128:
    case MOD_ARB:
    case MOD_ARB_MIRRORED:
    case MOD_ARB_ROTATED:
        _demod->evm = cabsf(_demod->res);
        break;
    default:
        printf("WARNING: get_demodulator_evm(), unknown scheme\n");
        break;
    }
    *_evm = _demod->evm;
}


void modem_demodulate_linear_array(
    float _v,
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

void modem_demodulate_linear_array_ref(
    float _v,
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


