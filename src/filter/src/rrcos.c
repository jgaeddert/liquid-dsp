//
// Design root raised-cosine filter
//

#include <math.h>
#include <stdio.h>

#include "rrcos.h"

/** \brief Calculate square-root raised-cosine filter coefficients

  design_rrc_filter calculates the coefficients for a square-root raised-cosine
  (RRC) finite impulse response (FIR) filter commonly used in digital
  communications.  The input parameters are as follows

    - \f$k\f$ : samples per symbol
    - \f$m\f$ : sample delay
    - \f$\beta\f$ : excess bandwidth (rolloff) factor

  The function returns the filter coefficients in the buffer, h, which has been
  allocated externally.  The length of the filter is always

  \f[
    h_{len} = 2 k m + 1
  \f]

  The filter coefficients themselves are derived from the following equation

  \f[ 
    h\left[z\right] =
      4\beta \frac{ \cos\left[(1+\beta)\pi z\right] +
                    \sin\left[(1+\beta)\pi z\right] / (4\beta z) }
                  { \pi \sqrt{T}\left[ 1-16\beta^2z^2\right] }
  \f]

  where \f$z=n/k-m\f$, and \f$T=1\f$ for most cases.

  The function compensates for the two cases where \f$h[n]\f$ might be
  undefined in the above equation, viz.

  \f[
    \mathop {\lim }\limits_{z \to 0 } h(z) =
      \frac{ 4\beta \left[ 1 + \frac{1-\beta\pi }{ 4\beta } \right] }
           { \pi\sqrt{T}\left( 1-16\beta^2 z^2 \right) }
  \f]

  and

  \f[
    \mathop {\lim }\limits_{z \to \pm \frac{1}{4\beta} } h(z) =
        \frac{(1+\beta)}{2\pi}\sin\left[\frac{(1+\beta)\pi}{4\beta}\right]
      - \frac{(1-\beta)}{2}\cos\left[\frac{(1-\beta)\pi}{4\beta}\right]
      + \frac{2\beta}{\pi}\sin\left[\frac{(1-\beta)\pi}{4\beta}\right]

  \f]

  \param[in]  k         samples per symbol
  \param[in]  m         symbol delay
  \param[in]  beta      excess bandwidth/rolloff factor ( 0 < beta < 1 )
  \param[in]  h         pointer to filter coefficients

 */

void design_rrc_filter(
  unsigned int k,      // samples per symbol
  unsigned int m,      // delay
  float beta,          // rolloff factor ( 0 < beta <= 1 )
  float dt,
  float * h            // pointer to filter coefficients
)
{
    unsigned int h_len;

    if ( k < 1 ) {
        printf("ERROR: design_rrc_filter: k must be greater than 0\n");
        return;
    } else if ( m < 1 ) {
        printf("ERROR: design_rrc_filter: m must be greater than 0\n");
        return;
    } else if ( (beta < 0.0f) || (beta > 1.0f) ) {
        printf("ERROR: design_rrc_filter: beta must be in [0,1]\n");
        return;
    } else;

    unsigned int n;
    float z, t1, t2, t3, t4, T=1.0f;

    float nf, kf, mf;

    h_len = 2*k*m + 1;

    // Calculate filter coefficients
    for (n=0; n<h_len; n++) {
        nf = (float) n;
        kf = (float) k;
        mf = (float) m;

        z = (nf+dt)/kf-mf;
        t1 = cosf((1+beta)*M_PI*z);
        t2 = sinf((1-beta)*M_PI*z);

        // Check for special condition where z equals zero
        if ( fabsf(z) < 1e-3 ) {
            t4 = 4*beta/(M_PI*sqrtf(T)*(1-(16*beta*beta*z*z)));
            h[n] = t4*( 1 + (1-beta)*M_PI/(4*beta) );
        } else {
            t3 = 1/((4*beta*z));

            float g = 1-16*beta*beta*z*z;
            g *= g;

            // Check for special condition where 16*beta^2*z^2 equals 1
            if ( g < 1e-3 ) {
                float g1, g2, g3, g4;
                g1 = -(1+beta)*M_PI*sinf((1+beta)*M_PI/(4*beta));
                g2 = cosf((1-beta)*M_PI/(4*beta))*(1-beta)*M_PI;
                g3 = -sinf((1-beta)*M_PI/(4*beta))*4*beta;
                g4 = -2*M_PI;

                h[n] = (g1+g2+g3)/g4;
            } else {
                t4 = 4*beta/(M_PI*sqrtf(T)*(1-(16*beta*beta*z*z)));
                h[n] = t4*( t1 + (t2*t3) );
            }
        }
    }
}

