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
    mod->scheme = LIQUID_MODEM_APSK4;

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
    mod->scheme = LIQUID_MODEM_APSK8;

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
    mod->scheme = LIQUID_MODEM_APSK16;

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
    mod->scheme = LIQUID_MODEM_APSK32;

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
    mod->scheme = LIQUID_MODEM_APSK64;

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
    mod->scheme = LIQUID_MODEM_APSK128;

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
    mod->scheme = LIQUID_MODEM_APSK256;

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

// modulate APSK
void modem_modulate_apsk(modem _mod,
                         unsigned int symbol_in,
                         float complex *y)
{
    if (symbol_in >= _mod->M) {
        fprintf(stderr,"error: modem_modulate_apsk(), input symbol exceeds maximum\n");
        return;
    }

    // map input symbol to constellation symbol
    unsigned int i;
    unsigned int s = _mod->apsk_symbol_map[symbol_in];

    // determine in which level the symbol is located
    unsigned int p=0;   // level
    unsigned int t=0;   // accumulated number of points per level
    for (i=0; i<_mod->apsk_num_levels; i++) {
        if (s < t + _mod->apsk_p[i]) {
            p = i;
            break;
        }
        t += _mod->apsk_p[i];
    }
    unsigned int s0 = s - t;
    unsigned int s1 = _mod->apsk_p[p];

#if 0
    printf("  s : %3u -> %3u in level %3u (t = %3u) [symbol %3u / %3u]\n", symbol_in, s, p, t, s0,s1);
#endif

    // map symbol to constellation point (radius, angle)
    float r = _mod->apsk_r[p];
    float phi = _mod->apsk_phi[p] + (float)(s0)*2.0f*M_PI / (float)(s1);

    // compute output symbol
    *y = r * liquid_cexpjf(phi);
}

// demodulate APSK
void modem_demodulate_apsk(modem _mod,
                           float complex _x,
                           unsigned int * _symbol_out)
{
    // compute amplitude
    float r = cabsf(_x);

    // determine which ring to demodulate with
    unsigned int i, p=0;
    for (i=0; i<_mod->apsk_num_levels-1; i++) {
        if (r < _mod->apsk_r_slicer[i]) {
            p = i;
            break;
        } else {
            p = _mod->apsk_num_levels-1;
        }
    }

    // find closest point in ring
    float theta = cargf(_x);
    if (theta < 0.0f) theta += 2.0f*M_PI;
    float dphi = 2.0f*M_PI / (float) _mod->apsk_p[p];
    unsigned int s_hat=0;
    float i_hat = (theta - _mod->apsk_phi[p]) / dphi;
    s_hat = roundf(i_hat);      // compute symbol (closest angle)
    s_hat %= _mod->apsk_p[p];   // ensure symbol is in range
    //printf("          i_hat : %12.8f (%3u)\n", i_hat, s_hat);

    // accumulate symbol points
    for (i=0; i<p; i++)
        s_hat += _mod->apsk_p[i];
    //assert(s_hat < _mod->M);

    // reverse symbol mapping
    unsigned int s_prime=0;
    for (i=0; i<_mod->M; i++) {
        if ( _mod->apsk_symbol_map[i] == s_hat) {
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

    *_symbol_out = s_prime;

    // re-modulate symbol and store state
    modem_modulate(_mod, s_prime, &_mod->x_hat);
    _mod->r = _x;
}

