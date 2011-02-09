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

#include "autotest/autotest.h"
#include "liquid.h"

// 
// AUTOTEST: clogf
//
void autotest_clogf()
{
    float tol = 1e-3f;

    unsigned int n = 32;
    float complex z[32] = {
        2.7215e+00+_Complex_I* -8.4494e-01,   2.2648e+00+_Complex_I*  2.3875e+00,
        3.2932e+00+_Complex_I* -2.4196e+00,  -1.3182e+00+_Complex_I*  2.1458e+00,
       -1.7778e+00+_Complex_I*  4.3176e-01,  -1.8082e-01+_Complex_I*  1.0310e+00,
       -1.0817e+00+_Complex_I*  1.0721e-01,   3.6178e+00+_Complex_I*  3.3296e+00,
        1.0857e+00+_Complex_I*  1.7384e+00,  -2.8672e+00+_Complex_I*  8.5575e-01,
       -3.8696e+00+_Complex_I* -2.0569e+00,  -2.9021e+00+_Complex_I*  2.4334e+00,
       -2.7466e+00+_Complex_I* -7.9244e-01,  -2.9617e+00+_Complex_I* -3.1295e+00,
        3.9914e+00+_Complex_I* -2.2539e+00,   1.0346e-01+_Complex_I*  2.7129e+00,
        9.0112e-01+_Complex_I* -1.6317e+00,   1.1004e+00+_Complex_I*  1.9430e-01,
       -5.1336e-02+_Complex_I*  3.7822e+00,  -1.6599e+00+_Complex_I*  2.1709e+00,
        2.1396e-01+_Complex_I*  2.1593e+00,  -7.9817e-01+_Complex_I*  3.1322e+00,
       -1.7335e+00+_Complex_I* -1.1803e+00,   2.4618e+00+_Complex_I*  3.3522e+00,
       -3.4420e+00+_Complex_I*  3.5946e+00,   2.0796e-01+_Complex_I* -3.3116e+00,
       -2.4623e+00+_Complex_I*  1.3058e+00,   3.1219e+00+_Complex_I* -1.2089e+00,
       -3.4866e+00+_Complex_I* -3.8398e+00,  -3.3839e-01+_Complex_I* -3.4952e+00,
       -2.0938e+00+_Complex_I*  3.7651e+00,   3.2177e+00+_Complex_I*  2.8074e+00};

    float complex test[32] = {
        1.0472e+00+_Complex_I* -3.0103e-01,   1.1911e+00+_Complex_I*  8.1177e-01,
        1.4077e+00+_Complex_I* -6.3365e-01,   9.2362e-01+_Complex_I*  2.1217e+00,
        6.0403e-01+_Complex_I*  2.9033e+00,   4.5647e-02+_Complex_I*  1.7444e+00,
        8.3443e-02+_Complex_I*  3.0428e+00,   1.5927e+00+_Complex_I*  7.4393e-01,
        7.1762e-01+_Complex_I*  1.0125e+00,   1.0960e+00+_Complex_I*  2.8515e+00,
        1.4776e+00+_Complex_I* -2.6530e+00,   1.3317e+00+_Complex_I*  2.4438e+00,
        1.0503e+00+_Complex_I* -2.8607e+00,   1.4607e+00+_Complex_I* -2.3286e+00,
        1.5225e+00+_Complex_I* -5.1406e-01,   9.9874e-01+_Complex_I*  1.5327e+00,
        6.2274e-01+_Complex_I* -1.0662e+00,   1.1104e-01+_Complex_I*  1.7477e-01,
        1.3304e+00+_Complex_I*  1.5844e+00,   1.0053e+00+_Complex_I*  2.2236e+00,
        7.7467e-01+_Complex_I*  1.4720e+00,   1.1732e+00+_Complex_I*  1.8203e+00,
        7.4059e-01+_Complex_I* -2.5438e+00,   1.4253e+00+_Complex_I*  9.3737e-01,
        1.6048e+00+_Complex_I*  2.3345e+00,   1.1994e+00+_Complex_I* -1.5081e+00,
        1.0250e+00+_Complex_I*  2.6540e+00,   1.2083e+00+_Complex_I* -3.6944e-01,
        1.6461e+00+_Complex_I* -2.3080e+00,   1.2561e+00+_Complex_I* -1.6673e+00,
        1.4605e+00+_Complex_I*  2.0783e+00,   1.4517e+00+_Complex_I*  7.1740e-01};

    unsigned int i;
    for (i=0; i<n; i++) {
        float complex t = liquid_clogf(z[i]);

        CONTEND_DELTA(crealf(t), crealf(test[i]), tol);
        CONTEND_DELTA(cimagf(t), cimagf(test[i]), tol);
    }
}
