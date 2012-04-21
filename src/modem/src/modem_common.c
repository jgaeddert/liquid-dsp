/*
 * Copyright (c) 2007, 2008, 2009, 2010, 2012 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010, 2012 Virginia Polytechnic
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
// modem_common.c : common utilities specific to precision
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "liquid.internal.h"

// modem structure used for both modulation and demodulation 
//
// The modem structure implements a variety of common modulation schemes,
// including (differential) phase-shift keying, and (quadrature) amplitude
// modulation.
//
// While the same modem structure may be used for both modulation and
// demodulation for most schemes, it is important to use separate objects
// for differential-mode modems (e.g. LIQUID_MODEM_DPSK) as the internal state
// will change after each symbol.  It is usually good practice to keep
// separate instances of modulators and demodulators.
struct modem_s {
    modulation_scheme scheme;       // modulation scheme

    unsigned int m;                 // bits per symbol (modulation depth)
    unsigned int M;                 // constellation size, M=2^m

    float alpha;                    // scaling factor to ensure unity energy

    // Reference vector for demodulating linear arrays
    //
    // By storing these values in an array they do not need to be
    // calculated during run-time.  This speeds up the demodulation by
    // approximately 8%.
    float ref[MAX_MOD_BITS_PER_SYMBOL];

    // modulation
    float complex * symbol_map;     // complete symbol map
    int modulate_using_map;         // modulate using map (look-up table) flag

    // demodulation
    float complex r;                // received state vector
    float complex x_hat;            // estimated symbol (demodulator)

    // QAM modem
    unsigned int m_i;               // bits per symbol, in-phase
    unsigned int M_i;               // in-phase dimension, M_i=2^{m_i}
    unsigned int m_q;               // bits per symbol, quadrature
    unsigned int M_q;               // quadrature dimension, M_q=2^{m_q}

    // PSK/DPSK modem
    float d_phi;                    // half of phase between symbols
    float dpsk_phi;                 // angle state for differential PSK

    // APSK modem
    unsigned int apsk_num_levels;   // number of levels
    unsigned int * apsk_p;          // number of symbols per level
    float * apsk_r;                 // radii of levels
    float * apsk_r_slicer;          // slicer radii of levels
    float * apsk_phi;               // phase offset of levels
    unsigned int * apsk_symbol_map; // symbol mapping

    // modulate function pointer
    void (*modulate_func)(modem _mod,
                          unsigned int _symbol_in,
                          float complex * _y);

    // demodulate function pointer
    void (*demodulate_func)(modem _demod,
                            float complex _x,
                            unsigned int * _symbol_out);

    // soft demodulation
    //int demodulate_soft;    // soft demodulation flag
    // neighbors array
    unsigned char * demod_soft_neighbors;   // array of nearest neighbors
    unsigned int demod_soft_p;              // number of neighbors in array
};

// create digital modem of a specific scheme and bits/symbol
modem modem_create(modulation_scheme _scheme)
{
    switch (_scheme) {
    
    // Phase-shift keying (PSK)
    case LIQUID_MODEM_PSK2:     return modem_create_psk(1);
    case LIQUID_MODEM_PSK4:     return modem_create_psk(2);
    case LIQUID_MODEM_PSK8:     return modem_create_psk(3);
    case LIQUID_MODEM_PSK16:    return modem_create_psk(4);
    case LIQUID_MODEM_PSK32:    return modem_create_psk(5);
    case LIQUID_MODEM_PSK64:    return modem_create_psk(6);
    case LIQUID_MODEM_PSK128:   return modem_create_psk(7);
    case LIQUID_MODEM_PSK256:   return modem_create_psk(8);

    // Differential phase-shift keying (DPSK)
    case LIQUID_MODEM_DPSK2:    return modem_create_dpsk(1);
    case LIQUID_MODEM_DPSK4:    return modem_create_dpsk(2);
    case LIQUID_MODEM_DPSK8:    return modem_create_dpsk(3);
    case LIQUID_MODEM_DPSK16:   return modem_create_dpsk(4);
    case LIQUID_MODEM_DPSK32:   return modem_create_dpsk(5);
    case LIQUID_MODEM_DPSK64:   return modem_create_dpsk(6);
    case LIQUID_MODEM_DPSK128:  return modem_create_dpsk(7);
    case LIQUID_MODEM_DPSK256:  return modem_create_dpsk(8);

    // amplitude-shift keying (ASK)
    case LIQUID_MODEM_ASK2:     return modem_create_ask(1);
    case LIQUID_MODEM_ASK4:     return modem_create_ask(2);
    case LIQUID_MODEM_ASK8:     return modem_create_ask(3);
    case LIQUID_MODEM_ASK16:    return modem_create_ask(4);
    case LIQUID_MODEM_ASK32:    return modem_create_ask(5);
    case LIQUID_MODEM_ASK64:    return modem_create_ask(6);
    case LIQUID_MODEM_ASK128:   return modem_create_ask(7);
    case LIQUID_MODEM_ASK256:   return modem_create_ask(8);

    // rectangular quadrature amplitude-shift keying (QAM)
    case LIQUID_MODEM_QAM4:     return modem_create_qam(2);
    case LIQUID_MODEM_QAM8:     return modem_create_qam(3);
    case LIQUID_MODEM_QAM16:    return modem_create_qam(4);
    case LIQUID_MODEM_QAM32:    return modem_create_qam(5);
    case LIQUID_MODEM_QAM64:    return modem_create_qam(6);
    case LIQUID_MODEM_QAM128:   return modem_create_qam(7);
    case LIQUID_MODEM_QAM256:   return modem_create_qam(8);

    // amplitude phase-shift keying (APSK)
    case LIQUID_MODEM_APSK4:    return modem_create_apsk(2);
    case LIQUID_MODEM_APSK8:    return modem_create_apsk(3);
    case LIQUID_MODEM_APSK16:   return modem_create_apsk(4);
    case LIQUID_MODEM_APSK32:   return modem_create_apsk(5);
    case LIQUID_MODEM_APSK64:   return modem_create_apsk(6);
    case LIQUID_MODEM_APSK128:  return modem_create_apsk(7);
    case LIQUID_MODEM_APSK256:  return modem_create_apsk(8);

    // specific modems
    case LIQUID_MODEM_BPSK:      return modem_create_bpsk();
    case LIQUID_MODEM_QPSK:      return modem_create_qpsk();
    case LIQUID_MODEM_OOK:       return modem_create_ook();
    case LIQUID_MODEM_SQAM32:    return modem_create_sqam32();
    case LIQUID_MODEM_SQAM128:   return modem_create_sqam128();
    case LIQUID_MODEM_V29:       return modem_create_V29();
    case LIQUID_MODEM_ARB16OPT:  return modem_create_arb16opt();
    case LIQUID_MODEM_ARB32OPT:  return modem_create_arb32opt();
    case LIQUID_MODEM_ARB64OPT:  return modem_create_arb64opt();
    case LIQUID_MODEM_ARB128OPT: return modem_create_arb128opt();
    case LIQUID_MODEM_ARB256OPT: return modem_create_arb256opt();
    case LIQUID_MODEM_ARB64VT:   return modem_create_arb64vt();
    
    // arbitrary modem
    case LIQUID_MODEM_ARB:
        fprintf(stderr,"error: modem_create(), cannot create arbitrary modem (LIQUID_MODEM_ARB) without specifying constellation\n");
        exit(1);

    // unknown modulation scheme
    default:
        fprintf(stderr,"error: modem_create(), unknown/unsupported modulation scheme : %u\n", _scheme);
        exit(1);
    }

    // should never get to this point, but adding return statment
    // to keep compiler happy
    return NULL;
}

// recreate modulation scheme, re-allocating memory as necessary
modem modem_recreate(modem _q,
                     modulation_scheme _scheme)
{
    // TODO : regenerate modem only when truly necessary
    if (_q->scheme != _scheme) {
        // destroy and re-create modem
        modem_destroy(_q);
        _q = modem_create(_scheme);
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
    printf("    scheme:         %s\n", modulation_types[_mod->scheme].name);
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

// Generate random symbol
unsigned int modem_gen_rand_sym(modem _mod)
{
    return rand() % (_mod->M);
}

// Get modem depth (bits/symbol)
unsigned int modem_get_bps(modem _mod)
{
    return _mod->m;
}

// generic modulatio function
//  _q          :   modem object
//  _symbol_in  :   input symbol
//  _y          :   output sample
void modem_modulate(modem _q,
                    unsigned int _symbol_in,
                    float complex * _y)
{
    // validate input
    if (_symbol_in >= _q->M) {
        fprintf(stderr,"error: modem_modulate(), input symbol exceeds constellation size\n");
        exit(1);
    }

    if (_q->modulate_using_map) {
        // modulate simply using map (look-up table)
        modem_modulate_map(_q, _symbol_in, _y);
    } else {
        // invoke method specific to scheme (calculate symbol on the fly)
        _q->modulate_func(_q, _symbol_in, _y);
    }
}

// modulate using symbol map (look-up table)
void modem_modulate_map(modem _q,
                        unsigned int _symbol_in,
                        float complex * _y)
{
    if (_symbol_in >= _q->M) {
        fprintf(stderr,"error: modem_modulate_table(), input symbol exceeds maximum\n");
        exit(1);
    } else if (_q == NULL) {
        fprintf(stderr,"error: modem_modulate_table(), symbol table not initialized\n");
        exit(1);
    }

    // map sample directly to output
    *_y = _q->symbol_map[_symbol_in]; 
}

// generic demodulation
void modem_demodulate(modem _demod,
                      float complex x,
                      unsigned int *symbol_out)
{
    // invoke method specific to scheme (calculate symbol on the fly)
    _demod->demodulate_func(_demod, x, symbol_out);
}

// generic soft demodulation
void modem_demodulate_soft(modem _demod,
                           float complex _x,
                           unsigned int  * _s,
                           unsigned char * _soft_bits)
{
    // switch scheme
    switch (_demod->scheme) {
    case LIQUID_MODEM_ARB:  modem_demodulate_soft_arb( _demod,_x,_s,_soft_bits); return;
    case LIQUID_MODEM_BPSK: modem_demodulate_soft_bpsk(_demod,_x,_s,_soft_bits); return;
    case LIQUID_MODEM_QPSK: modem_demodulate_soft_qpsk(_demod,_x,_s,_soft_bits); return;
    default:;
    }

    // check if...
    if (_demod->demod_soft_neighbors != NULL && _demod->demod_soft_p != 0) {
        // demodulate using approximate log-likelihood method with
        // look-up table for nearest neighbors
        modem_demodulate_soft_table(_demod, _x, _s, _soft_bits);

        return;
    }

    // for now demodulate normally and simply copy the
    // hard-demodulated bits
    unsigned int symbol_out;
    _demod->demodulate_func(_demod, _x, &symbol_out);
    *_s = symbol_out;

    // unpack soft bits
    liquid_unpack_soft_bits(symbol_out, _demod->m, _soft_bits);
}

#if DEBUG_DEMODULATE_SOFT
// print a string of bits to the standard output
void print_bitstring_demod_soft(unsigned int _x,
                                unsigned int _n)
{
    unsigned int i;
    for (i=0; i<_n; i++)
        printf("%1u", (_x >> (_n-i-1)) & 1);
}
#endif

// generic soft demodulation using look-up table...
//  _demod      :   demodulator object
//  _r          :   received sample
//  _s          :   hard demodulator output
//  _soft_bits  :   soft bit ouput (approximate log-likelihood ratio)
void modem_demodulate_soft_table(modem _demod,
                                 float complex _r,
                                 unsigned int * _s,
                                 unsigned char * _soft_bits)
{
    // run hard demodulation; this will store re-modulated sample
    // as internal variable x_hat
    unsigned int s;
    modem_demodulate(_demod, _r, &s);

    unsigned int bps = modem_get_bps(_demod);

    // gamma = 1/(2*sigma^2), approximate for constellation size
    float gamma = 1.2f*_demod->M;

    // set and initialize minimum bit values
    unsigned int i;
    unsigned int k;
    float dmin_0[bps];
    float dmin_1[bps];
    for (k=0; k<bps; k++) {
        dmin_0[k] = 8.0f;
        dmin_1[k] = 8.0f;
    }

    unsigned int bit;
    float d;
    float complex x_hat;    // re-modulated symbol
    unsigned char * softab = _demod->demod_soft_neighbors;
    unsigned int p = _demod->demod_soft_p;

    // check hard demodulation
    d = crealf( (_r-_demod->x_hat)*conjf(_r-_demod->x_hat) );
    for (k=0; k<bps; k++) {
        bit = (s >> (bps-k-1)) & 0x01;
        if (bit) dmin_1[k] = d;
        else     dmin_0[k] = d;
    }

    // parse all 'nearest neighbors' and find minimum distance for each bit
    for (i=0; i<p; i++) {
        // remodulate symbol
        if (_demod->modulate_using_map)
            x_hat = _demod->symbol_map[ softab[s*p + i] ];
        else
            modem_modulate(_demod, softab[s*p+i], &x_hat);

        // compute magnitude squared of Euclidean distance
        //d = crealf( (_r-x_hat)*conjf(_r-x_hat) );
        // (same as above, but faster)
        float complex e = _r - x_hat;
        d = crealf(e)*crealf(e) + cimagf(e)*cimagf(e);

        // look at each bit in 'nearest neighbor' and update minimum
        for (k=0; k<bps; k++) {
            // strip bit
            unsigned int bit = (softab[s*p+i] >> (bps-k-1)) & 0x01;
            if ( bit ) {
                if (d < dmin_1[k]) dmin_1[k] = d;
            } else {
                if (d < dmin_0[k]) dmin_0[k] = d;
            }
        }
    }

    // make soft bit assignments
    for (k=0; k<bps; k++) {
        int soft_bit = ((dmin_0[k] - dmin_1[k])*gamma)*16 + 127;
        if (soft_bit > 255) soft_bit = 255;
        if (soft_bit <   0) soft_bit = 0;
        _soft_bits[k] = (unsigned char)soft_bit;
    }

    // set hard output symbol
    *_s = s;
}



// get demodulator's estimated transmit sample
void modem_get_demodulator_sample(modem _demod,
                                  float complex * _x_hat)
{
    *_x_hat = _demod->x_hat;
}

// get demodulator phase error
float modem_get_demodulator_phase_error(modem _demod)
{
    return cimagf(_demod->r*conjf(_demod->x_hat));
}

// get error vector magnitude
float modem_get_demodulator_evm(modem _demod)
{
    return cabsf(_demod->x_hat - _demod->r);
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


