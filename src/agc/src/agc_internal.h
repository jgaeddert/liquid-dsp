// 
// Automatic gain control
//

#ifndef __LIQUID_AGC_INTERNAL_H__
#define __LIQUID_AGC_INTERNAL_H__

#include <complex.h>
#include "liquid.h"

//-----------------------------------------------------------------------------
//
// Automatic Gain Control
//
//-----------------------------------------------------------------------------
/* \brief Automatic gain control signal processor
 *
 * The automatic gain control (AGC) signal processor is a variable gain
 * amplifier to automatically scale an input signal to match a target
 * energy.
 *
 * The AGC uses a first-order open loop architecture that estimates the
 * input signal energy and ajusts its gain accordingly.  The loop uses
 * a first-order low-pass (non-integrating) Butterworth filter prototype
 * with the bilinear z-transform.  The coefficients are calculated as
 *   \f[ \alpha = \frac{sin(\theta)-cos(\theta)}{sin(\theta)+cos(\theta)} \f]
 *   \f[ \beta  = \frac{sin(\theta)}{sin(\theta)+cos(\theta)} \f]
 * where \f$\theta = \pi BT / 2 = \pi f_c / f_s\f$ is related to the
 * open-loop filter bandwidth.
 *
 * For relatively small bandwidths, the filter coefficients can be
 * approximated as
 *   \f[ \alpha \approx \pi BT - 1 \f]
 *   \f[ \beta  \approx \pi BT / 2 \f]
 *
 * \cite  R. G. Lyons, Understanding Digital Signal Processing, 2nd ed. New
 * Jersey: Prentice Hall, 2004.
 */
struct agc_s {
    float e;            // estimated signal energy
    float e_target;     // target signal energy

    // gain variables
    float g;            // current gain value
    float g_min;        // minimum gain value
    float g_max;        // maximum gain value

    // loop filter parameters
    float BT;           // bandwidth-time constant
    float alpha;        // feed-back gain
    float beta;         // feed-forward gain

    // loop filter state variables
    float e_prime;
    float e_hat;        // filtered energy estimate
    float tmp2;

    fir_filter_rrrf f;
};

#endif // __LIQUID_AGC_INTERNAL_H__


