//
//
//

#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>

#include "modem_common.h"
#include "modem.h"
#include "modem_internal.h"

modem modem_create(
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
    case MOD_PAM:
        return modem_create_pam(_bits_per_symbol);
    case MOD_QAM:
        return modem_create_qam(_bits_per_symbol);
    case MOD_PSK:
        return modem_create_psk(_bits_per_symbol);
    case MOD_BPSK:
        return modem_create_bpsk();
    case MOD_QPSK:
        return modem_create_qpsk();
    case MOD_DPSK:
        return modem_create_dpsk(_bits_per_symbol);
    case MOD_ARB:
        return modem_create_arb(_bits_per_symbol);
    case MOD_ARB_MIRRORED:
        return modem_create_arb_mirrored(_bits_per_symbol);
    case MOD_ARB_ROTATED:
        return modem_create_arb_rotated(_bits_per_symbol);
    default:
        perror("ERROR: modem_create(), unknown/unsupported modulation scheme\n");
        exit(-1);
    }

    // should never get to this point, but adding return statment
    // to keep compiler happy
    return NULL;
}

void modem_init(modem _mod, unsigned int _bits_per_symbol)
{
    if (_bits_per_symbol < 1 ) {
        perror("ERROR! modem_init(), modem must have at least 1 bit/symbol\n");
        return;
    } else if (_bits_per_symbol > MAX_MOD_BITS_PER_SYMBOL) {
        perror("ERROR! modem_init(), maximum number of bits per symbol exceeded\n");
        return;
    }

    _mod->m = _bits_per_symbol;
    _mod->M = 1 << (_mod->m);
    _mod->m_i = 0;
    _mod->M_i = 0;
    _mod->m_q = 0;
    _mod->M_q = 0;

    _mod->alpha = 0.0f;

    _mod->symbol_map = NULL;

    _mod->state = 0.0f;
    _mod->state_theta = 0.0f;

    _mod->res = 0.0f;

    _mod->phase_error = 0.0f;
    _mod->evm = 0.0f;

    _mod->d_phi = 0.0f;
}

modem modem_create_pam(
    unsigned int _bits_per_symbol)
{
    modem mod = (modem) malloc( sizeof(struct modem_s) );
    mod->scheme = MOD_PAM;

    modem_init(mod, _bits_per_symbol);

    mod->m_i = mod->m;
    mod->M_i = mod->M;

    switch (mod->M) {
    case 2:     mod->alpha = PAM2_ALPHA;     break;
    case 4:     mod->alpha = PAM4_ALPHA;     break;
    case 8:     mod->alpha = PAM8_ALPHA;     break;
    case 16:    mod->alpha = PAM16_ALPHA;    break;
    case 32:    mod->alpha = PAM32_ALPHA;    break;
    default:
        // calculate alpha dynamically
        // NOTE: this is only an approximation
        mod->alpha = sqrtf(3.0f)/(float)(mod->M);
    }

    unsigned int k;
    for (k=0; k<(mod->m); k++)
        mod->ref[k] = (1<<k) * mod->alpha;

    return mod;
}

modem modem_create_qam(
    unsigned int _bits_per_symbol)
{
    if (_bits_per_symbol < 1 ) {
        perror("ERROR! modem_create_qam, modem must have at least 2 bits/symbol\n");
        return NULL;
    }

    modem mod = (modem) malloc( sizeof(struct modem_s) );
    mod->scheme = MOD_QAM;

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

    return mod;
}

modem modem_create_psk(
    unsigned int _bits_per_symbol)
{
    modem mod = (modem) malloc( sizeof(struct modem_s) );
    mod->scheme = MOD_PSK;

    modem_init(mod, _bits_per_symbol);

    mod->alpha = M_PI/(float)(mod->M);

    unsigned int k;
    for (k=0; k<(mod->m); k++)
        mod->ref[k] = (1<<k) * mod->alpha;

    mod->d_phi = M_PI*(1.0f - 1.0f/(float)(mod->M));

    return mod;
}

modem modem_create_bpsk()
{
    modem mod = (modem) malloc( sizeof(struct modem_s) );
    mod->scheme = MOD_BPSK;

    modem_init(mod, 1);

    return mod;
}

modem modem_create_qpsk()
{
    modem mod = (modem) malloc( sizeof(struct modem_s) );
    mod->scheme = MOD_QPSK;

    modem_init(mod, 2);

    return mod;
}

modem modem_create_dpsk(
    unsigned int _bits_per_symbol)
{
    modem mod = (modem) malloc( sizeof(struct modem_s) );
    mod->scheme = MOD_DPSK;

    modem_init(mod, _bits_per_symbol);

    mod->alpha = M_PI/(float)(mod->M);

    unsigned int k;
    for (k=0; k<(mod->m); k++)
        mod->ref[k] = (1<<k) * mod->alpha;

    mod->d_phi = M_PI*(1.0f - 1.0f/(float)(mod->M));

    mod->state = 1.0f;
    mod->state_theta = 0.0f;

    return mod;
}

modem modem_create_arb(
    unsigned int _bits_per_symbol)
{
    modem mod = (modem) malloc( sizeof(struct modem_s) );
    mod->scheme = MOD_ARB;

    modem_init(mod, _bits_per_symbol);

    mod->M = mod->M;
    mod->symbol_map = (float complex*) calloc( mod->M, sizeof(float complex) );

    return mod;
}

modem modem_create_arb_mirrored(
    unsigned int _bits_per_symbol)
{
    modem mod = (modem) malloc( sizeof(struct modem_s) );
    mod->scheme = MOD_ARB_MIRRORED;

    modem_init(mod, _bits_per_symbol);

    /// \bug
    mod->M = (mod->M) >> 2;    // 2^(m-2) = M/4
    mod->symbol_map = (float complex*) calloc( mod->M, sizeof(float complex) );

    return mod;
}

modem modem_create_arb_rotated(
    unsigned int _bits_per_symbol)
{
    modem mod = (modem) malloc( sizeof(struct modem_s) );
    mod->scheme = MOD_ARB_ROTATED;

    modem_init(mod, _bits_per_symbol);

    /// \bug
    mod->M = (mod->M) >> 2;    // 2^(m-2) = M/4
    mod->symbol_map = (float complex*) calloc( mod->M, sizeof(float complex) );

    return mod;
}

void modem_arb_init(modem _mod, float complex *_symbol_map, unsigned int _len)
{
#ifdef LIQUID_VALIDATE_INPUT
    if ( (_mod->scheme != MOD_ARB) && (_mod->scheme != MOD_ARB_MIRRORED) &&
         (_mod->scheme != MOD_ARB_ROTATED) )
    {
        perror("ERROR: modem_arb_init(), modem is not of arbitrary type\n");
        exit(-1);
    } else if (_len != _mod->M) {
        perror("ERROR: modem_arb_init(), array sizes do not match\n");
        exit(-1);
    }
#endif

    unsigned int i;
    for (i=0; i<_len; i++) {
#ifdef LIQUID_VALIDATE_INPUT
        if ((_mod->scheme == MOD_ARB_MIRRORED) || (_mod->scheme == MOD_ARB_ROTATED)) {
            // symbols should only exist in first quadrant
            if ( crealf(_symbol_map[i]) <= 0 || cimagf(_symbol_map[i]) <= 0 )
                printf("WARNING: modem_arb_init(), symbols exist outside first quadrant\n");
        }
#endif

        _mod->symbol_map[i] = _symbol_map[i];
    }

    // balance I/Q channels
    if (_mod->scheme == MOD_ARB)
        modem_arb_balance_iq(_mod);

    // scale modem to have unity energy
    modem_arb_scale(_mod);

}

void modem_arb_init_file(modem _mod, char* filename) {
    // try to open file
    FILE * f = fopen(filename, "r");
    if (f == NULL) {
        perror("modem_arb_init_file(), could not open file\n");
        exit(1);
    }

    unsigned int i, results;
    float sym_i, sym_q;
    for (i=0; i<_mod->M; i++) {
        results = fscanf(f, "%f %f\n", &sym_i, &sym_q);
        _mod->symbol_map[i] = sym_i + J*sym_q;

        // ensure proper number of symbols were read
        if (results < 2) {
            perror("modem_arb_init_file() unable to parse line\n");
            exit(-1);
        }

#ifdef LIQUID_VALIDATE_INPUT
        if ((_mod->scheme == MOD_ARB_MIRRORED) || (_mod->scheme == MOD_ARB_ROTATED)) {
        // symbols should only exist in first quadrant
            if ( sym_i < 0.0f || sym_q < 0.0f )
                printf("WARNING: modem_arb_init_file(), symbols exist outside first quadrant\n");
        }
#endif

    }

    fclose(f);

    // balance I/Q channels
    if (_mod->scheme == MOD_ARB)
        modem_arb_balance_iq(_mod);

    // scale modem to have unity energy
    modem_arb_scale(_mod);
}

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

void modem_arb_balance_iq(modem _mod)
{
    float mean=0.0f;
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

void free_modem(modem _mod)
{
    free(_mod->symbol_map);
    free(_mod);
}


