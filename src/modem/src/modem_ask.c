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
// modem_ask.c
//

// create an ask (amplitude-shift keying) modem object
modem modem_create_ask(unsigned int _bits_per_symbol)
{
    modem mod = (modem) malloc( sizeof(struct modem_s) );

    modem_init(mod, _bits_per_symbol);

    mod->m_i = mod->m;
    mod->M_i = mod->M;

    switch (mod->M) {
    case 2:     mod->alpha = ASK2_ALPHA;   mod->scheme = LIQUID_MODEM_ASK2;   break;
    case 4:     mod->alpha = ASK4_ALPHA;   mod->scheme = LIQUID_MODEM_ASK4;   break;
    case 8:     mod->alpha = ASK8_ALPHA;   mod->scheme = LIQUID_MODEM_ASK8;   break;
    case 16:    mod->alpha = ASK16_ALPHA;  mod->scheme = LIQUID_MODEM_ASK16;  break;
    case 32:    mod->alpha = ASK32_ALPHA;  mod->scheme = LIQUID_MODEM_ASK32;  break;
    case 64:    mod->alpha = ASK64_ALPHA;  mod->scheme = LIQUID_MODEM_ASK64;  break;
    case 128:   mod->alpha = ASK128_ALPHA; mod->scheme = LIQUID_MODEM_ASK128; break;
    case 256:   mod->alpha = ASK256_ALPHA; mod->scheme = LIQUID_MODEM_ASK256; break;
    default:
#if 0
        // calculate alpha dynamically
        mod->alpha = expf(-0.70735 + 0.63653*mod->m);
#else
        fprintf(stderr,"error: modem_create_ask(), cannot support ASK with m > 8\n");
        exit(1);
#endif
    }

    unsigned int k;
    for (k=0; k<(mod->m); k++)
        mod->ref[k] = (1<<k) * mod->alpha;

    mod->modulate_func = &modem_modulate_ask;
    mod->demodulate_func = &modem_demodulate_ask;

    // soft demodulation
    if (mod->m == 2) {
        mod->demod_soft_neighbors = (unsigned char*) ask4_demod_soft_neighbors;
        mod->demod_soft_p         = 2;
    } else if (mod->m == 3) {
        mod->demod_soft_neighbors = (unsigned char*) ask8_demod_soft_neighbors;
        mod->demod_soft_p         = 2;
    } else if (mod->m == 4) {
        mod->demod_soft_neighbors = (unsigned char*) ask16_demod_soft_neighbors;
        mod->demod_soft_p         = 2;
    }

    return mod;
}

// modulate ASK
void modem_modulate_ask(modem _mod,
                        unsigned int symbol_in,
                        float complex *y)
{
    // 'encode' input symbol (actually gray decoding)
    symbol_in = gray_decode(symbol_in);

    // modulate symbol
    *y = (2*(int)symbol_in - (int)(_mod->M) + 1) * _mod->alpha;
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

