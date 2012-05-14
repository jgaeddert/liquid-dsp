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
// modem_apsk.c
//

// create an apsk (amplitude/phase-shift keying) modem object
MODEM() MODEM(_create_apsk)(unsigned int _bits_per_symbol)
{
    MODEM() q = NULL;
    switch (_bits_per_symbol) {
    case 2: q = MODEM(_create_apsk4)();   break;
    case 3: q = MODEM(_create_apsk8)();   break;
    case 4: q = MODEM(_create_apsk16)();  break;
    case 5: q = MODEM(_create_apsk32)();  break;
    case 6: q = MODEM(_create_apsk64)();  break;
    case 7: q = MODEM(_create_apsk128)(); break;
    case 8: q = MODEM(_create_apsk256)(); break;
    default:
        fprintf(stderr,"error: modem_create_apsk(), unsupported modulation level (%u)\n",
                _bits_per_symbol);
        exit(1);
    }

    // initialize symbol map
    q->symbol_map = (TC*)malloc(q->M*sizeof(TC));
    MODEM(_init_map)(q);
    q->modulate_using_map = 1;

    return q;
}

// create specific APSK-4 modem
MODEM() MODEM(_create_apsk4)()
{
    MODEM() q = (MODEM()) malloc( sizeof(struct MODEM(_s)) );
    q->scheme = LIQUID_MODEM_APSK4;

    MODEM(_init)(q, 2);
    
    // set internals
    q->data.apsk.num_levels = apsk4_num_levels;
    q->data.apsk.p          = (unsigned int *) apsk4_p;
    q->data.apsk.r          = (T *) apsk4_r;
    q->data.apsk.phi        = (T *) apsk4_phi;
    q->data.apsk.r_slicer   = (T *) apsk4_r_slicer;
    q->data.apsk.symbol_map = (unsigned int *) apsk4_symbol_map;

    q->modulate_func = &MODEM(_modulate_apsk);
    q->demodulate_func = &MODEM(_demodulate_apsk);

    // initialize soft-demodulation look-up table
    MODEM(_demodsoft_gentab)(q, 3);

    return q;
}

// create specific APSK-8 modem
MODEM() MODEM(_create_apsk8)()
{
    MODEM() q = (MODEM()) malloc( sizeof(struct MODEM(_s)) );
    q->scheme = LIQUID_MODEM_APSK8;

    MODEM(_init)(q, 3);
    
    // set internals
    q->data.apsk.num_levels = apsk8_num_levels;
    q->data.apsk.p          = (unsigned int *) apsk8_p;
    q->data.apsk.r          = (T *) apsk8_r;
    q->data.apsk.phi        = (T *) apsk8_phi;
    q->data.apsk.r_slicer   = (T *) apsk8_r_slicer;
    q->data.apsk.symbol_map = (unsigned int *) apsk8_symbol_map;

    q->modulate_func = &MODEM(_modulate_apsk);
    q->demodulate_func = &MODEM(_demodulate_apsk);

    // initialize soft-demodulation look-up table
    MODEM(_demodsoft_gentab)(q, 3);

    return q;
}

// create specific APSK-16 modem
MODEM() MODEM(_create_apsk16)()
{
    MODEM() q = (MODEM()) malloc( sizeof(struct MODEM(_s)) );
    q->scheme = LIQUID_MODEM_APSK16;

    MODEM(_init)(q, 4);
    
    // set internals
    q->data.apsk.num_levels = apsk16_num_levels;
    q->data.apsk.p          = (unsigned int *) apsk16_p;
    q->data.apsk.r          = (T *) apsk16_r;
    q->data.apsk.phi        = (T *) apsk16_phi;
    q->data.apsk.r_slicer   = (T *) apsk16_r_slicer;
    q->data.apsk.symbol_map = (unsigned int *) apsk16_symbol_map;

    q->modulate_func = &MODEM(_modulate_apsk);
    q->demodulate_func = &MODEM(_demodulate_apsk);

    // initialize soft-demodulation look-up table
    MODEM(_demodsoft_gentab)(q, 4);

    return q;
}

// create specific APSK-32 modem
MODEM() MODEM(_create_apsk32)()
{
    MODEM() q = (MODEM()) malloc( sizeof(struct MODEM(_s)) );
    q->scheme = LIQUID_MODEM_APSK32;

    MODEM(_init)(q, 5);
    
    // set internals
    q->data.apsk.num_levels = apsk32_num_levels;
    q->data.apsk.p          = (unsigned int *) apsk32_p;
    q->data.apsk.r          = (T *) apsk32_r;
    q->data.apsk.phi        = (T *) apsk32_phi;
    q->data.apsk.r_slicer   = (T *) apsk32_r_slicer;
    q->data.apsk.symbol_map = (unsigned int *) apsk32_symbol_map;

    q->modulate_func = &MODEM(_modulate_apsk);
    q->demodulate_func = &MODEM(_demodulate_apsk);

    // initialize soft-demodulation look-up table
    MODEM(_demodsoft_gentab)(q, 4);

    return q;
}

// create specific APSK-64 modem
MODEM() MODEM(_create_apsk64)()
{
    MODEM() q = (MODEM()) malloc( sizeof(struct MODEM(_s)) );
    q->scheme = LIQUID_MODEM_APSK64;

    MODEM(_init)(q, 6);
    
    // set internals
    q->data.apsk.num_levels = apsk64_num_levels;
    q->data.apsk.p          = (unsigned int *) apsk64_p;
    q->data.apsk.r          = (T *) apsk64_r;
    q->data.apsk.phi        = (T *) apsk64_phi;
    q->data.apsk.r_slicer   = (T *) apsk64_r_slicer;
    q->data.apsk.symbol_map = (unsigned int *) apsk64_symbol_map;

    q->modulate_func = &MODEM(_modulate_apsk);
    q->demodulate_func = &MODEM(_demodulate_apsk);

    // initialize soft-demodulation look-up table
    MODEM(_demodsoft_gentab)(q, 4);

    return q;
}

// create specific APSK-128 modem
MODEM() MODEM(_create_apsk128)()
{
    MODEM() q = (MODEM()) malloc( sizeof(struct MODEM(_s)) );
    q->scheme = LIQUID_MODEM_APSK128;

    MODEM(_init)(q, 7);
    
    // set internals
    q->data.apsk.num_levels = apsk128_num_levels;
    q->data.apsk.p          = (unsigned int *) apsk128_p;
    q->data.apsk.r          = (T *) apsk128_r;
    q->data.apsk.phi        = (T *) apsk128_phi;
    q->data.apsk.r_slicer   = (T *) apsk128_r_slicer;
    q->data.apsk.symbol_map = (unsigned int *) apsk128_symbol_map;

    q->modulate_func = &MODEM(_modulate_apsk);
    q->demodulate_func = &MODEM(_demodulate_apsk);

    // initialize soft-demodulation look-up table
    MODEM(_demodsoft_gentab)(q, 5);

    return q;
}

// create specific APSK-256 modem
MODEM() MODEM(_create_apsk256)()
{
    MODEM() q = (MODEM()) malloc( sizeof(struct MODEM(_s)) );
    q->scheme = LIQUID_MODEM_APSK256;

    MODEM(_init)(q, 8);
    
    // set internals
    q->data.apsk.num_levels = apsk256_num_levels;
    q->data.apsk.p          = (unsigned int *) apsk256_p;
    q->data.apsk.r          = (T *) apsk256_r;
    q->data.apsk.phi        = (T *) apsk256_phi;
    q->data.apsk.r_slicer   = (T *) apsk256_r_slicer;
    q->data.apsk.symbol_map = (unsigned int *) apsk256_symbol_map;

    q->modulate_func = &MODEM(_modulate_apsk);
    q->demodulate_func = &MODEM(_demodulate_apsk);

    // initialize soft-demodulation look-up table
    MODEM(_demodsoft_gentab)(q, 5);

    return q;
}

// modulate APSK
void MODEM(_modulate_apsk)(MODEM()      _q,
                           unsigned int _sym_in,
                           TC *         _y)
{
    if (_sym_in >= _q->M) {
        fprintf(stderr,"error: modem_modulate_apsk(), input symbol exceeds maximum\n");
        return;
    }

    // map input symbol to constellation symbol
    unsigned int i;
    unsigned int s = _q->data.apsk.symbol_map[_sym_in];

    // determine in which level the symbol is located
    unsigned int p=0;   // level
    unsigned int t=0;   // accumulated number of points per level
    for (i=0; i<_q->data.apsk.num_levels; i++) {
        if (s < t + _q->data.apsk.p[i]) {
            p = i;
            break;
        }
        t += _q->data.apsk.p[i];
    }
    unsigned int s0 = s - t;
    unsigned int s1 = _q->data.apsk.p[p];

#if 0
    printf("  s : %3u -> %3u in level %3u (t = %3u) [symbol %3u / %3u]\n", _sym_in, s, p, t, s0,s1);
#endif

    // map symbol to constellation point (radius, angle)
    T r = _q->data.apsk.r[p];
    T phi = _q->data.apsk.phi[p] + (T)(s0)*2.0f*M_PI / (T)(s1);

    // compute output symbol
    *_y = r * liquid_cexpjf(phi);
}

// demodulate APSK
void MODEM(_demodulate_apsk)(MODEM()        _q,
                             TC             _x,
                             unsigned int * _sym_out)
{
    // compute amplitude
    T r = cabsf(_x);

    // determine which ring to demodulate with
    unsigned int i, p=0;
    for (i=0; i<_q->data.apsk.num_levels-1; i++) {
        if (r < _q->data.apsk.r_slicer[i]) {
            p = i;
            break;
        } else {
            p = _q->data.apsk.num_levels-1;
        }
    }

    // find closest point in ring
    T theta = cargf(_x);
    if (theta < 0.0f) theta += 2.0f*M_PI;
    T dphi = 2.0f*M_PI / (T) _q->data.apsk.p[p];
    unsigned int s_hat=0;
    T i_hat = (theta - _q->data.apsk.phi[p]) / dphi;
    s_hat = roundf(i_hat);      // compute symbol (closest angle)
    s_hat %= _q->data.apsk.p[p];   // ensure symbol is in range
    //printf("          i_hat : %12.8f (%3u)\n", i_hat, s_hat);

    // accumulate symbol points
    for (i=0; i<p; i++)
        s_hat += _q->data.apsk.p[i];
    //assert(s_hat < _q->M);

    // reverse symbol mapping
    unsigned int s_prime=0;
    for (i=0; i<_q->M; i++) {
        if ( _q->data.apsk.symbol_map[i] == s_hat) {
            s_prime = i;
            break;
        }
    }

#if 0
    printf("              x : %12.8f + j*%12.8f\n", crealf(_x), cimagf(_x));
    printf("              p : %3u\n", p);
    printf("          theta : %12.8f\n", theta);
    printf("           dmin : %12.8f\n", dmin);
    printf("              s : %3u > %3u\n", s_hat, s_prime);
#endif

    *_sym_out = s_prime;

    // re-modulate symbol and store state
    MODEM(_modulate)(_q, s_prime, &_q->x_hat);
    _q->r = _x;
}

