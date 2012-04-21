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
modem modem_create_dpsk(unsigned int _bits_per_symbol)
{
    modem mod = (modem) malloc( sizeof(struct modem_s) );
    
    switch (_bits_per_symbol) {
    case 1: mod->scheme = LIQUID_MODEM_DPSK2;   break;
    case 2: mod->scheme = LIQUID_MODEM_DPSK4;   break;
    case 3: mod->scheme = LIQUID_MODEM_DPSK8;   break;
    case 4: mod->scheme = LIQUID_MODEM_DPSK16;  break;
    case 5: mod->scheme = LIQUID_MODEM_DPSK32;  break;
    case 6: mod->scheme = LIQUID_MODEM_DPSK64;  break;
    case 7: mod->scheme = LIQUID_MODEM_DPSK128; break;
    case 8: mod->scheme = LIQUID_MODEM_DPSK256; break;
    default:
        fprintf(stderr,"error: modem_create_dpsk(), cannot support DPSK with m > 8\n");
        exit(1);
    }

    modem_init(mod, _bits_per_symbol);

    mod->alpha = M_PI/(float)(mod->M);

    unsigned int k;
    for (k=0; k<(mod->m); k++)
        mod->ref[k] = (1<<k) * mod->alpha;

    mod->d_phi = M_PI*(1.0f - 1.0f/(float)(mod->M));

    // reset modem
    modem_reset(mod);

    mod->modulate_func = &modem_modulate_dpsk;
    mod->demodulate_func = &modem_demodulate_dpsk;

    return mod;
}

// modulate DPSK
void modem_modulate_dpsk(modem _mod,
                         unsigned int symbol_in,
                         float complex *y)
{
    // 'encode' input symbol (actually gray decoding)
    symbol_in = gray_decode(symbol_in);

    // compute phase difference between this symbol and the previous
    _mod->dpsk_phi += symbol_in * 2 * _mod->alpha;

    // limit phase
    _mod->dpsk_phi -= (_mod->dpsk_phi > 2*M_PI) ? 2*M_PI : 0.0f;
    
    // compute output sample
    *y = liquid_cexpjf(_mod->dpsk_phi);

    // save symbol state
    _mod->r = *y;
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

