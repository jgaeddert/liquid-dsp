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
// Design root raised-cosine filter
//

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

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
                    \sin\left[(1-\beta)\pi z\right] / (4\beta z) }
                  { \pi \sqrt{T}\left[ 1-16\beta^2z^2\right] }
  \f]

  where \f$z=n/k-m\f$, and \f$T=1\f$ for most cases.

  The function compensates for the two cases where \f$h[n]\f$ might be
  undefined in the above equation, viz.

  \f[
    \mathop {\lim }\limits_{z \to 0 } h(z) = 1 - \beta + 4\beta/\pi
  \f]

  and

  \f[
    \mathop {\lim }\limits_{z \to \pm \frac{1}{4\beta} } h(z) =
        \frac{\beta}{\sqrt{2}}
        \left[
            \left(1 + \frac{2}{\pi}\right)\sin\left(\frac{\pi}{4\beta}\right) +
            \left(1 - \frac{2}{\pi}\right)\cos\left(\frac{\pi}{4\beta}\right)
        \right]
  \f]

  \param[in]  _k         samples per symbol
  \param[in]  _m         symbol delay
  \param[in]  _beta      excess bandwidth/rolloff factor ( 0 < _beta < 1 )
  \param[in]  _h         pointer to filter coefficients

 */

void design_rrc_filter(
  unsigned int _k,
  unsigned int _m,
  float _beta,
  float _dt,
  float * _h
)
{
    unsigned int h_len;

    if ( _k < 1 ) {
        fprintf(stderr,"error: design_rrc_filter(): k must be greater than 0\n");
        exit(1);
    } else if ( _m < 1 ) {
        fprintf(stderr,"error: design_rrc_filter(): m must be greater than 0\n");
        exit(1);
    } else if ( (_beta < 0.0f) || (_beta > 1.0f) ) {
        fprintf(stderr,"error: design_rrc_filter(): beta must be in [0,1]\n");
        exit(1);
    } else;

    unsigned int n;
    float z, t1, t2, t3, t4, T=1.0f;

    float nf, kf, mf;

    h_len = 2*_k*_m + 1;

    // Calculate filter coefficients
    for (n=0; n<h_len; n++) {
        nf = (float) n;
        kf = (float) _k;
        mf = (float) _m;

        z = (nf+_dt)/kf-mf;
        t1 = cosf((1+_beta)*M_PI*z);
        t2 = sinf((1-_beta)*M_PI*z);

        // Check for special condition where z equals zero
        if ( fabsf(z) < 1e-5 ) {
            _h[n] = 1 - _beta + 4*_beta/M_PI;
        } else {
            t3 = 1/((4*_beta*z));

            float g = 1-16*_beta*_beta*z*z;
            g *= g;

            // Check for special condition where 16*_beta^2*z^2 equals 1
            if ( g < 1e-5 ) {
                float g1, g2, g3, g4;
                g1 = 1 + 2.0f/M_PI;
                g2 = sinf(0.25f*M_PI/_beta);
                g3 = 1 - 2.0f/M_PI;
                g4 = cosf(0.25f*M_PI/_beta);
                _h[n] = _beta/sqrtf(2.0f)*(g1*g2 + g3*g4);
            } else {
                t4 = 4*_beta/(M_PI*sqrtf(T)*(1-(16*_beta*_beta*z*z)));
                _h[n] = t4*( t1 + (t2*t3) );
            }
        }
    }
}

