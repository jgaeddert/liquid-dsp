/*
 * Copyright (c) 2007, 2008, 2009, 2010, 2011 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010, 2011 Virginia Polytechnic
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
// modem_create.c
//
// Create and initialize linear digital modem schemes.
//

#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "liquid.internal.h"

// create digital modem of a specific scheme and bits/symbol
modem modem_create(modulation_scheme _scheme,
                   unsigned int _bits_per_symbol)
{
    if (_bits_per_symbol < 1 ) {
        fprintf(stderr,"error: modem_create(), modem must have at least 1 bit/symbol\n");
        exit(1);
    } else if (_bits_per_symbol > MAX_MOD_BITS_PER_SYMBOL) {
        fprintf(stderr,"error: modem_create(), maximum number of bits/symbol (%u) exceeded\n",
                MAX_MOD_BITS_PER_SYMBOL);
        exit(1);
    }

    switch (_scheme) {
    case LIQUID_MODEM_PSK:      return modem_create_psk(_bits_per_symbol);
    case LIQUID_MODEM_DPSK:     return modem_create_dpsk(_bits_per_symbol);
    case LIQUID_MODEM_ASK:      return modem_create_ask(_bits_per_symbol);
    case LIQUID_MODEM_QAM:      return modem_create_qam(_bits_per_symbol);
    case LIQUID_MODEM_APSK:     return modem_create_apsk(_bits_per_symbol);

    // arbitrary modem definitions
    case LIQUID_MODEM_ARB:      return modem_create_arb(_bits_per_symbol);

    // specific modems
    case LIQUID_MODEM_BPSK:     return modem_create_bpsk();
    case LIQUID_MODEM_QPSK:     return modem_create_qpsk();
    case LIQUID_MODEM_OOK:      return modem_create_ook();
    case LIQUID_MODEM_SQAM32:   return modem_create_sqam32();
    case LIQUID_MODEM_SQAM128:  return modem_create_sqam128();

    // arbitrary modems
    case LIQUID_MODEM_V29:      return modem_create_V29();
    case LIQUID_MODEM_ARB16OPT: return modem_create_arb16opt();
    case LIQUID_MODEM_ARB32OPT: return modem_create_arb32opt();
    case LIQUID_MODEM_ARB64OPT: return modem_create_arb64opt();
    case LIQUID_MODEM_ARB128OPT: return modem_create_arb128opt();
    case LIQUID_MODEM_ARB256OPT: return modem_create_arb256opt();
    case LIQUID_MODEM_ARB64VT:  return modem_create_arb64vt();
    default:
        fprintf(stderr,"error: modem_create(), unknown/unsupported modulation scheme : %u (%u b/s)\n",
                _scheme, _bits_per_symbol);
        exit(-1);
    }

    // should never get to this point, but adding return statment
    // to keep compiler happy
    return NULL;
}

// recreate modulation scheme, re-allocating memory as necessary
modem modem_recreate(modem _q,
                     modulation_scheme _scheme,
                     unsigned int _bits_per_symbol)
{
    if (_bits_per_symbol < 1 ) {
        fprintf(stderr,"error: modem_recreate(), modem must have at least 1 bit/symbol\n");
        exit(1);
    } else if (_bits_per_symbol > MAX_MOD_BITS_PER_SYMBOL) {
        fprintf(stderr,"error: modem_recreate(), maximum number of bits/symbol (%u) exceeded\n",
                MAX_MOD_BITS_PER_SYMBOL);
        exit(1);
    }

    // TODO : regenerate modem only when truly necessary
    if (_q->scheme != _scheme || _q->m != _bits_per_symbol) {
        // destroy and re-create modem
        modem_destroy(_q);
        _q = modem_create(_scheme, _bits_per_symbol);
    }

    // return object
    return _q;
}

// destroy a modem object
void modem_destroy(modem _mod)
{
    // free internally-allocated memory
    if (_mod->symbol_map != NULL)
        free(_mod->symbol_map);

    // free main object memory
    free(_mod);
}

// print a modem object
void modem_print(modem _mod)
{
    printf("linear modem:\n");
    printf("    scheme:         %s\n", modulation_scheme_str[_mod->scheme][0]);
    printf("    bits/symbol:    %u\n", _mod->m);
}

// reset a modem object (only an issue with dpsk)
void modem_reset(modem _mod)
{
    _mod->r = 1.0f;         // received sample
    _mod->x_hat = _mod->r;  // estimated symbol
    _mod->dpsk_phi = 0.0f;  // reset differential PSK phase state
}

// initialize a generic modem object
void modem_init(modem _mod,
                unsigned int _bits_per_symbol)
{
    if (_bits_per_symbol < 1 ) {
        fprintf(stderr,"error: modem_init(), modem must have at least 1 bit/symbol\n");
        exit(1);
    } else if (_bits_per_symbol > MAX_MOD_BITS_PER_SYMBOL) {
        fprintf(stderr,"error: modem_init(), maximum number of bits per symbol exceeded\n");
        exit(1);
    }

    // initialize common elements
    _mod->symbol_map = NULL;    // symbol map (LIQUID_MODEM_ARB only)
    _mod->modulate_using_map=0; // modulate using map flag
    _mod->alpha = 0.0f;         // scaling factor

    // QAM modem
    _mod->m = _bits_per_symbol; // bits/symbol
    _mod->M = 1 << (_mod->m);   // constellation size (2^m)
    _mod->m_i = 0;              // bits/symbol (in-phase)
    _mod->M_i = 0;              // constellation size (in-phase)
    _mod->m_q = 0;              // bits/symbol (quadrature-phase)
    _mod->M_q = 0;              // constellation size (quadrature-phase)

    // PSK/DPSK modem
    _mod->d_phi = 0.0f;         // half of angle between symbols
    _mod->dpsk_phi = 0.0f;      // angle state for differential PSK

    // APSK modem
    _mod->apsk_num_levels = 0;  // number of levels
    _mod->apsk_p = NULL;        // number of levels per symbol
    _mod->apsk_r = NULL;        // number of levels per symbol
    _mod->apsk_r_slicer = NULL; // radii of levels
    _mod->apsk_phi = NULL;      // phase offset of levels
    _mod->apsk_symbol_map=NULL; // symbol mapping

    // set function pointers initially to NULL
    _mod->modulate_func = NULL;
    _mod->demodulate_func = NULL;

    // soft demodulation
    _mod->demod_soft_neighbors = NULL;
    _mod->demod_soft_p = 0;

    // reset object
    modem_reset(_mod);
}

// initialize symbol map for fast modulation
void modem_init_map(modem _q)
{
    // validate input
    if (_q->symbol_map == NULL) {
        fprintf(stderr,"error: modem_init_map(), symbol map array has not been allocated\n");
        exit(1);
    } else if (_q->M == 0 || _q->M > (1<<MAX_MOD_BITS_PER_SYMBOL)) {
        fprintf(stderr,"error: modem_init_map(), constellation size is out of range\n");
        exit(1);
    } else if (_q->modulate_func == NULL) {
        fprintf(stderr,"error: modem_init_map(), modulation function has not been initialized\n");
        exit(1);
    }

    unsigned int i;
    for (i=0; i<_q->M; i++)
        _q->modulate_func(_q, i, &_q->symbol_map[i]);
}

// create an ask (amplitude-shift keying) modem object
modem modem_create_ask(unsigned int _bits_per_symbol)
{
    modem mod = (modem) malloc( sizeof(struct modem_s) );
    mod->scheme = LIQUID_MODEM_ASK;

    modem_init(mod, _bits_per_symbol);

    mod->m_i = mod->m;
    mod->M_i = mod->M;

    switch (mod->M) {
    case 2:     mod->alpha = ASK2_ALPHA;     break;
    case 4:     mod->alpha = ASK4_ALPHA;     break;
    case 8:     mod->alpha = ASK8_ALPHA;     break;
    case 16:    mod->alpha = ASK16_ALPHA;    break;
    case 32:    mod->alpha = ASK32_ALPHA;    break;
    case 64:    mod->alpha = ASK64_ALPHA;    break;
    case 128:   mod->alpha = ASK128_ALPHA;   break;
    case 256:   mod->alpha = ASK256_ALPHA;   break;
    default:
        // calculate alpha dynamically
        mod->alpha = expf(-0.70735 + 0.63653*mod->m);
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

// create a qam (quaternary amplitude-shift keying) modem object
modem modem_create_qam(unsigned int _bits_per_symbol)
{
    if (_bits_per_symbol < 1 ) {
        fprintf(stderr,"error: modem_create_qam(), modem must have at least 2 bits/symbol\n");
        exit(1);
    }

    modem mod = (modem) malloc( sizeof(struct modem_s) );
    mod->scheme = LIQUID_MODEM_QAM;

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
    case 4:     mod->alpha = RQAM4_ALPHA;       break;
    case 8:     mod->alpha = RQAM8_ALPHA;       break;
    case 16:    mod->alpha = RQAM16_ALPHA;      break;
    case 32:    mod->alpha = RQAM32_ALPHA;      break;
    case 64:    mod->alpha = RQAM64_ALPHA;      break;
    case 128:   mod->alpha = RQAM128_ALPHA;     break;
    case 256:   mod->alpha = RQAM256_ALPHA;     break;
    case 512:   mod->alpha = RQAM512_ALPHA;     break;
    case 1024:  mod->alpha = RQAM1024_ALPHA;    break;
    case 2048:  mod->alpha = RQAM2048_ALPHA;    break;
    case 4096:  mod->alpha = RQAM4096_ALPHA;    break;
    default:
        // calculate alpha dynamically
        // NOTE: this is only an approximation
        mod->alpha = sqrtf(2.0f / (float)(mod->M) );
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

// create a psk (phase-shift keying) modem object
modem modem_create_psk(unsigned int _bits_per_symbol)
{
    modem mod = (modem) malloc( sizeof(struct modem_s) );
    mod->scheme = LIQUID_MODEM_PSK;

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

// create a bpsk (binary phase-shift keying) modem object
modem modem_create_bpsk()
{
    modem mod = (modem) malloc( sizeof(struct modem_s) );
    mod->scheme = LIQUID_MODEM_BPSK;

    modem_init(mod, 1);

    mod->modulate_func = &modem_modulate_bpsk;
    mod->demodulate_func = &modem_demodulate_bpsk;

    return mod;
}

// create a qpsk (quaternary phase-shift keying) modem object
modem modem_create_qpsk()
{
    modem mod = (modem) malloc( sizeof(struct modem_s) );
    mod->scheme = LIQUID_MODEM_QPSK;

    modem_init(mod, 2);

    mod->modulate_func = &modem_modulate_qpsk;
    mod->demodulate_func = &modem_demodulate_qpsk;

    return mod;
}

// create an ook (on/off keying) modem object
modem modem_create_ook()
{
    modem mod = (modem) malloc( sizeof(struct modem_s) );
    mod->scheme = LIQUID_MODEM_OOK;

    modem_init(mod, 1);

    mod->modulate_func = &modem_modulate_ook;
    mod->demodulate_func = &modem_demodulate_ook;

    return mod;
}

// create a 'square' 32-QAM (on/off keying) modem object
modem modem_create_sqam32()
{
    modem q = (modem) malloc( sizeof(struct modem_s) );
    q->scheme = LIQUID_MODEM_SQAM32;

    modem_init(q, 5);

    // allocate memory for 8-point symbol map
    q->symbol_map = (float complex*) malloc( 8*sizeof(float complex) );
    memmove(q->symbol_map, modem_arb_sqam32, 8*sizeof(float complex));

    // set modulation, demodulation functions
    q->modulate_func = &modem_modulate_sqam32;
    q->demodulate_func = &modem_demodulate_sqam32;

    return q;
}

// create a 'square' 128-QAM (on/off keying) modem object
modem modem_create_sqam128()
{
    modem q = (modem) malloc( sizeof(struct modem_s) );
    q->scheme = LIQUID_MODEM_SQAM128;

    modem_init(q, 7);

    // allocate memory for 32-point symbol map
    q->symbol_map = (float complex*) malloc( 32*sizeof(float complex) );
    memmove(q->symbol_map, modem_arb_sqam128, 32*sizeof(float complex));

    // set modulation, demodulation functions
    q->modulate_func = &modem_modulate_sqam128;
    q->demodulate_func = &modem_demodulate_sqam128;

    return q;
}

// create a dpsk (differential phase-shift keying) modem object
modem modem_create_dpsk(unsigned int _bits_per_symbol)
{
    modem mod = (modem) malloc( sizeof(struct modem_s) );
    mod->scheme = LIQUID_MODEM_DPSK;

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

// create an apsk (amplitude/phase-shift keying) modem object
modem modem_create_apsk(unsigned int _bits_per_symbol)
{
    modem q = NULL;
    switch (_bits_per_symbol) {
    case 2: q = modem_create_apsk4();   break;
    case 3: q = modem_create_apsk8();   break;
    case 4: q = modem_create_apsk16();  break;
    case 5: q = modem_create_apsk32();  break;
    case 6: q = modem_create_apsk64();  break;
    case 7: q = modem_create_apsk128(); break;
    case 8: q = modem_create_apsk256(); break;
    default:
        fprintf(stderr,"error: modem_create_apsk(), unsupported modulation level (%u)\n",
                _bits_per_symbol);
        exit(1);
    }

    // initialize symbol map
    q->symbol_map = (float complex*)malloc(q->M*sizeof(float complex));
    modem_init_map(q);
    q->modulate_using_map = 1;

    return q;
}

// create specific APSK-4 modem
modem modem_create_apsk4()
{
    modem mod = (modem) malloc( sizeof(struct modem_s) );
    mod->scheme = LIQUID_MODEM_APSK;

    modem_init(mod, 2);
    
    // set internals
    mod->apsk_num_levels = apsk4_num_levels;
    mod->apsk_p = (unsigned int *) apsk4_p;
    mod->apsk_r = (float *) apsk4_r;
    mod->apsk_phi = (float *) apsk4_phi;
    mod->apsk_r_slicer = (float *) apsk4_r_slicer;
    mod->apsk_symbol_map = (unsigned int *) apsk4_symbol_map;

    mod->modulate_func = &modem_modulate_apsk;
    mod->demodulate_func = &modem_demodulate_apsk;

    mod->demod_soft_neighbors = (unsigned char*) apsk4_demod_soft_neighbors;
    mod->demod_soft_p         = 3;

    return mod;
}

// create specific APSK-8 modem
modem modem_create_apsk8()
{
    modem mod = (modem) malloc( sizeof(struct modem_s) );
    mod->scheme = LIQUID_MODEM_APSK;

    modem_init(mod, 3);
    
    // set internals
    mod->apsk_num_levels = apsk8_num_levels;
    mod->apsk_p = (unsigned int *) apsk8_p;
    mod->apsk_r = (float *) apsk8_r;
    mod->apsk_phi = (float *) apsk8_phi;
    mod->apsk_r_slicer = (float *) apsk8_r_slicer;
    mod->apsk_symbol_map = (unsigned int *) apsk8_symbol_map;

    mod->modulate_func = &modem_modulate_apsk;
    mod->demodulate_func = &modem_demodulate_apsk;

    mod->demod_soft_neighbors = (unsigned char*) apsk8_demod_soft_neighbors;
    mod->demod_soft_p         = 3;

    return mod;
}

// create specific APSK-16 modem
modem modem_create_apsk16()
{
    modem mod = (modem) malloc( sizeof(struct modem_s) );
    mod->scheme = LIQUID_MODEM_APSK;

    modem_init(mod, 4);
    
    // set internals
    mod->apsk_num_levels = apsk16_num_levels;
    mod->apsk_p = (unsigned int *) apsk16_p;
    mod->apsk_r = (float *) apsk16_r;
    mod->apsk_phi = (float *) apsk16_phi;
    mod->apsk_r_slicer = (float *) apsk16_r_slicer;
    mod->apsk_symbol_map = (unsigned int *) apsk16_symbol_map;

    mod->modulate_func = &modem_modulate_apsk;
    mod->demodulate_func = &modem_demodulate_apsk;

    mod->demod_soft_neighbors = (unsigned char*) apsk16_demod_soft_neighbors;
    mod->demod_soft_p         = 4;

    return mod;
}

// create specific APSK-32 modem
modem modem_create_apsk32()
{
    modem mod = (modem) malloc( sizeof(struct modem_s) );
    mod->scheme = LIQUID_MODEM_APSK;

    modem_init(mod, 5);
    
    // set internals
    mod->apsk_num_levels = apsk32_num_levels;
    mod->apsk_p = (unsigned int *) apsk32_p;
    mod->apsk_r = (float *) apsk32_r;
    mod->apsk_phi = (float *) apsk32_phi;
    mod->apsk_r_slicer = (float *) apsk32_r_slicer;
    mod->apsk_symbol_map = (unsigned int *) apsk32_symbol_map;

    mod->modulate_func = &modem_modulate_apsk;
    mod->demodulate_func = &modem_demodulate_apsk;

    mod->demod_soft_neighbors = (unsigned char*) apsk32_demod_soft_neighbors;
    mod->demod_soft_p         = 4;

    return mod;
}

// create specific APSK-64 modem
modem modem_create_apsk64()
{
    modem mod = (modem) malloc( sizeof(struct modem_s) );
    mod->scheme = LIQUID_MODEM_APSK;

    modem_init(mod, 6);
    
    // set internals
    mod->apsk_num_levels = apsk64_num_levels;
    mod->apsk_p = (unsigned int *) apsk64_p;
    mod->apsk_r = (float *) apsk64_r;
    mod->apsk_phi = (float *) apsk64_phi;
    mod->apsk_r_slicer = (float *) apsk64_r_slicer;
    mod->apsk_symbol_map = (unsigned int *) apsk64_symbol_map;

    mod->modulate_func = &modem_modulate_apsk;
    mod->demodulate_func = &modem_demodulate_apsk;

    mod->demod_soft_neighbors = (unsigned char*) apsk64_demod_soft_neighbors;
    mod->demod_soft_p         = 4;

    return mod;
}

// create specific APSK-128 modem
modem modem_create_apsk128()
{
    modem mod = (modem) malloc( sizeof(struct modem_s) );
    mod->scheme = LIQUID_MODEM_APSK;

    modem_init(mod, 7);
    
    // set internals
    mod->apsk_num_levels = apsk128_num_levels;
    mod->apsk_p = (unsigned int *) apsk128_p;
    mod->apsk_r = (float *) apsk128_r;
    mod->apsk_phi = (float *) apsk128_phi;
    mod->apsk_r_slicer = (float *) apsk128_r_slicer;
    mod->apsk_symbol_map = (unsigned int *) apsk128_symbol_map;

    mod->modulate_func = &modem_modulate_apsk;
    mod->demodulate_func = &modem_demodulate_apsk;

    mod->demod_soft_neighbors = (unsigned char*) apsk128_demod_soft_neighbors;
    mod->demod_soft_p         = 5;

    return mod;
}

// create specific APSK-256 modem
modem modem_create_apsk256()
{
    modem mod = (modem) malloc( sizeof(struct modem_s) );
    mod->scheme = LIQUID_MODEM_APSK;

    modem_init(mod, 8);
    
    // set internals
    mod->apsk_num_levels = apsk256_num_levels;
    mod->apsk_p = (unsigned int *) apsk256_p;
    mod->apsk_r = (float *) apsk256_r;
    mod->apsk_phi = (float *) apsk256_phi;
    mod->apsk_r_slicer = (float *) apsk256_r_slicer;
    mod->apsk_symbol_map = (unsigned int *) apsk256_symbol_map;

    mod->modulate_func = &modem_modulate_apsk;
    mod->demodulate_func = &modem_demodulate_apsk;

    mod->demod_soft_neighbors = (unsigned char*) apsk256_demod_soft_neighbors;
    mod->demod_soft_p         = 5;

    return mod;
}

// create an arbitrary modem object
modem modem_create_arb(unsigned int _bits_per_symbol)
{
    modem mod = (modem) malloc( sizeof(struct modem_s) );
    mod->scheme = LIQUID_MODEM_ARB;

    modem_init(mod, _bits_per_symbol);

    mod->M = mod->M;
    mod->symbol_map = (float complex*) calloc( mod->M, sizeof(float complex) );

    mod->modulate_func = &modem_modulate_arb;
    mod->demodulate_func = &modem_demodulate_arb;

    return mod;
}

// create a V.29 modem object (4 bits/symbol)
modem modem_create_V29()
{
    modem mod = modem_create_arb(4);
    modem_arb_init(mod,(float complex*)modem_arb_V29,16);
    return mod;
}

// create an arb16opt (optimal 16-qam) modem object
modem modem_create_arb16opt()
{
    modem mod = modem_create_arb(4);
    modem_arb_init(mod,(float complex*)modem_arb16opt,16);
    return mod;
}

// create an arb32opt (optimal 32-qam) modem object
modem modem_create_arb32opt()
{
    modem mod = modem_create_arb(5);
    modem_arb_init(mod,(float complex*)modem_arb32opt,32);
    return mod;
}

// create an arb64opt (optimal 64-qam) modem object
modem modem_create_arb64opt()
{
    modem mod = modem_create_arb(6);
    modem_arb_init(mod,(float complex*)modem_arb64opt,64);
    return mod;
}

// create an arb128opt (optimal 128-qam) modem object
modem modem_create_arb128opt()
{
    modem mod = modem_create_arb(7);
    modem_arb_init(mod,(float complex*)modem_arb128opt,128);
    return mod;
}

// create an arb256opt (optimal 256-qam) modem object
modem modem_create_arb256opt()
{
    modem mod = modem_create_arb(8);
    modem_arb_init(mod,(float complex*)modem_arb256opt,256);
    return mod;
}

// create an arb64vt (64-qam vt logo) modem object
modem modem_create_arb64vt()
{
    modem mod = modem_create_arb(6);
    modem_arb_init(mod,(float complex*)modem_arb_vt64,64);
    return mod;
}

// initialize an arbitrary modem object
//  _mod        :   modem object
//  _symbol_map :   arbitrary modem symbol map
//  _len        :   number of symbols in the map
void modem_arb_init(modem _mod,
                    float complex *_symbol_map,
                    unsigned int _len)
{
#ifdef LIQUID_VALIDATE_INPUT
    if (_mod->scheme != LIQUID_MODEM_ARB) {
        fprintf(stderr,"error: modem_arb_init(), modem is not of arbitrary type\n");
        exit(1);
    } else if (_len != _mod->M) {
        fprintf(stderr,"error: modem_arb_init(), array sizes do not match\n");
        exit(1);
    }
#endif

    unsigned int i;
    for (i=0; i<_len; i++)
        _mod->symbol_map[i] = _symbol_map[i];

    // balance I/Q channels
    if (_mod->scheme == LIQUID_MODEM_ARB)
        modem_arb_balance_iq(_mod);

    // scale modem to have unity energy
    modem_arb_scale(_mod);

}

// initialize an arbitrary modem object on a file
//  _mod        :   modem object
//  _filename   :   name of the data file
void modem_arb_init_file(modem _mod,
                         char * _filename)
{
    // try to open file
    FILE * fid = fopen(_filename, "r");
    if (fid == NULL) {
        fprintf(stderr,"error: modem_arb_init_file(), could not open file\n");
        exit(1);
    }

    unsigned int i, results;
    float sym_i, sym_q;
    for (i=0; i<_mod->M; i++) {
        if ( feof(fid) ) {
            fprintf(stderr,"error: modem_arb_init_file(), premature EOF for '%s'\n", _filename);
            exit(1);
        }

        results = fscanf(fid, "%f %f\n", &sym_i, &sym_q);
        _mod->symbol_map[i] = sym_i + _Complex_I*sym_q;

        // ensure proper number of symbols were read
        if (results < 2) {
            fprintf(stderr,"error: modem_arb_init_file(), unable to parse line\n");
            exit(1);
        }
    }

    fclose(fid);

    // balance I/Q channels
    if (_mod->scheme == LIQUID_MODEM_ARB)
        modem_arb_balance_iq(_mod);

    // scale modem to have unity energy
    modem_arb_scale(_mod);
}

// scale arbitrary modem constellation points
void modem_arb_scale(modem _mod)
{
    unsigned int i;

    // calculate energy
    float mag, e = 0.0f;
    for (i=0; i<_mod->M; i++) {
        mag = cabsf(_mod->symbol_map[i]);
        e += mag*mag;
    }

    e = sqrtf( e / _mod->M );

    for (i=0; i<_mod->M; i++) {
        _mod->symbol_map[i] /= e;
    }
}

// balance an arbitrary modem's I/Q points
void modem_arb_balance_iq(modem _mod)
{
    float complex mean=0.0f;
    unsigned int i;

    // accumulate average signal
    for (i=0; i<_mod->M; i++) {
        mean += _mod->symbol_map[i];
    }
    mean /= (float) (_mod->M);

    // subtract mean value from reference levels
    for (i=0; i<_mod->M; i++) {
        _mod->symbol_map[i] -= mean;
    }
}

