//
//
//

#include <stdio.h>
#include <math.h>
#include "modem_internal.h"

void modulate(
    modem _mod,
    unsigned int symbol_in,
    float complex *y)
{
    switch (_mod->scheme) {
    case MOD_PAM:
        modulate_pam(_mod, symbol_in, y);
        break;
    case MOD_QAM:
        modulate_qam(_mod, symbol_in, y);
        break;
    case MOD_PSK:
        modulate_psk(_mod, symbol_in, y);
        break;
    case MOD_BPSK:
        modulate_bpsk(_mod, symbol_in, y);
        break;
    case MOD_QPSK:
        modulate_qpsk(_mod, symbol_in, y);
        break;
    case MOD_DPSK:
        modulate_dpsk(_mod, symbol_in, y);
        break;
    case MOD_ARB:
    case MOD_ARB_MIRRORED:
    case MOD_ARB_ROTATED:
        modulate_arb(_mod, symbol_in, y);
        break;
    default:
        perror("ERROR: modulate, unknown/unsupported modulation scheme\n");
        break;
    }
}

void modulate_pam(
    modem _mod,
    unsigned int symbol_in,
    float complex *y)
{
    symbol_in = gray_decode(symbol_in);
    *y = (2*(int)symbol_in - (int)(_mod->M) + 1) * _mod->alpha;
}

void modulate_qam(
    modem _mod,
    unsigned int symbol_in,
    float complex *y)
{
    unsigned int s_i, s_q;
    s_i = symbol_in >> _mod->m_q;
    s_q = symbol_in & ( (1<<_mod->m_q)-1 );

    s_i = gray_decode(s_i);
    s_q = gray_decode(s_q);

    *y = (2*(int)s_i - (int)(_mod->M_i) + 1) * _mod->alpha +
         (2*(int)s_q - (int)(_mod->M_q) + 1) * _mod->alpha * J;
}

void modulate_psk(
    modem _mod,
    unsigned int symbol_in,
    float complex *y)
{
    symbol_in = gray_decode(symbol_in);
    ///\todo: combine into single statement
    float theta = symbol_in * 2 * _mod->alpha;
    *y = cexpf(J*theta);
}

void modulate_bpsk(
    modem _mod,
    unsigned int symbol_in,
    float complex *y)
{
    *y = symbol_in ? -1.0f : 1.0f;
}

void modulate_qpsk(
    modem _mod,
    unsigned int symbol_in,
    float complex *y)
{
    *y  = symbol_in & 0x01 ? -0.707106781f   : 0.707106781f;
    *y += symbol_in & 0x02 ? -0.707106781f*J : 0.707106781f*J;
}

void modulate_dpsk(
    modem _mod,
    unsigned int symbol_in,
    float complex *y)
{
    symbol_in = gray_decode(symbol_in);
    float d_theta = symbol_in * 2 * _mod->alpha;
    _mod->state_theta += d_theta;
    _mod->state_theta -= (_mod->state_theta > 2*M_PI) ? 2*M_PI : 0.0f;
    ///\todo: combine into single statement
    *y = cexpf(J*(_mod->state_theta));

    _mod->state = *y;
    //printf("mod: state_theta = %f\n", _mod->state_theta);
}

void modulate_arb(
    modem _mod,
    unsigned int symbol_in,
    float complex *y)
{
    if (symbol_in >= _mod->M) {
        perror("ERROR: modulate_arb(), input symbol exceeds maximum\n");
        return;
    }

    *y = _mod->symbol_map[symbol_in]; 
}

#if 0
void modulate_arb_mirrored(modem _mod, unsigned int symbol_in, float complex *y)
{
    if (symbol_in >= _mod->M) {
        perror("ERROR: modulate_arb_mirrored(), input symbol exceeds maximum\n");
        return;
    }

    // IQ:
    // -----
    // 01 00
    // 11 10
    unsigned int quadrant = symbol_in >> (_mod->m - 2);
    symbol_in &= _mod->M - 1;

    *y = _mod->symbol_map[symbol_in]; 

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
#endif

#if 0
void modulate_arb_rotated(modem _mod, unsigned int symbol_in, float complex *y)
{
    if (symbol_in >= _mod->M) {
        perror("ERROR: modulate_arb_rotated(), input symbol exceeds maximum\n");
        return;
    }

    // IQ:
    // -----
    // 01 00
    // 11 10
    unsigned int quadrant = symbol_in >> (_mod->m - 2);
    symbol_in &= _mod->M - 1;

    *y = _mod->symbol_map[symbol_in]; 

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
#endif



