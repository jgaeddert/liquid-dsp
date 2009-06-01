/*
 * Copyright (c) 2007, 2009 Joseph Gaeddert
 * Copyright (c) 2007, 2009 Virginia Polytechnic Institute & State University
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
//
//

#include "modem_fixed.h"

modem_q32* modem_create_q32(
    modulation_scheme _scheme,
    unsigned int _bits_per_symbol)
{
    if (_bits_per_symbol < 1 ) {
        perror("ERROR! modem_create, modem must have at least 1 bit/symbol\n");
        return NULL;
    } else if (_bits_per_symbol > MAX_MOD_BITS_PER_SYMBOL) {
        perror("ERROR! modem_create, maximum number of bits/symbol exceeded\n");
        return NULL;
    }

    switch (_scheme) {
    case MOD_ASK:
        return modem_create_ask_q32(_bits_per_symbol);
    case MOD_QAM:
        return modem_create_qam_q32(_bits_per_symbol);
    case MOD_PSK:
        return modem_create_psk_q32(_bits_per_symbol);
    case MOD_BPSK:
        return modem_create_bpsk_q32(_bits_per_symbol);
    case MOD_QPSK:
        return modem_create_qpsk_q32(_bits_per_symbol);
    case MOD_DPSK:
        return modem_create_dpsk_q32(_bits_per_symbol);
    case MOD_ARB:
        return modem_create_arb_q32(_bits_per_symbol);
    case MOD_ARB_MIRRORED:
        return modem_create_arb_mirrored_q32(_bits_per_symbol);
    case MOD_ARB_ROTATED:
        return modem_create_arb_rotated_q32(_bits_per_symbol);
    default:
        perror("ERROR: modem_create_q32(), unknown/unsupported modulation scheme\n");
        exit(-1);
    }

    return NULL;
}

void modem_init_q32(modem_q32* _mod, unsigned int _bits_per_symbol)
{
//#if liquid_VALIDATE_INPUT
    if (_bits_per_symbol < 1) {
        perror("ERROR: modem_init_q32(), must have at least 1 bit per symbol\n");
        return;
    } else if (_bits_per_symbol > MAX_MOD_BITS_PER_SYMBOL) {
        perror("ERROR! modem_init_q32(), maximum number of bits/symbol exceeded\n");
        return;
    }
//#endif

    _mod->m = _bits_per_symbol;
    _mod->M = 1 << (_mod->m);
    _mod->m_i = 0;
    _mod->M_i = 0;
    _mod->m_q = 0;
    _mod->M_q = 0;
    
    _mod->alpha = 0;

    _mod->num_levels = 0;
    _mod->levels_i = NULL;
    _mod->levels_q = NULL;

    _mod->state_i = 0;
    _mod->state_q = 0;
    _mod->state_theta = 0;

    _mod->res_i = 0;
    _mod->res_q = 0;

    _mod->phase_error = 0;
    _mod->evm = 0;

    _mod->d_phi = 0;
}

modem_q32* modem_create_ask_q32(
    unsigned int _bits_per_symbol)
{
    modem_q32* mod = (modem_q32*) malloc( sizeof(modem_q32) );
    mod->scheme = MOD_ASK;
    
    modem_init_q32(mod, _bits_per_symbol);

    switch (mod->M) {
    case 2:     mod->alpha = PAM2_ALPHA_Q32;    break;
    case 4:     mod->alpha = PAM4_ALPHA_Q32;    break;
    case 8:     mod->alpha = PAM8_ALPHA_Q32;    break;
    case 16:    mod->alpha = PAM16_ALPHA_Q32;   break;
    case 32:    mod->alpha = PAM32_ALPHA_Q32;   break;
    default:
        // calculate alpha dynamically
        // NOTE: this is only an approximation
        //   alpha ~ sqrt(3) / M
        mod->alpha = (q32_t) (0xDDB3D743UL / (mod->M));
    }

    unsigned int k;
    for (k=0; k<(mod->m); k++)
        mod->ref[k] = (q32_t) ((1<<k) * mod->alpha);

    return mod;
}

modem_q32* modem_create_qam_q32(
    unsigned int _bits_per_symbol)
{
    if (_bits_per_symbol < 2 ) {
        perror("ERROR! modem_create_qam, modem must have at least 2 bits/symbol\n");
        return NULL;
    }

    modem_q32* mod = (modem_q32*) malloc( sizeof(modem_q32) );
    mod->scheme = MOD_QAM;

    modem_init_q32(mod, _bits_per_symbol);

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
    case 4:     mod->alpha = RQAM4_ALPHA_Q32;       break;
    case 8:     mod->alpha = RQAM8_ALPHA_Q32;       break;
    case 16:    mod->alpha = RQAM16_ALPHA_Q32;      break;
    case 32:    mod->alpha = RQAM32_ALPHA_Q32;      break;
    case 64:    mod->alpha = RQAM64_ALPHA_Q32;      break;
    case 128:   mod->alpha = RQAM128_ALPHA_Q32;     break;
    case 256:   mod->alpha = RQAM256_ALPHA_Q32;     break;
    case 512:   mod->alpha = RQAM512_ALPHA_Q32;     break;
    case 1024:  mod->alpha = RQAM1024_ALPHA_Q32;    break;
    case 2048:  mod->alpha = RQAM2048_ALPHA_Q32;    break;
    case 4096:  mod->alpha = RQAM4096_ALPHA_Q32;    break;
    default:
        // calculate alpha dynamically
        // NOTE: this is only an approximation
        //   alpha ~ sqrt(2) / M
        mod->alpha = (q32_t) (0xB504F334 / (mod->M) );
    }

    unsigned int k;
    for (k=0; k<(mod->m); k++)
        mod->ref[k] = (q32_t) ( (1<<k) * mod->alpha );

    return mod;
}

modem_q32* modem_create_psk_q32(
    unsigned int _bits_per_symbol)
{
    modem_q32* mod = (modem_q32*) malloc( sizeof(modem_q32) );
    mod->scheme = MOD_PSK;

    modem_init_q32(mod, _bits_per_symbol);

    mod->alpha = liquid_Q32_PI/(mod->M);

    unsigned int k;
    for (k=0; k<(mod->m); k++)
        mod->ref[k] = (q32_t) ( (1<<k) * mod->alpha );

    mod->d_phi = mad_f_mul(liquid_Q32_PI, (MAD_F_ONE - MAD_F_ONE/(mod->M)) );
    //printf("modem_create_psk_32, d_phi: %f\n", liquid_angle_fixed_to_double(mod->d_phi));

    mod->state_i = MAD_F_ONE;
    mod->state_q = 0;
    mod->state_theta = 0;

    return mod;
}

modem_q32* modem_create_bpsk_q32(
    unsigned int _bits_per_symbol)
{
    modem_q32* mod = (modem_q32*) malloc( sizeof(modem_q32) );
    mod->scheme = MOD_BPSK;

    modem_init_q32(mod, 1);

    return mod;
}

modem_q32* modem_create_qpsk_q32(
    unsigned int _bits_per_symbol)
{
    modem_q32* mod = (modem_q32*) malloc( sizeof(modem_q32) );
    mod->scheme = MOD_QPSK;

    modem_init_q32(mod, 2);

    return mod;
}

modem_q32* modem_create_dpsk_q32(
    unsigned int _bits_per_symbol)
{
    modem_q32* mod = (modem_q32*) malloc( sizeof(modem_q32) );
    mod->scheme = MOD_DPSK;

    modem_init_q32(mod, _bits_per_symbol);

    mod->alpha = (q32_t) ( liquid_Q32_PI/(mod->M) );

    unsigned int k;
    for (k=0; k<(mod->m); k++)
        mod->ref[k] = (q32_t) ( (1<<k) * mod->alpha );

    mod->d_phi = mad_f_mul(liquid_Q32_PI, (MAD_F_ONE - MAD_F_ONE/(mod->M)) );

    mod->state_i = MAD_F_ONE;
    mod->state_q = 0;
    mod->state_theta = 0;

    return mod;
}


modem_q32* modem_create_arb_q32(unsigned int _bits_per_symbol)
{
#ifdef liquid_VALIDATE_INPUT
    if (_bits_per_symbol < 2) {
        // must have at least 2 bits per symbol
        perror("ERROR: modem_create_arb_q32(), must have at least 2 bits per symbol\n");
        return NULL;
    } else if (_bits_per_symbol > MAX_MOD_BITS_PER_SYMBOL) {
        perror("ERROR! modem_create_arb_q32(), maximum number of bits/symbol exceeded\n");
        return NULL;
    }
#endif

    modem_q32* mod = (modem_q32*) malloc( sizeof(modem_q32) );
    mod->scheme = MOD_ARB;

    modem_init_q32(mod, _bits_per_symbol);

    mod->num_levels = mod->M;
    mod->levels_i = (q32_t*) calloc(mod->num_levels, sizeof(q32_t) );
    mod->levels_q = (q32_t*) calloc(mod->num_levels, sizeof(q32_t) );

    return mod;
}

modem_q32* modem_create_arb_mirrored_q32(unsigned int _bits_per_symbol)
{
#ifdef liquid_VALIDATE_INPUT
    if (_bits_per_symbol < 2) {
        // must have at least 2 bits per symbol
        perror("ERROR: modem_create_arb_mirrored_q32(), must have at least 2 bits per symbol\n");
        return NULL;
    } else if (_bits_per_symbol > MAX_MOD_BITS_PER_SYMBOL) {
        perror("ERROR! modem_create_arb_mirrored_q32(), maximum number of bits/symbol exceeded\n");
        return NULL;
    }
#endif

    modem_q32* mod = (modem_q32*) malloc( sizeof(modem_q32) );
    mod->scheme = MOD_ARB_MIRRORED;

    modem_init_q32(mod, _bits_per_symbol);

    mod->num_levels = (mod->M) >> 2;
    mod->levels_i = (q32_t*) calloc(mod->num_levels, sizeof(q32_t) );
    mod->levels_q = (q32_t*) calloc(mod->num_levels, sizeof(q32_t) );

    return mod;
}


modem_q32* modem_create_arb_rotated_q32(unsigned int _bits_per_symbol)
{
#ifdef liquid_VALIDATE_INPUT
    if (_bits_per_symbol < 2) {
        // must have at least 2 bits per symbol
        perror("ERROR: modem_create_arb_rotated_q32(), must have at least 2 bits per symbol\n");
        return NULL;
    } else if (_bits_per_symbol > MAX_MOD_BITS_PER_SYMBOL) {
        perror("ERROR! modem_create_arb_rotated_q32(), maximum number of bits/symbol exceeded\n");
        return NULL;
    }
#endif

    modem_q32* mod = (modem_q32*) malloc( sizeof(modem_q32) );
    mod->scheme = MOD_ARB_ROTATED;

    modem_init_q32(mod, _bits_per_symbol);

    mod->num_levels = (mod->M) >> 2;
    mod->levels_i = (q32_t*) calloc(mod->num_levels, sizeof(q32_t) );
    mod->levels_q = (q32_t*) calloc(mod->num_levels, sizeof(q32_t) );

    return mod;
}

void free_modem_q32(modem_q32 *_mod)
{
    free(_mod->levels_i);
    free(_mod->levels_q);
    free(_mod);
}

// initialize modem on array
void modem_arb_init_q32(modem_q32 *_mod, q32_t* _I, q32_t* _Q, unsigned int _len)
{
#ifdef liquid_VALIDATE_INPUT
    if (_len != _mod->num_levels) {
        perror("ERROR: modem_arb_init_q32(), array sizes do not match\n");
        return;
    }
#endif

    unsigned int i;
    for (i=0; i<_len; i++) {
#ifdef liquid_VALIDATE_INPUT
        if ((_mod->scheme == MOD_ARB_MIRRORED) || (_mod->scheme == MOD_ARB_ROTATED)) {
            // symbols should only exist in first quadrant
            if ( _I[i] <= 0 || _Q[i] <= 0 )
                printf("WARNING: modem_arb_init_q32(), symbols exist outside first quadrant\n");
        }
#endif

        _mod->levels_i[i] = _I[i];
        _mod->levels_q[i] = _Q[i];
    }

    // Balance I/Q signals
    if (_mod->scheme == MOD_ARB)
        modem_arb_balance_iq_q32(_mod);

    // scale energy to unity
    modem_arb_scale_q32(_mod);
}

void modem_arb_init_file_q32(modem_q32 *_mod, char* filename) {
    // try to open file
    FILE * f = fopen(filename, "r");
    if (f == NULL) {
        perror("modem_arb_init_file_q32(), could not open file\n");
        exit(1);
    }

    unsigned int i, results;
    for (i=0; i<_mod->num_levels; i++) {
        results = fscanf(f, "%d %d\n", &(_mod->levels_i[i]), &(_mod->levels_q[i]));

        // ensure proper number of symbols were read
        if (results < 2) {
            perror("modem_arb_init_file_q32() unable to parse line\n");
            exit(-1);
        }

#ifdef liquid_VALIDATE_INPUT
        if ((_mod->scheme == MOD_ARB_MIRRORED) || (_mod->scheme == MOD_ARB_ROTATED)) {
        // symbols should only exist in first quadrant
            if ( _mod->levels_i[i] <= 0 || _mod->levels_q[i] <= 0 )
                printf("WARNING: modem_arb_init_file_q32(), symbols exist outside first quadrant\n");
        }
#endif

    }

    fclose(f);

    // Balance I/Q signals
    if (_mod->scheme == MOD_ARB)
        modem_arb_balance_iq_q32(_mod);

    // scale modem to have unity energy
    modem_arb_scale_q32(_mod);
}

// scale energy to unity
void modem_arb_scale_q32(modem_q32 *_mod)
{
    ///\todo do not use floating point computation
    float e = 0.0f;

    float I, Q;
    unsigned int i;
    for (i=0; i<_mod->num_levels; i++) {
        I = mad_f_todouble( _mod->levels_i[i] );
        Q = mad_f_todouble( _mod->levels_q[i] );
        e += I*I + Q*Q;
    }
    e /= (float) (_mod->num_levels);
    e = sqrtf(e);

    for (i=0; i<_mod->num_levels; i++) {
        _mod->levels_i[i] = mad_f_tofixed( mad_f_todouble(_mod->levels_i[i])/e );
        _mod->levels_q[i] = mad_f_tofixed( mad_f_todouble(_mod->levels_q[i])/e );
    }
}

void modem_arb_balance_iq_q32(modem_q32 *_mod)
{
    signed long int mean_i=0, mean_q=0;
    unsigned int i;

    // accumulate average signal
    for (i=0; i<_mod->num_levels; i++) {
        mean_i += (signed long int) (_mod->levels_i[i]);
        mean_q += (signed long int) (_mod->levels_q[i]);
    }
    mean_i /= _mod->num_levels;
    mean_q /= _mod->num_levels;

    // subtract mean value from reference levels
    for (i=0; i<_mod->num_levels; i++) {
        _mod->levels_i[i] -= mean_i;
        _mod->levels_q[i] -= mean_q;
    }
}

void modulate_q32(
    modem_q32 *_mod,
    unsigned int symbol_in,
    q32_t *I_out,
    q32_t *Q_out)
{
    switch (_mod->scheme) {
    case MOD_ASK:
        modulate_ask_q32(_mod, symbol_in, I_out, Q_out);
        break;
    case MOD_QAM:
        modulate_qam_q32(_mod, symbol_in, I_out, Q_out);
        break;
    case MOD_PSK:
        modulate_psk_q32(_mod, symbol_in, I_out, Q_out);
        break;
    case MOD_BPSK:
        modulate_bpsk_q32(_mod, symbol_in, I_out, Q_out);
        break;
    case MOD_QPSK:
        modulate_qpsk_q32(_mod, symbol_in, I_out, Q_out);
        break;
    case MOD_DPSK:
        modulate_dpsk_q32(_mod, symbol_in, I_out, Q_out);
        break;
    case MOD_ARB:
        modulate_arb_q32(_mod, symbol_in, I_out, Q_out);
        break;
    case MOD_ARB_MIRRORED:
        modulate_arb_mirrored_q32(_mod, symbol_in, I_out, Q_out);
        break;
    case MOD_ARB_ROTATED:
        modulate_arb_rotated_q32(_mod, symbol_in, I_out, Q_out);
        break;
    default:
        perror("ERROR: modulate_q32(), unknown/unsupported modulation scheme\n");
        break;
    }
}

void modulate_ask_q32(
    modem_q32 *_mod,
    unsigned int symbol_in,
    q32_t *I_out,
    q32_t *Q_out)
{
    symbol_in = gray_decode(symbol_in);
    *I_out = (2*symbol_in - (_mod->M) + 1) * _mod->alpha;
    *Q_out = 0;
}

void modulate_qam_q32(
    modem_q32 *_mod,
    unsigned int symbol_in,
    q32_t *I_out,
    q32_t *Q_out)
{
    unsigned int s_i, s_q;
    s_i = symbol_in >> _mod->m_q;
    s_q = symbol_in & ( (1<<_mod->m_q)-1 );

    s_i = gray_decode(s_i);
    s_q = gray_decode(s_q);

    *I_out = (2*(int)s_i - (int)(_mod->M_i) + 1) * _mod->alpha;
    *Q_out = (2*(int)s_q - (int)(_mod->M_q) + 1) * _mod->alpha;
}

void modulate_psk_q32(
    modem_q32 *_mod,
    unsigned int symbol_in,
    q32_t *I_out,
    q32_t *Q_out)
{
    symbol_in = gray_decode(symbol_in);
    q32_t theta = symbol_in * 2 * _mod->alpha;
    liquid_sincos_q32(theta, Q_out, I_out);
    //printf("  s : %u, I=%f, Q=%f\n", symbol_in, mad_f_todouble(*I_out), mad_f_todouble(*Q_out) );
}

void modulate_bpsk_q32(
    modem_q32 *_mod,
    unsigned int symbol_in,
    q32_t *I_out,
    q32_t *Q_out)
{
    *I_out = symbol_in ? -MAD_F_ONE : MAD_F_ONE;
    *Q_out = 0;
}

void modulate_qpsk_q32(
    modem_q32 *_mod,
    unsigned int symbol_in,
    q32_t *I_out,
    q32_t *Q_out)
{
    *I_out = symbol_in & 0x01 ? -liquid_Q32_1_BY_SQRT_2 : liquid_Q32_1_BY_SQRT_2;
    *Q_out = symbol_in & 0x02 ? -liquid_Q32_1_BY_SQRT_2 : liquid_Q32_1_BY_SQRT_2;
}

void modulate_dpsk_q32(
    modem_q32 *_mod,
    unsigned int symbol_in,
    q32_t *I_out,
    q32_t *Q_out)
{
    symbol_in = gray_decode(symbol_in);
    q32_t d_theta = symbol_in * 2 * _mod->alpha;
    _mod->state_theta += d_theta;
    if (_mod->state_theta > liquid_Q32_2_PI)
        _mod->state_theta -= liquid_Q32_2_PI;

    liquid_sincos_q32(_mod->state_theta, Q_out, I_out);

    _mod->state_i = *I_out;
    _mod->state_q = *Q_out;
    //printf("mod: state_theta = %f\n", _mod->state_theta);
}

void modulate_arb_q32(modem_q32 *_mod, unsigned int symbol_in, q32_t *I_out, q32_t *Q_out)
{
#ifdef liquid_VALIDATE_INPUT
    if (symbol_in >= _mod->M) {
        perror("ERROR: modulate_arb_q32(), input symbol exceeds maximum\n");
        return;
    }
#endif

    *I_out = _mod->levels_i[symbol_in];
    *Q_out = _mod->levels_q[symbol_in];
}


void modulate_arb_mirrored_q32(modem_q32 *_mod, unsigned int symbol_in, q32_t *I_out, q32_t *Q_out)
{
#ifdef liquid_VALIDATE_INPUT
    if (symbol_in >= _mod->M) {
        perror("ERROR: modulate_arb_mirrored_q32(), input symbol exceeds maximum\n");
        return;
    }
#endif

    // IQ:
    // -----
    // 01 00
    // 11 10
    unsigned int quadrant = symbol_in >> (_mod->m - 2);
    symbol_in &= _mod->num_levels - 1;

    q32_t I = _mod->levels_i[symbol_in];
    q32_t Q = _mod->levels_q[symbol_in];

    // mirrored
    if ( quadrant & 0x01 )
        *I_out = -I;
    else
        *I_out =  I;

    if ( quadrant & 0x02 )
        *Q_out = -Q;
    else
        *Q_out =  Q;
}


void modulate_arb_rotated_q32(modem_q32 *_mod, unsigned int symbol_in, q32_t *I_out, q32_t *Q_out)
{
    if (symbol_in >= _mod->M) {
        perror("ERROR: modulate_arb_rotated_q32(), input symbol exceeds maximum\n");
        return;
    }

    // IQ:
    // -----
    // 01 00
    // 11 10
    unsigned int quadrant = symbol_in >> (_mod->m - 2);
    symbol_in &= _mod->num_levels - 1;

    q32_t I = _mod->levels_i[symbol_in];
    q32_t Q = _mod->levels_q[symbol_in];

    // rotated
    if (quadrant == 0) {
        *I_out =  I;
        *Q_out =  Q;
    } else if (quadrant == 1) {
        *I_out = -Q;
        *Q_out =  I;
    } else if (quadrant == 3) {
        *I_out = -I;
        *Q_out = -Q;
    } else {
        *I_out =  Q;
        *Q_out = -I;
    }
}

void demodulate_q32(
    modem_q32 *_demod,
    q32_t I_in,
    q32_t Q_in,
    unsigned int *symbol_out)
{
    switch (_demod->scheme) {
    case MOD_ASK:
        demodulate_ask_q32(_demod, I_in, Q_in, symbol_out);
        return;
    case MOD_QAM:
        demodulate_qam_q32(_demod, I_in, Q_in, symbol_out);
        return;
    case MOD_PSK:
        demodulate_psk_q32(_demod, I_in, Q_in, symbol_out);
        return;
    case MOD_BPSK:
        demodulate_bpsk_q32(_demod, I_in, Q_in, symbol_out);
        return;
    case MOD_QPSK:
        demodulate_qpsk_q32(_demod, I_in, Q_in, symbol_out);
        return;
    case MOD_DPSK:
        demodulate_dpsk_q32(_demod, I_in, Q_in, symbol_out);
        return;
    case MOD_ARB:
        demodulate_arb_q32(_demod, I_in, Q_in, symbol_out);
        return;
    case MOD_ARB_MIRRORED:
        demodulate_arb_mirrored_q32(_demod, I_in, Q_in, symbol_out);
        return;
    case MOD_ARB_ROTATED:
        demodulate_arb_rotated_q32(_demod, I_in, Q_in, symbol_out);
        return;
    default:
        perror("ERROR: demodulate, unknown/unsupported demodulation scheme\n");
        break;
    }

}

void demodulate_ask_q32(
    modem_q32 *_demod,
    q32_t I_in,
    q32_t Q_in,
    unsigned int *symbol_out)
{
    unsigned int s;
    _demod->state_i = I_in;
    _demod->state_q = Q_in;
    demodulate_linear_array_ref_q32(I_in, _demod->m, _demod->ref, &s, &(_demod->res_i));
    *symbol_out = gray_encode(s);

    _demod->res_q = Q_in;
}

void demodulate_qam_q32(
    modem_q32 *_demod,
    q32_t I_in,
    q32_t Q_in,
    unsigned int *symbol_out)
{
    unsigned int s_i, s_q;
    demodulate_linear_array_ref_q32(I_in, _demod->m_i, _demod->ref, &s_i, &(_demod->res_i));
    demodulate_linear_array_ref_q32(Q_in, _demod->m_q, _demod->ref, &s_q, &(_demod->res_q));
    s_i = gray_encode(s_i);
    s_q = gray_encode(s_q);
    *symbol_out = ( s_i << _demod->m_q ) + s_q;
}

void demodulate_psk_q32(
    modem_q32 *_demod,
    q32_t I_in,
    q32_t Q_in,
    unsigned int *symbol_out)
{
    unsigned int s;
    q32_t theta = liquid_atan2_q32(Q_in, I_in);
    _demod->state_i = I_in;
    _demod->state_q = Q_in;
    _demod->state_theta = theta;

    // subtract phase offset, ensuring phase is in [-pi,pi)
    theta -= _demod->d_phi;
    if (theta < -liquid_Q32_PI)
        theta += liquid_Q32_2_PI;
    //printf("  theta : %f\n", liquid_angle_fixed_to_double(theta) );

    demodulate_linear_array_ref_q32(theta, _demod->m, _demod->ref, &s, &(_demod->phase_error));
    *symbol_out = gray_encode(s);
}


void demodulate_bpsk_q32(
    modem_q32 *_demod,
    q32_t I_in,
    q32_t Q_in,
    unsigned int *symbol_out)
{
    *symbol_out = (I_in > 0) ? 0 : 1;
    _demod->state_i = I_in;
    _demod->state_q = Q_in;
}

void demodulate_qpsk_q32(
    modem_q32 *_demod,
    q32_t I_in,
    q32_t Q_in,
    unsigned int *symbol_out)
{
    *symbol_out  = (I_in > 0) ? 0 : 1;
    *symbol_out |= (Q_in > 0) ? 0 : 2;
    _demod->state_i = I_in;
    _demod->state_q = Q_in;
}

void demodulate_dpsk_q32(
    modem_q32 *_demod,
    q32_t I_in,
    q32_t Q_in,
    unsigned int *symbol_out)
{
    unsigned int s;
    q32_t theta = liquid_atan2_q32(Q_in, I_in);
    q32_t d_theta = theta - _demod->state_theta;
    _demod->state_i = I_in;
    _demod->state_q = Q_in;
    _demod->state_theta = theta;

    // subtract phase offset, ensuring phase is in [-pi,pi)
    d_theta -= _demod->d_phi;
    if (d_theta < -liquid_Q32_PI)
        d_theta += liquid_Q32_2_PI;
    else if (d_theta > liquid_Q32_PI)
        d_theta -= liquid_Q32_2_PI;

    demodulate_linear_array_ref_q32(d_theta, _demod->m, _demod->ref, &s, &(_demod->phase_error));
    *symbol_out = gray_encode(s);
}

void demodulate_arb_q32(modem_q32* _mod, q32_t I_in, q32_t Q_in, unsigned int *symbol_out)
{
    //printf("demodulate_arb_q32() invoked with I=%d, Q=%d\n", I_in, Q_in);
    
    unsigned int i, s=0;
    unsigned int d, d_min = UINT_MAX;

    for (i=0; i<_mod->num_levels; i++) {
        d = liquid_cabs_q32(
            I_in - _mod->levels_i[i],
            Q_in - _mod->levels_q[i]);

        //printf("  d[%u] = %u\n", i, d);

        if ( d < d_min ) {
            d_min = d;
            s = i;
        }
    }
    //printf(" > s = %d\n", *symbol_out);
    _mod->res_i = I_in - _mod->levels_i[s];
    _mod->res_q = Q_in - _mod->levels_q[s];

    *symbol_out = s;
}


void demodulate_arb_mirrored_q32(modem_q32* _mod, q32_t I_in, q32_t Q_in, unsigned int *symbol_out)
{
    //printf("demodulate_arb_mirrored_q32() invoked with I=%d, Q=%d\n", I_in, Q_in);
    
    unsigned int i;
    unsigned int s_quad=0;
    unsigned int s_q1=0;
    unsigned int d, d_min = UINT_MAX;

    // determine quadrant, demodulate 2 most-significant bits appropriately
    // IQ:
    // -----
    // 01 00
    // 11 10
    if ( I_in < 0 )
        s_quad |= 0x01;

    if ( Q_in < 0 )
        s_quad |= 0x02;

    // mirrored
    I_in = abs(I_in);
    Q_in = abs(Q_in);

    for (i=0; i<_mod->num_levels; i++) {
        d = liquid_cabs_q32(
            I_in - _mod->levels_i[i],
            Q_in - _mod->levels_q[i]);

        //printf("  d[%u] = %u\n", i, d);

        if ( d < d_min ) {
            d_min = d;
            s_q1 = i;
        }
    }
    //printf(" > s = %d\n", *symbol_out);
    _mod->res_i = I_in - _mod->levels_i[s_q1];
    _mod->res_q = Q_in - _mod->levels_q[s_q1];

    // output symbol : <q(1) q(0)> <b(m-2-1) b(m-2-2) ... b(0)>
    *symbol_out = (s_quad << (_mod->m - 2)) | s_q1;
}


void demodulate_arb_rotated_q32(modem_q32* _mod, q32_t I_in, q32_t Q_in, unsigned int *symbol_out)
{
    //printf("demodulate_arb_rotated_q32() invoked with I=%d, Q=%d\n", I_in, Q_in);
    
    unsigned int i;
    unsigned int s_quad=0;
    unsigned int s_q1=0;
    unsigned int d, d_min = UINT_MAX;

    // determine quadrant, demodulate 2 most-significant bits appropriately
    // IQ:
    // -----
    // 01 00
    // 11 10
    if ( I_in < 0 )
        s_quad |= 0x01;

    if ( Q_in < 0 )
        s_quad |= 0x02;

    // rotated
    q32_t I = I_in;
    q32_t Q = Q_in;
    if (s_quad == 0) {
        I_in =  I;
        Q_in =  Q;
    } else if (s_quad == 1) {
        I_in =  Q;
        Q_in = -I;
    } else if (s_quad == 3) {
        I_in = -I;
        Q_in = -Q;
    } else if (s_quad == 2) {
        I_in = -Q;
        Q_in =  I;
    }

    for (i=0; i<_mod->num_levels; i++) {
        d = liquid_cabs_q32(
            I_in - _mod->levels_i[i],
            Q_in - _mod->levels_q[i]);

        //printf("  d[%u] = %u\n", i, d);

        if ( d < d_min ) {
            d_min = d;
            s_q1 = i;
        }
    }
    //printf(" > s = %d\n", *symbol_out);
    _mod->res_i = I_in - _mod->levels_i[s_q1];
    _mod->res_q = Q_in - _mod->levels_q[s_q1];

    // output symbol : <q(1) q(0)> <b(m-2-1) b(m-2-2) ... b(0)>
    *symbol_out = (s_quad << (_mod->m - 2)) | s_q1;
}


// get demodulator phase error
void get_demodulator_phase_error_q32(modem_q32* _demod, q32_t* _phi)
{
    switch (_demod->scheme) {
    case MOD_PSK:
    case MOD_DPSK:
        // no need to calculate phase error from PSK/DPSK
        break;
    case MOD_BPSK:
        _demod->phase_error = liquid_atan2_q32(_demod->state_q, _demod->state_i);
        if (_demod->phase_error > liquid_Q32_PI_BY_2)
            _demod->phase_error -= liquid_Q32_PI;
        else if (_demod->phase_error < -liquid_Q32_PI_BY_2)
            _demod->phase_error += liquid_Q32_PI;
        break;
    case MOD_QPSK:
        _demod->phase_error = liquid_atan2_q32(_demod->state_q, _demod->state_i);

        // fold phase error angle onto Q1 projection, 0 <= phase_error <= pi/2
        if (_demod->phase_error < 0 )
            _demod->phase_error += liquid_Q32_PI;
        if (_demod->phase_error > liquid_Q32_PI_BY_2)
            _demod->phase_error -= liquid_Q32_PI_BY_2;
        _demod->phase_error -= liquid_Q32_PI_BY_4;
        break;
    case MOD_ASK:
        if (_demod->state_i > 0)
            _demod->phase_error =  _demod->state_q;
        else
            _demod->phase_error = -_demod->state_q;
        break;
    case MOD_QAM:
    case MOD_ARB:
    case MOD_ARB_MIRRORED:
    case MOD_ARB_ROTATED:
        _demod->phase_error = liquid_atan2_q32(_demod->res_q, _demod->res_i);
        break;
    default:;
    }

    *_phi = _demod->phase_error;
}


// get error vector magnitude
void get_demodulator_evm_q32(modem_q32* _demod, q32_t* _evm)
{
    switch (_demod->scheme) {
    case MOD_PSK:
    case MOD_BPSK:
    case MOD_QPSK:
    case MOD_DPSK:
    case MOD_ASK:
    case MOD_QAM:
    case MOD_ARB:
    case MOD_ARB_MIRRORED:
    case MOD_ARB_ROTATED:
        *_evm = liquid_cabs_q32(_demod->res_q, _demod->res_i);
        break;
    default:;
    }

    *_evm = _demod->evm;
}

void demodulate_linear_array_ref_q32(
    q32_t _v,
    unsigned int _m,
    q32_t *_ref,
    unsigned int *_s,
    q32_t *_res)
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

