/*
 * Copyright (c) 2007 - 2019 Joseph Gaeddert
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
// modem_common.c : common utilities specific to precision
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "liquid.internal.h"

#define DEBUG_DEMODULATE_SOFT 0

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
struct MODEM(_s)
{
    // common data
    modulation_scheme scheme;   // modulation scheme
    unsigned int m;             // bits per symbol (modulation depth)
    unsigned int M;             // constellation size, M=2^m

    // Reference vector for demodulating linear arrays
    //
    // By storing these values in an array they do not need to be
    // calculated during run-time.  This speeds up the demodulation by
    // approximately 8%.
    T ref[MAX_MOD_BITS_PER_SYMBOL];

    // modulation
    TC * symbol_map;     // complete symbol map
    int modulate_using_map;         // modulate using map (look-up table) flag

    // demodulation
    TC r;                // received state vector
    TC x_hat;            // estimated symbol (demodulator)

    // common data structure shared between specific modem types
    union {
        // PSK modem
        struct {
            T d_phi;            // half of phase between symbols
            T alpha;            // scaling factor for phase symbols
        } psk;

        // DPSK modem
        struct {
            T d_phi;            // half of phase between symbols
            T phi;              // angle state for differential PSK
            T alpha;            // scaling factor for phase symbols
        } dpsk;

        // ASK modem
        struct {
            T alpha;            // scaling factor to ensure unity energy
        } ask;

        // QAM modem
        struct {
            unsigned int m_i;   // bits per symbol, in-phase
            unsigned int m_q;   // bits per symbol, quadrature
            unsigned int M_i;   // in-phase dimension, M_i=2^{m_i}
            unsigned int M_q;   // quadrature dimension, M_q=2^{m_q}
            T alpha;            // scaling factor to ensure unity energy
        } qam;

        // APSK modem
        struct {
            unsigned int num_levels;    // number of levels
            unsigned int p[8];          // number of symbols per level
            T r[8];                     // radii of levels
            T r_slicer[8];              // slicer radii of levels
            T phi[8];                   // phase offset of levels
            unsigned char * map;        // symbol mapping (allocated)
        } apsk;

        // 'square' 32-QAM
        struct {
            TC * map;           // 8-sample sub-map (first quadrant)
        } sqam32;

        // 'square' 128-QAM
        struct {
            TC * map;           // 32-sample sub-map (first quadrant)
        } sqam128;
    } data;

    // modulate function pointer
    void (*modulate_func)(MODEM() _q,
                          unsigned int _symbol_in,
                          TC * _y);

    // demodulate function pointer
    void (*demodulate_func)(MODEM() _q,
                            TC _x,
                            unsigned int * _symbol_out);

    // soft demodulation
    //int demodulate_soft;    // soft demodulation flag
    // neighbors array
    unsigned char * demod_soft_neighbors;   // array of nearest neighbors
    unsigned int demod_soft_p;              // number of neighbors in array
};

// create digital modem of a specific scheme and bits/symbol
MODEM() MODEM(_create)(modulation_scheme _scheme)
{
    switch (_scheme) {
    
    // Phase-shift keying (PSK)
    case LIQUID_MODEM_PSK2:     return MODEM(_create_psk)(1);
    case LIQUID_MODEM_PSK4:     return MODEM(_create_psk)(2);
    case LIQUID_MODEM_PSK8:     return MODEM(_create_psk)(3);
    case LIQUID_MODEM_PSK16:    return MODEM(_create_psk)(4);
    case LIQUID_MODEM_PSK32:    return MODEM(_create_psk)(5);
    case LIQUID_MODEM_PSK64:    return MODEM(_create_psk)(6);
    case LIQUID_MODEM_PSK128:   return MODEM(_create_psk)(7);
    case LIQUID_MODEM_PSK256:   return MODEM(_create_psk)(8);

    // Differential phase-shift keying (DPSK)
    case LIQUID_MODEM_DPSK2:    return MODEM(_create_dpsk)(1);
    case LIQUID_MODEM_DPSK4:    return MODEM(_create_dpsk)(2);
    case LIQUID_MODEM_DPSK8:    return MODEM(_create_dpsk)(3);
    case LIQUID_MODEM_DPSK16:   return MODEM(_create_dpsk)(4);
    case LIQUID_MODEM_DPSK32:   return MODEM(_create_dpsk)(5);
    case LIQUID_MODEM_DPSK64:   return MODEM(_create_dpsk)(6);
    case LIQUID_MODEM_DPSK128:  return MODEM(_create_dpsk)(7);
    case LIQUID_MODEM_DPSK256:  return MODEM(_create_dpsk)(8);

    // amplitude-shift keying (ASK)
    case LIQUID_MODEM_ASK2:     return MODEM(_create_ask)(1);
    case LIQUID_MODEM_ASK4:     return MODEM(_create_ask)(2);
    case LIQUID_MODEM_ASK8:     return MODEM(_create_ask)(3);
    case LIQUID_MODEM_ASK16:    return MODEM(_create_ask)(4);
    case LIQUID_MODEM_ASK32:    return MODEM(_create_ask)(5);
    case LIQUID_MODEM_ASK64:    return MODEM(_create_ask)(6);
    case LIQUID_MODEM_ASK128:   return MODEM(_create_ask)(7);
    case LIQUID_MODEM_ASK256:   return MODEM(_create_ask)(8);

    // rectangular quadrature amplitude-shift keying (QAM)
    case LIQUID_MODEM_QAM4:     return MODEM(_create_qam)(2);
    case LIQUID_MODEM_QAM8:     return MODEM(_create_qam)(3);
    case LIQUID_MODEM_QAM16:    return MODEM(_create_qam)(4);
    case LIQUID_MODEM_QAM32:    return MODEM(_create_qam)(5);
    case LIQUID_MODEM_QAM64:    return MODEM(_create_qam)(6);
    case LIQUID_MODEM_QAM128:   return MODEM(_create_qam)(7);
    case LIQUID_MODEM_QAM256:   return MODEM(_create_qam)(8);
    case LIQUID_MODEM_QAM512:   return MODEM(_create_qam)(9);
    case LIQUID_MODEM_QAM1024:  return MODEM(_create_qam)(10);
    case LIQUID_MODEM_QAM2048:  return MODEM(_create_qam)(11);
    case LIQUID_MODEM_QAM4096:  return MODEM(_create_qam)(12);
    case LIQUID_MODEM_QAM8192:  return MODEM(_create_qam)(13);
    case LIQUID_MODEM_QAM16384: return MODEM(_create_qam)(14);
    case LIQUID_MODEM_QAM32768: return MODEM(_create_qam)(15);
    case LIQUID_MODEM_QAM65536: return MODEM(_create_qam)(16);

    // amplitude phase-shift keying (APSK)
    case LIQUID_MODEM_APSK4:    return MODEM(_create_apsk)(2);
    case LIQUID_MODEM_APSK8:    return MODEM(_create_apsk)(3);
    case LIQUID_MODEM_APSK16:   return MODEM(_create_apsk)(4);
    case LIQUID_MODEM_APSK32:   return MODEM(_create_apsk)(5);
    case LIQUID_MODEM_APSK64:   return MODEM(_create_apsk)(6);
    case LIQUID_MODEM_APSK128:  return MODEM(_create_apsk)(7);
    case LIQUID_MODEM_APSK256:  return MODEM(_create_apsk)(8);

    // specific modems
    case LIQUID_MODEM_BPSK:      return MODEM(_create_bpsk)();
    case LIQUID_MODEM_QPSK:      return MODEM(_create_qpsk)();
    case LIQUID_MODEM_OOK:       return MODEM(_create_ook)();
    case LIQUID_MODEM_SQAM32:    return MODEM(_create_sqam32)();
    case LIQUID_MODEM_SQAM128:   return MODEM(_create_sqam128)();
    case LIQUID_MODEM_V29:       return MODEM(_create_V29)();
    case LIQUID_MODEM_ARB16OPT:  return MODEM(_create_arb16opt)();
    case LIQUID_MODEM_ARB32OPT:  return MODEM(_create_arb32opt)();
    case LIQUID_MODEM_ARB64OPT:  return MODEM(_create_arb64opt)();
    case LIQUID_MODEM_ARB128OPT: return MODEM(_create_arb128opt)();
    case LIQUID_MODEM_ARB256OPT: return MODEM(_create_arb256opt)();
    case LIQUID_MODEM_ARB64VT:   return MODEM(_create_arb64vt)();
    
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
MODEM() MODEM(_recreate)(MODEM() _q,
                         modulation_scheme _scheme)
{
    // TODO : regenerate modem only when truly necessary
    if (_q->scheme != _scheme) {
        // destroy and re-create modem
        MODEM(_destroy)(_q);
        _q = MODEM(_create)(_scheme);
    }

    // return object
    return _q;
}

// destroy a modem object
void MODEM(_destroy)(MODEM() _q)
{
    // free symbol map
    if (_q->symbol_map != NULL)
        free(_q->symbol_map);

    // free soft-demodulation neighbors table
    if (_q->demod_soft_neighbors != NULL)
        free(_q->demod_soft_neighbors);

    // free memory in specific data types
    if (_q->scheme == LIQUID_MODEM_SQAM32) {
        free(_q->data.sqam32.map);
    } else if (_q->scheme == LIQUID_MODEM_SQAM128) {
        free(_q->data.sqam128.map);
    } else if (liquid_modem_is_apsk(_q->scheme)) {
        free(_q->data.apsk.map);
    }

    // free main object memory
    free(_q);
}

// print a modem object
void MODEM(_print)(MODEM() _q)
{
    printf("linear modem:\n");
    printf("    scheme:         %s\n", modulation_types[_q->scheme].name);
    printf("    bits/symbol:    %u\n", _q->m);
}

// reset a modem object (only an issue with dpsk)
void MODEM(_reset)(MODEM() _q)
{
    _q->r = 1.0f;         // received sample
    _q->x_hat = _q->r;  // estimated symbol

    if ( liquid_modem_is_dpsk(_q->scheme) )
        _q->data.dpsk.phi = 0.0f;  // reset differential PSK phase state
}

// initialize a generic modem object
void MODEM(_init)(MODEM() _q,
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
    _q->symbol_map = NULL;    // symbol map (LIQUID_MODEM_ARB only)
    _q->modulate_using_map=0; // modulate using map flag

    // common data
    _q->m = _bits_per_symbol; // bits/symbol
    _q->M = 1 << (_q->m);   // constellation size (2^m)

    // set function pointers initially to NULL
    _q->modulate_func = NULL;
    _q->demodulate_func = NULL;

    // soft demodulation
    _q->demod_soft_neighbors = NULL;
    _q->demod_soft_p = 0;
}

// initialize symbol map for fast modulation
void MODEM(_init_map)(MODEM() _q)
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
unsigned int MODEM(_gen_rand_sym)(MODEM() _q)
{
    return rand() % (_q->M);
}

// Get modem depth (bits/symbol)
unsigned int MODEM(_get_bps)(MODEM() _q)
{
    return _q->m;
}

// get modulation scheme
modulation_scheme MODEM(_get_scheme)(MODEM() _q)
{
    return _q->scheme;
}

// generic modulatio function
//  _q          :   modem object
//  _symbol_in  :   input symbol
//  _y          :   output sample
void MODEM(_modulate)(MODEM() _q,
                      unsigned int _symbol_in,
                      TC * _y)
{
    // validate input
    if (_symbol_in >= _q->M) {
        fprintf(stderr,"error: modem_modulate(), input symbol exceeds constellation size\n");
        exit(1);
    }

    if (_q->modulate_using_map) {
        // modulate simply using map (look-up table)
        MODEM(_modulate_map)(_q, _symbol_in, _y);
    } else {
        // invoke method specific to scheme (calculate symbol on the fly)
        _q->modulate_func(_q, _symbol_in, _y);
    }
}

// modulate using symbol map (look-up table)
void MODEM(_modulate_map)(MODEM() _q,
                          unsigned int _symbol_in,
                          TC * _y)
{
    if (_symbol_in >= _q->M) {
        fprintf(stderr,"error: modem_modulate_table(), input symbol exceeds maximum\n");
        exit(1);
    } else if (_q->symbol_map == NULL) {
        fprintf(stderr,"error: modem_modulate_table(), symbol table not initialized\n");
        exit(1);
    }

    // map sample directly to output
    *_y = _q->symbol_map[_symbol_in]; 
}

// generic demodulation
void MODEM(_demodulate)(MODEM() _q,
                        TC x,
                        unsigned int *symbol_out)
{
    // invoke method specific to scheme (calculate symbol on the fly)
    _q->demodulate_func(_q, x, symbol_out);
}

// generic soft demodulation
void MODEM(_demodulate_soft)(MODEM() _q,
                             TC _x,
                             unsigned int  * _s,
                             unsigned char * _soft_bits)
{
    // switch scheme
    switch (_q->scheme) {
    case LIQUID_MODEM_ARB:  MODEM(_demodulate_soft_arb)( _q,_x,_s,_soft_bits); return;
    case LIQUID_MODEM_BPSK: MODEM(_demodulate_soft_bpsk)(_q,_x,_s,_soft_bits); return;
    case LIQUID_MODEM_QPSK: MODEM(_demodulate_soft_qpsk)(_q,_x,_s,_soft_bits); return;
    default:;
    }

    // check if...
    if (_q->demod_soft_neighbors != NULL && _q->demod_soft_p != 0) {
        // demodulate using approximate log-likelihood method with
        // look-up table for nearest neighbors
        MODEM(_demodulate_soft_table)(_q, _x, _s, _soft_bits);

        return;
    }

    // for now demodulate normally and simply copy the
    // hard-demodulated bits
    unsigned int symbol_out;
    _q->demodulate_func(_q, _x, &symbol_out);
    *_s = symbol_out;

    // unpack soft bits
    liquid_unpack_soft_bits(symbol_out, _q->m, _soft_bits);
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
//  _q          :   demodulator object
//  _r          :   received sample
//  _s          :   hard demodulator output
//  _soft_bits  :   soft bit ouput (approximate log-likelihood ratio)
void MODEM(_demodulate_soft_table)(MODEM() _q,
                                   TC _r,
                                   unsigned int * _s,
                                   unsigned char * _soft_bits)
{
    // run hard demodulation; this will store re-modulated sample
    // as internal variable x_hat
    unsigned int s;
    MODEM(_demodulate)(_q, _r, &s);

    unsigned int bps = MODEM(_get_bps)(_q);

    // gamma = 1/(2*sigma^2), approximate for constellation size
    T gamma = 1.2f*_q->M;

    // set and initialize minimum bit values
    unsigned int i;
    unsigned int k;
    T dmin_0[bps];
    T dmin_1[bps];
    for (k=0; k<bps; k++) {
        dmin_0[k] = 8.0f;
        dmin_1[k] = 8.0f;
    }

    unsigned int bit;
    T d;
    TC x_hat;    // re-modulated symbol
    unsigned char * softab = _q->demod_soft_neighbors;
    unsigned int p = _q->demod_soft_p;

    // check hard demodulation
    d = crealf( (_r-_q->x_hat)*conjf(_r-_q->x_hat) );
    for (k=0; k<bps; k++) {
        bit = (s >> (bps-k-1)) & 0x01;
        if (bit) dmin_1[k] = d;
        else     dmin_0[k] = d;
    }

    // parse all 'nearest neighbors' and find minimum distance for each bit
    for (i=0; i<p; i++) {
        // remodulate symbol
        if (_q->modulate_using_map)
            x_hat = _q->symbol_map[ softab[s*p + i] ];
        else
            MODEM(_modulate)(_q, softab[s*p+i], &x_hat);

        // compute magnitude squared of Euclidean distance
        //d = crealf( (_r-x_hat)*conjf(_r-x_hat) );
        // (same as above, but faster)
        TC e = _r - x_hat;
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
void MODEM(_get_demodulator_sample)(MODEM() _q,
                                    TC * _x_hat)
{
    *_x_hat = _q->x_hat;
}

// get demodulator phase error
T MODEM(_get_demodulator_phase_error)(MODEM() _q)
{
    return cimagf(_q->r*conjf(_q->x_hat));
}

// get error vector magnitude
T MODEM(_get_demodulator_evm)(MODEM() _q)
{
    return cabsf(_q->x_hat - _q->r);
}

// Demodulate a linear symbol constellation using dynamic threshold calculation
//  _v      :   input value
//  _m      :   bits per symbol
//  _alpha  :   scaling factor
//  _s      :   demodulated symbol
//  _res    :   residual
void MODEM(_demodulate_linear_array)(T              _v,
                                     unsigned int   _m,
                                     T              _alpha,
                                     unsigned int * _s,
                                     T *            _res)
{
    unsigned int s=0;
    unsigned int i, k = _m;
    T ref=0.0f;
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
void MODEM(_demodulate_linear_array_ref)(T              _v,
                                         unsigned int   _m,
                                         T *            _ref,
                                         unsigned int * _s,
                                         T *            _res)
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


// generate soft demodulation look-up table
void MODEM(_demodsoft_gentab)(MODEM()      _q,
                              unsigned int _p)
{
    // validate input: ensure number of nearest symbols is not too large
    if (_p > (_q->M-1)) {
        fprintf(stderr,"error: modem_demodsoft_gentab(), requesting too many neighbors\n");
        exit(1);
    }
    
    // allocate internal memory
    _q->demod_soft_p = _p;
    _q->demod_soft_neighbors = (unsigned char*)malloc(_q->M*_p*sizeof(unsigned char));

    unsigned int i;
    unsigned int j;
    unsigned int k;

    // generate constellation
    // TODO : enforce full constellation for modulation
    unsigned int M = _q->M;  // constellation size
    TC c[M];         // constellation
    for (i=0; i<M; i++)
        modem_modulate(_q, i, &c[i]);

    // 
    // find nearest symbols (see algorithm in sandbox/modem_demodulate_soft_gentab.c)
    //

    // initialize table (with 'M' as invalid symbol)
    for (i=0; i<M; i++) {
        for (k=0; k<_p; k++)
            _q->demod_soft_neighbors[i*_p + k] = M;
    }

    int symbol_valid;
    for (i=0; i<M; i++) {
        for (k=0; k<_p; k++) {
            T dmin=1e9f;
            for (j=0; j<M; j++) {
                symbol_valid = 1;
                // ignore this symbol
                if (i==j) symbol_valid = 0;

                // also ignore symbol if it is already in the index
                unsigned int l;
                for (l=0; l<_p; l++) {
                    if ( _q->demod_soft_neighbors[i*_p + l] == j )
                        symbol_valid = 0;
                }

                // compute distance
                T d = cabsf( c[i] - c[j] );

                if ( d < dmin && symbol_valid ) {
                    dmin = d;
                    _q->demod_soft_neighbors[i*_p + k] = j;
                }

            }
        }
    }

#if DEBUG_DEMODULATE_SOFT
    // 
    // print results
    //
    unsigned int bps = _q->m;
    for (i=0; i<M; i++) {
        printf(" %4u [", i);
        print_bitstring_demod_soft(i,bps);
        printf("] : ");
        for (k=0; k<_p; k++) {
            printf(" ");
            print_bitstring_demod_soft(_q->demod_soft_neighbors[i*_p + k],bps);
            if (_q->demod_soft_neighbors[i*_p + k] < M)
                printf("(%6.4f)", cabsf( c[i]-c[_q->demod_soft_neighbors[i*_p+k]] ));
            else
                printf("        ");
        }
        printf("\n");
    }

    // print c-type array
    printf("\n");
    printf("// %s%u soft demodulation nearest neighbors (p=%u)\n", modulation_types[_q->scheme].name, M, _p);
    printf("const unsigned char %s%u_demod_soft_neighbors[%u] = {\n", modulation_types[_q->scheme].name, M, _p*M);
    printf("    ");
    for (i=0; i<M; i++) {
        for (k=0; k<_p; k++) {
            printf("%4u", _q->demod_soft_neighbors[i*_p+k]);
            if (k != _p-1) printf(",");
        }
        if (i != M-1) {
            printf(",   // ");
            print_bitstring_demod_soft(i,bps);
            printf("\n    ");
        } else {
            printf("};  // ");
            print_bitstring_demod_soft(i,bps);
            printf("\n\n");
        }
    }
#endif
}


