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
// modem_psk.c
//

// create a psk (phase-shift keying) modem object
modem modem_create_psk(unsigned int _bits_per_symbol)
{
    modem mod = (modem) malloc( sizeof(struct modem_s) );

    switch (_bits_per_symbol) {
    case 1: mod->scheme = LIQUID_MODEM_PSK2;   break;
    case 2: mod->scheme = LIQUID_MODEM_PSK4;   break;
    case 3: mod->scheme = LIQUID_MODEM_PSK8;   break;
    case 4: mod->scheme = LIQUID_MODEM_PSK16;  break;
    case 5: mod->scheme = LIQUID_MODEM_PSK32;  break;
    case 6: mod->scheme = LIQUID_MODEM_PSK64;  break;
    case 7: mod->scheme = LIQUID_MODEM_PSK128; break;
    case 8: mod->scheme = LIQUID_MODEM_PSK256; break;
    default:
        fprintf(stderr,"error: modem_create_psk(), cannot support PSK with m > 8\n");
        exit(1);
    }

    // initialize basic modem structure
    modem_init(mod, _bits_per_symbol);

    // compute alpha
    mod->alpha = M_PI/(float)(mod->M);

    // initialize demodulation array reference
    unsigned int k;
    for (k=0; k<(mod->m); k++)
        mod->ref[k] = (1<<k) * mod->alpha;

    // compute phase offset (half of phase difference between symbols)
    mod->d_phi = M_PI*(1.0f - 1.0f/(float)(mod->M));

    // set modulation/demodulation functions
    mod->modulate_func = &modem_modulate_psk;
    mod->demodulate_func = &modem_demodulate_psk;

    // initialize symbol map
    mod->symbol_map = (float complex*)malloc(mod->M*sizeof(float complex));
    modem_init_map(mod);
    mod->modulate_using_map = 1;

    // soft demodulation
    if (mod->m == 3) {
        mod->demod_soft_neighbors = (unsigned char*) psk8_demod_soft_neighbors;
        mod->demod_soft_p         = 2;
    } else if (mod->m == 4) {
        mod->demod_soft_neighbors = (unsigned char*) psk16_demod_soft_neighbors;
        mod->demod_soft_p         = 2;
    } else if (mod->m == 5) {
        mod->demod_soft_neighbors = (unsigned char*) psk32_demod_soft_neighbors;
        mod->demod_soft_p         = 2;
    }

    return mod;
}

// modulate PSK
void modem_modulate_psk(modem _mod,
                        unsigned int symbol_in,
                        float complex *y)
{
    // 'encode' input symbol (actually gray decoding)
    symbol_in = gray_decode(symbol_in);

    // compute output sample
    *y = liquid_cexpjf(symbol_in * 2 * _mod->alpha );
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

