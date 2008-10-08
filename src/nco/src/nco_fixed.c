// 
// Numerically-controlled oscillator, fixed-point precision
// 

#include "nco_fixed.h"

void nco_init_q32(nco_q32 *_nco)
{
    // reset phase state
    _nco->theta = 0;

    // reset frequency
    _nco->d_theta = 0;
}

void nco_set_phase_q32(nco_q32 *_nco, q32_t _phi)
{
    // set internal phase state
    _nco->theta = _phi;

#if NCO_FIXED_CONSTRAIN_PHASE
    nco_constrain_phase_q32(_nco);
#endif
}

void nco_adjust_phase_q32(nco_q32 *_nco, q32_t _dphi)
{
    // adjust internal phase state
    _nco->theta += _dphi;

#if NCO_FIXED_CONSTRAIN_PHASE
    nco_constrain_phase_q32(_nco);
#endif
}

void nco_step_q32(nco_q32 *_nco)
{
    // increment internal phase state
    _nco->theta += _nco->d_theta;

#if NCO_FIXED_CONSTRAIN_PHASE
    nco_constrain_phase_q32(_nco);
#endif
}

void nco_constrain_phase_q32(nco_q32 *_nco)
{
    // constrain internal phase state to be in (-pi,pi)
    if (_nco->theta > Q32_PI)
        _nco->theta -= Q32_2_PI;
    else if (_nco->theta < -Q32_PI)
        _nco->theta += Q32_2_PI;
}

// Rotate input vector array up by NCO angle, \f$\vec{y} = \vec{x}e^{j\theta}\f$
void nco_mix_block_up_q32(
    nco_q32 *_nco,
    q32_t *_xi,
    q32_t *_xq,
    unsigned int _N,
    q32_t *_yi,
    q32_t *_yq)
{
    unsigned int i;

#if 0
    for (i=0; i<_N; i++) {
        nco_mix_up(_nco, _xi[i], _xq[i], &_yi[i], &_yq[i]);
        nco_step(_nco);
    }
#else
    q32_t theta =   _nco->theta;
    q32_t d_theta = _nco->theta;
    for (i=0; i<_N; i++) {
        rotate_q32(_xi[i], _xq[i], theta, &_yi[i], &_yq[i]);

        // nco_step_q32(_nco);
        ///\todo check for overflow
        theta += d_theta;
    }

    // nco_constrain_phase_q32(_nco);
    if (theta > Q32_PI)
        theta -= Q32_2_PI;
    else if (theta < -Q32_PI)
        theta += Q32_2_PI;

    nco_set_phase_q32(_nco, theta);
#endif
}

