/*
 * Copyright (c) 2011 Joseph Gaeddert
 * Copyright (c) 2011 Virginia Polytechnic Institute & State University
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
// optimal 64-QAM
//

#include <complex.h>

// optimal 64-QAM
const float complex modem_arb64opt[64] = {
     -9.6048e-01 +  -1.0031e+00*_Complex_I,
     -1.1105e+00 +  -6.8896e-01*_Complex_I,
     -1.1029e+00 +  -9.5064e-02*_Complex_I,
     -1.2692e+00 +  -3.8774e-01*_Complex_I,
     -7.9322e-01 +   1.1014e+00*_Complex_I,
     -9.5999e-01 +   7.9701e-01*_Complex_I,
     -1.2734e+00 +   1.9621e-01*_Complex_I,
     -1.1121e+00 +   4.9520e-01*_Complex_I,
     -6.2388e-01 +  -9.8614e-01*_Complex_I,
     -7.7108e-01 +  -6.8653e-01*_Complex_I,
     -7.6793e-01 +  -1.0097e-01*_Complex_I,
     -9.1812e-01 +  -3.9210e-01*_Complex_I,
     -4.5416e-01 +   1.0712e+00*_Complex_I,
     -6.1578e-01 +   7.7701e-01*_Complex_I,
     -9.1845e-01 +   1.9699e-01*_Complex_I,
     -7.7026e-01 +   4.8601e-01*_Complex_I,
     -1.1067e-01 +  -1.2612e+00*_Complex_I,
     -2.6544e-01 +  -9.6343e-01*_Complex_I,
      1.3076e+00 +   4.5615e-01*_Complex_I,
     -2.5827e-01 +  -3.9458e-01*_Complex_I,
     -1.0044e-01 +   1.0486e+00*_Complex_I,
      3.9086e-02 +   7.4394e-01*_Complex_I,
     -2.6144e-01 +   1.7968e-01*_Complex_I,
     -1.1912e-01 +   4.5495e-01*_Complex_I,
     -4.5452e-01 +  -1.2733e+00*_Complex_I,
     -4.3866e-01 +  -6.8570e-01*_Complex_I,
     -4.3802e-01 +  -1.0872e-01*_Complex_I,
     -5.8466e-01 +  -3.9550e-01*_Complex_I,
     -2.6246e-01 +   1.3479e+00*_Complex_I,
     -2.8816e-01 +   7.5672e-01*_Complex_I,
     -5.8784e-01 +   1.8705e-01*_Complex_I,
     -4.4008e-01 +   4.7244e-01*_Complex_I,
      7.9849e-01 +  -1.0124e+00*_Complex_I,
      9.5371e-01 +  -7.1194e-01*_Complex_I,
      1.2696e+00 +  -1.1448e-01*_Complex_I,
      1.1145e+00 +  -4.2441e-01*_Complex_I,
      9.0580e-01 +   1.0653e+00*_Complex_I,
      1.0641e+00 +   7.6363e-01*_Complex_I,
      1.0837e+00 +   1.7467e-01*_Complex_I,
      9.1489e-01 +   4.6389e-01*_Complex_I,
      5.8983e-01 +  -1.2906e+00*_Complex_I,
      6.0746e-01 +  -7.0336e-01*_Complex_I,
      9.1373e-01 +  -1.3004e-01*_Complex_I,
      7.4917e-01 +  -4.1576e-01*_Complex_I,
      5.6608e-01 +   1.0707e+00*_Complex_I,
      7.1464e-01 +   7.5794e-01*_Complex_I,
      7.4460e-01 +   1.6652e-01*_Complex_I,
      5.7594e-01 +   4.5036e-01*_Complex_I,
      7.6128e-02 +  -9.7664e-01*_Complex_I,
     -9.2551e-02 +  -6.8763e-01*_Complex_I,
     -9.4285e-02 +  -1.0993e-01*_Complex_I,
      6.5368e-02 +  -4.0109e-01*_Complex_I,
      1.0945e-01 +   1.3350e+00*_Complex_I,
      2.2470e-01 +   4.7166e-01*_Complex_I,
      5.5987e-01 +  -1.1259e-01*_Complex_I,
      7.0006e-02 +   1.9154e-01*_Complex_I,
      2.4876e-01 +  -1.2685e+00*_Complex_I,
      4.2452e-01 +  -9.8023e-01*_Complex_I,
      2.5356e-01 +  -6.8613e-01*_Complex_I,
      4.0576e-01 +  -4.1184e-01*_Complex_I,
      2.4615e-01 +   1.0373e+00*_Complex_I,
      3.9219e-01 +   7.4440e-01*_Complex_I,
      2.4704e-01 +  -1.0509e-01*_Complex_I,
      3.9791e-01 +   1.8370e-01*_Complex_I,
};
