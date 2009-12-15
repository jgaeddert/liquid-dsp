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


void modem_demodulate(
    modem _demod,
    float complex x,
    unsigned int *symbol_out)
{
    _demod->demodulate_func(_demod, x, symbol_out);
}

void modem_demodulate_ask(
    modem _demod,
    float complex x,
    unsigned int *symbol_out)
{
    unsigned int s;
    _demod->state = x;
    float res_i;
    modem_demodulate_linear_array_ref(crealf(x), _demod->m, _demod->ref, &s, &res_i);
    _demod->res = res_i + _Complex_I*cimagf(x);
    *symbol_out = gray_encode(s);
}

void modem_demodulate_qam(
    modem _demod,
    float complex x,
    unsigned int *symbol_out)
{
    unsigned int s_i, s_q;
    _demod->state = x;
    float res_i, res_q;
    modem_demodulate_linear_array_ref(crealf(x), _demod->m_i, _demod->ref, &s_i, &res_i);
    modem_demodulate_linear_array_ref(cimagf(x), _demod->m_q, _demod->ref, &s_q, &res_q);
    _demod->res = res_i + _Complex_I*res_q;
    s_i = gray_encode(s_i);
    s_q = gray_encode(s_q);
    *symbol_out = ( s_i << _demod->m_q ) + s_q;
}


void modem_demodulate_psk(
    modem _demod,
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
}

void modem_demodulate_bpsk(
    modem _demod,
    float complex x,
    unsigned int *symbol_out)
{
    *symbol_out = (crealf(x) > 0 ) ? 0 : 1;
    _demod->state = x;
}

void modem_demodulate_qpsk(
    modem _demod,
    float complex x,
    unsigned int *symbol_out)
{
    *symbol_out  = (crealf(x) > 0 ) ? 0 : 1;
    *symbol_out += (cimagf(x) > 0 ) ? 0 : 2;
    _demod->state = x;
}

void modem_demodulate_dpsk(
    modem _demod,
    float complex x,
    unsigned int *symbol_out)
{
    unsigned int s;
    float theta = cargf(x);
    float d_theta = theta - _demod->state_theta;
    _demod->state = x;
    _demod->state_theta = theta;

    // subtract phase offset, ensuring phase is in [-pi,pi)
    d_theta -= _demod->d_phi;
    if (d_theta > M_PI)
        d_theta -= 2*M_PI;
    else if (d_theta < -M_PI)
        d_theta += 2*M_PI;

    modem_demodulate_linear_array_ref(d_theta, _demod->m, _demod->ref, &s, &(_demod->phase_error));
    *symbol_out = gray_encode(s);
}

void modem_demodulate_arb(
    modem _mod,
    float complex x,
    unsigned int *symbol_out)
{
    //printf("modem_demodulate_arb() invoked with I=%d, Q=%d\n", x);
    
    unsigned int i, s=0;
    float d, d_min = 1e9;

    for (i=0; i<_mod->M; i++) {
        d = cabsf(x - _mod->symbol_map[i]);

        //printf("  d[%u] = %u\n", i, d);

        if ( d < d_min ) {
            d_min = d;
            s = i;
        }
    }
    //printf(" > s = %d\n", *symbol_out);
    _mod->res = x - _mod->symbol_map[s];
    _mod->state = x;

    *symbol_out = s;
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
    float phi = _mod->apsk_phi[p];
    float phi_hat = 0.0f;
    float d, d_hat, dmin = 2.0f*M_PI;
    unsigned int s_hat=0;
    for (i=0; i<_mod->apsk_p[p]; i++) {
        d = theta - phi;

        // constrain phase to be in [-pi,pi]
        if (d < -M_PI) d += 2.0f*M_PI;
        if (d >  M_PI) d -= 2.0f*M_PI;

        d_hat = fabsf(d);
        if (d_hat < dmin) {
            dmin = d_hat;
            s_hat = i;
            phi_hat = phi;
            _mod->phase_error = d * _mod->apsk_r[p];
        }

        phi += dphi;
    }
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
    printf("        phi_hat : %12.8f\n", phi_hat);
    printf("           dmin : %12.8f\n", dmin);
    printf("              s : %3u > %3u\n", s_hat, s_prime);
#endif

    *_symbol_out = s_prime;
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
    switch (_demod->scheme) {
    case MOD_PSK:
    case MOD_DPSK:
        // no need to calculate phase error from generic PSK/DPSK
        break;
    case MOD_BPSK:
        _demod->phase_error = cargf(_demod->state);
        if (_demod->phase_error > M_PI / 2.0f)
            _demod->phase_error -= M_PI;
        else if (_demod->phase_error < -M_PI / 2.0f)
            _demod->phase_error += M_PI;
        break;
    case MOD_QPSK:
        _demod->phase_error = cargf(_demod->state);

        // fold phase error angle onto Q1 projection, 0 <= phase_error <= pi/2
        if (_demod->phase_error < 0 )
            _demod->phase_error += M_PI;
        if (_demod->phase_error > M_PI / 2.0f)
            _demod->phase_error -= M_PI / 2.0f;
        _demod->phase_error -= M_PI / 4.0f;
        break;
    case MOD_ASK:
        _demod->phase_error = (crealf(_demod->state) > 0.0f) ?
             cimagf(_demod->state) :
            -cimagf(_demod->state);
        break;
    case MOD_QAM:
    case MOD_ARB:
    case MOD_ARB_MIRRORED:
    case MOD_ARB_ROTATED:
        if (cabsf(_demod->res) < 1e-3f) {
            _demod->phase_error = 0.0f;
        } else {
            // calculate phase difference between received signal and constellation point
            //_demod->phase_error =
            //    atan2(_demod->state_q + _demod->res_q, _demod->state_i + _demod->res_i)
            //   -atan2(_demod->state_q, _demod->state_i);
            _demod->phase_error = cargf(_demod->state + _demod->res) - cargf(_demod->state);

            // normalize phase error by signal amplitude
            _demod->phase_error *= cabsf(_demod->state);
        }

        break;
    default:;
    }

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


