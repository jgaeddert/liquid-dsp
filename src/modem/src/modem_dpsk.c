/*
 * Copyright (c) 2007, 2008, 2009, 2010, 2011, 2012 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010, 2011, 2012 Virginia Polytechnic
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
// modem_dpsk.c
//

// create a dpsk (differential phase-shift keying) modem object
MODEM() MODEM(_create_dpsk)(unsigned int _bits_per_symbol)
{
    MODEM() q = (MODEM()) malloc( sizeof(struct MODEM(_s)) );
    
    switch (_bits_per_symbol) {
    case 1: q->scheme = LIQUID_MODEM_DPSK2;   break;
    case 2: q->scheme = LIQUID_MODEM_DPSK4;   break;
    case 3: q->scheme = LIQUID_MODEM_DPSK8;   break;
    case 4: q->scheme = LIQUID_MODEM_DPSK16;  break;
    case 5: q->scheme = LIQUID_MODEM_DPSK32;  break;
    case 6: q->scheme = LIQUID_MODEM_DPSK64;  break;
    case 7: q->scheme = LIQUID_MODEM_DPSK128; break;
    case 8: q->scheme = LIQUID_MODEM_DPSK256; break;
    default:
        fprintf(stderr,"error: modem_create_dpsk(), cannot support DPSK with m > 8\n");
        exit(1);
    }

    MODEM(_init)(q, _bits_per_symbol);

    q->data.dpsk.alpha = M_PI/(T)(q->M);
    
    // set angle state to zero
    q->data.dpsk.phi = 0;

    // initialize demodulation array reference
    unsigned int k;
    for (k=0; k<(q->m); k++)
#if LIQUID_FPM
        q->ref[k] = Q(_angle_float_to_fixed)((1<<k) * q->data.psk.alpha);
#else
        q->ref[k] = (1<<k) * q->data.psk.alpha;
#endif

#if LIQUID_FPM
    q->data.dpsk.d_phi = Q(_pi) - Q(_pi)/q->M;
#else
    q->data.dpsk.d_phi = M_PI*(1.0f - 1.0f/(T)(q->M));
#endif

    // reset modem
    MODEM(_reset)(q);

    q->modulate_func = &MODEM(_modulate_dpsk);
    q->demodulate_func = &MODEM(_demodulate_dpsk);

    return q;
}

// modulate DPSK
void MODEM(_modulate_dpsk)(MODEM()      _q,
                           unsigned int _sym_in,
                           TC *         _y)
{
    // 'encode' input symbol (actually gray decoding)
    _sym_in = gray_decode(_sym_in);

    // compute phase difference between this symbol and the previous
    _q->data.dpsk.phi += _sym_in * 2 * _q->data.dpsk.alpha;

    // limit phase
    _q->data.dpsk.phi -= (_q->data.dpsk.phi > 2*M_PI) ? 2*M_PI : 0.0f;
    
    // compute output sample
    float complex v = liquid_cexpjf(_q->data.dpsk.phi);
#if LIQUID_FPM
    *_y = CQ(_float_to_fixed)(v);
#else
    *_y = v;
#endif

    // save symbol state
    _q->r = *_y;
}


void MODEM(_demodulate_dpsk)(MODEM()        _q,
                             TC             _x,
                             unsigned int * _sym_out)
{
#if LIQUID_FPM
    // compute angle difference
    T theta   = CQ(_carg)(_x);
    T d_theta = theta - _q->data.dpsk.phi;
    _q->data.dpsk.phi = theta;

    // subtract phase offset, ensuring phase is in [-pi,pi)
    d_theta -= _q->data.dpsk.d_phi;
    if      (d_theta >  Q(_pi)) d_theta -= Q(_2pi);
    else if (d_theta < -Q(_pi)) d_theta += Q(_2pi);
#else
    // compute angle difference
    T theta = cargf(_x);
    T d_theta = theta - _q->data.dpsk.phi;
    _q->data.dpsk.phi = theta;

    // subtract phase offset, ensuring phase is in [-pi,pi)
    d_theta -= _q->data.dpsk.d_phi;
    if      (d_theta >  M_PI) d_theta -= 2*M_PI;
    else if (d_theta < -M_PI) d_theta += 2*M_PI;
#endif

    // demodulate on linearly-spaced array
    unsigned int s;             // demodulated symbol
    T demod_phase_error;    // demodulation phase error
    MODEM(_demodulate_linear_array_ref)(d_theta, _q->m, _q->ref, &s, &demod_phase_error);

    // 'decode' output symbol (actually gray encoding)
    *_sym_out = gray_encode(s);

    // re-modulate symbol (accounting for differential rotation)
    // and store state
#if LIQUID_FPM
    _q->x_hat = CQ(_cexpj)(theta - demod_phase_error);
#else
    _q->x_hat = liquid_cexpjf(theta - demod_phase_error);
#endif
    _q->r = _x;
}

