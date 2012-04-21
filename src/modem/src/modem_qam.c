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
// modem_qam.c
//

#include <assert.h>

// create a qam (quaternary amplitude-shift keying) modem object
modem modem_create_qam(unsigned int _bits_per_symbol)
{
    if (_bits_per_symbol < 1 ) {
        fprintf(stderr,"error: modem_create_qam(), modem must have at least 2 bits/symbol\n");
        exit(1);
    }

    modem mod = (modem) malloc( sizeof(struct modem_s) );

    modem_init(mod, _bits_per_symbol);

    if (mod->m % 2) {
        // rectangular qam
        mod->m_i = (mod->m + 1) >> 1;
        mod->m_q = (mod->m - 1) >> 1;
    } else {
        // square qam
        mod->m_i = mod->m >> 1;
        mod->m_q = mod->m >> 1;
    }

    mod->M_i = 1 << (mod->m_i);
    mod->M_q = 1 << (mod->m_q);

    assert(mod->m_i + mod->m_q == mod->m);
    assert(mod->M_i * mod->M_q == mod->M);

    switch (mod->M) {
    case 4:     mod->alpha = RQAM4_ALPHA;    mod->scheme = LIQUID_MODEM_QAM4;   break;
    case 8:     mod->alpha = RQAM8_ALPHA;    mod->scheme = LIQUID_MODEM_QAM8;   break;
    case 16:    mod->alpha = RQAM16_ALPHA;   mod->scheme = LIQUID_MODEM_QAM16;  break;
    case 32:    mod->alpha = RQAM32_ALPHA;   mod->scheme = LIQUID_MODEM_QAM32;  break;
    case 64:    mod->alpha = RQAM64_ALPHA;   mod->scheme = LIQUID_MODEM_QAM64;  break;
    case 128:   mod->alpha = RQAM128_ALPHA;  mod->scheme = LIQUID_MODEM_QAM128; break;
    case 256:   mod->alpha = RQAM256_ALPHA;  mod->scheme = LIQUID_MODEM_QAM256; break;
#if 0
    case 512:   mod->alpha = RQAM512_ALPHA;     break;
    case 1024:  mod->alpha = RQAM1024_ALPHA;    break;
    case 2048:  mod->alpha = RQAM2048_ALPHA;    break;
    case 4096:  mod->alpha = RQAM4096_ALPHA;    break;
    default:
        // calculate alpha dynamically
        // NOTE: this is only an approximation
        mod->alpha = sqrtf(2.0f / (float)(mod->M) );
#else
    default:
        fprintf(stderr,"error: modem_create_qam(), cannot support QAM with m > 8\n");
        exit(1);
#endif
    }

    unsigned int k;
    for (k=0; k<(mod->m); k++)
        mod->ref[k] = (1<<k) * mod->alpha;

    mod->modulate_func = &modem_modulate_qam;
    mod->demodulate_func = &modem_demodulate_qam;

    // initialize symbol map
    mod->symbol_map = (float complex*)malloc(mod->M*sizeof(float complex));
    modem_init_map(mod);
    mod->modulate_using_map = 1;

    // soft demodulation
    if (mod->m == 3) {
        mod->demod_soft_neighbors = (unsigned char*) qam8_demod_soft_neighbors;
        mod->demod_soft_p         = 3;
    } else if (mod->m == 4) {
        mod->demod_soft_neighbors = (unsigned char*) qam16_demod_soft_neighbors;
        mod->demod_soft_p         = 4;
    } else if (mod->m == 5) {
        mod->demod_soft_neighbors = (unsigned char*) qam32_demod_soft_neighbors;
        mod->demod_soft_p         = 4;
    } else if (mod->m == 6) {
        mod->demod_soft_neighbors = (unsigned char*) qam64_demod_soft_neighbors;
        mod->demod_soft_p         = 4;
    } else if (mod->m == 7) {
        mod->demod_soft_neighbors = (unsigned char*) qam128_demod_soft_neighbors;
        mod->demod_soft_p         = 4;
    } else if (mod->m == 8) {
        mod->demod_soft_neighbors = (unsigned char*) qam256_demod_soft_neighbors;
        mod->demod_soft_p         = 4;
    }

    return mod;
}

// modulate QAM
void modem_modulate_qam(modem _mod,
                        unsigned int symbol_in,
                        float complex *y)
{
    unsigned int s_i;   // in-phase symbol
    unsigned int s_q;   // quadrature symbol
    s_i = symbol_in >> _mod->m_q;
    s_q = symbol_in & ( (1<<_mod->m_q)-1 );

    // 'encode' symbols (actually gray decoding)
    s_i = gray_decode(s_i);
    s_q = gray_decode(s_q);

    // compute output sample
    *y = (2*(int)s_i - (int)(_mod->M_i) + 1) * _mod->alpha +
         (2*(int)s_q - (int)(_mod->M_q) + 1) * _mod->alpha * _Complex_I;
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

