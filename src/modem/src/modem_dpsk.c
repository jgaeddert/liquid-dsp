/*
 * Copyright (c) 2007 - 2015 Joseph Gaeddert
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
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

    q->modulate_func = &MODEM(_modulate_dpsk);
    q->demodulate_func = &MODEM(_demodulate_dpsk);

    // reset and return
    MODEM(_reset)(q);
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

