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

// 
// AUTOTEST: csqrtf
//
void autotest_csqrtf()
{
    float tol = 1e-3f;

    unsigned int n = 32;
    float complex z[32] = {
        1.3608e+00+_Complex_I* -4.2247e-01,   1.1324e+00+_Complex_I*  1.1938e+00,
        1.6466e+00+_Complex_I* -1.2098e+00,  -6.5911e-01+_Complex_I*  1.0729e+00,
       -8.8890e-01+_Complex_I*  2.1588e-01,  -9.0412e-02+_Complex_I*  5.1548e-01,
       -5.4086e-01+_Complex_I*  5.3604e-02,   1.8089e+00+_Complex_I*  1.6648e+00,
        5.4285e-01+_Complex_I*  8.6919e-01,  -1.4336e+00+_Complex_I*  4.2788e-01,
       -1.9348e+00+_Complex_I* -1.0285e+00,  -1.4511e+00+_Complex_I*  1.2167e+00,
       -1.3733e+00+_Complex_I* -3.9622e-01,  -1.4808e+00+_Complex_I* -1.5648e+00,
        1.9957e+00+_Complex_I* -1.1270e+00,   5.1730e-02+_Complex_I*  1.3564e+00,
        4.5056e-01+_Complex_I* -8.1587e-01,   5.5021e-01+_Complex_I*  9.7149e-02,
       -2.5668e-02+_Complex_I*  1.8911e+00,  -8.2993e-01+_Complex_I*  1.0854e+00,
        1.0698e-01+_Complex_I*  1.0797e+00,  -3.9909e-01+_Complex_I*  1.5661e+00,
       -8.6674e-01+_Complex_I* -5.9017e-01,   1.2309e+00+_Complex_I*  1.6761e+00,
       -1.7210e+00+_Complex_I*  1.7973e+00,   1.0398e-01+_Complex_I* -1.6558e+00,
       -1.2311e+00+_Complex_I*  6.5291e-01,   1.5609e+00+_Complex_I* -6.0443e-01,
       -1.7433e+00+_Complex_I* -1.9199e+00,  -1.6919e-01+_Complex_I* -1.7476e+00,
       -1.0469e+00+_Complex_I*  1.8825e+00,   1.6088e+00+_Complex_I*  1.4037e+00};

    float complex test[32] = {
        1.1802e+00+_Complex_I* -1.7899e-01,   1.1785e+00+_Complex_I*  5.0647e-01,
        1.3583e+00+_Complex_I* -4.4534e-01,   5.4776e-01+_Complex_I*  9.7936e-01,
        1.1366e-01+_Complex_I*  9.4964e-01,   4.6526e-01+_Complex_I*  5.5397e-01,
        3.6399e-02+_Complex_I*  7.3633e-01,   1.4607e+00+_Complex_I*  5.6986e-01,
        8.8533e-01+_Complex_I*  4.9088e-01,   1.7676e-01+_Complex_I*  1.2103e+00,
        3.5802e-01+_Complex_I* -1.4363e+00,   4.7042e-01+_Complex_I*  1.2932e+00,
        1.6736e-01+_Complex_I* -1.1838e+00,   5.8032e-01+_Complex_I* -1.3482e+00,
        1.4642e+00+_Complex_I* -3.8485e-01,   8.3939e-01+_Complex_I*  8.0799e-01,
        8.3144e-01+_Complex_I* -4.9064e-01,   7.4462e-01+_Complex_I*  6.5233e-02,
        9.6582e-01+_Complex_I*  9.7902e-01,   5.1789e-01+_Complex_I*  1.0479e+00,
        7.7199e-01+_Complex_I*  6.9927e-01,   7.8009e-01+_Complex_I*  1.0038e+00,
        3.0154e-01+_Complex_I* -9.7860e-01,   1.2866e+00+_Complex_I*  6.5140e-01,
        6.1944e-01+_Complex_I*  1.4508e+00,   9.3889e-01+_Complex_I* -8.8178e-01,
        2.8497e-01+_Complex_I*  1.1456e+00,   1.2718e+00+_Complex_I* -2.3763e-01,
        6.5191e-01+_Complex_I* -1.4725e+00,   8.9067e-01+_Complex_I* -9.8107e-01,
        7.4403e-01+_Complex_I*  1.2651e+00,   1.3682e+00+_Complex_I*  5.1297e-01};

    unsigned int i;
    for (i=0; i<n; i++) {
        float complex t = liquid_csqrtf(z[i]);

        CONTEND_DELTA(crealf(t), crealf(test[i]), tol);
        CONTEND_DELTA(cimagf(t), cimagf(test[i]), tol);
    }
}

// 
// AUTOTEST: casinf
//
void autotest_casinf()
{
    float tol = 1e-3f;

    unsigned int n = 32;

    float complex z[32] = {
        1.3608e+00+_Complex_I* -4.2247e-01,   1.1324e+00+_Complex_I*  1.1938e+00,
        1.6466e+00+_Complex_I* -1.2098e+00,  -6.5911e-01+_Complex_I*  1.0729e+00,
       -8.8890e-01+_Complex_I*  2.1588e-01,  -9.0412e-02+_Complex_I*  5.1548e-01,
       -5.4086e-01+_Complex_I*  5.3604e-02,   1.8089e+00+_Complex_I*  1.6648e+00,
        5.4285e-01+_Complex_I*  8.6919e-01,  -1.4336e+00+_Complex_I*  4.2788e-01,
       -1.9348e+00+_Complex_I* -1.0285e+00,  -1.4511e+00+_Complex_I*  1.2167e+00,
       -1.3733e+00+_Complex_I* -3.9622e-01,  -1.4808e+00+_Complex_I* -1.5648e+00,
        1.9957e+00+_Complex_I* -1.1270e+00,   5.1730e-02+_Complex_I*  1.3564e+00,
        4.5056e-01+_Complex_I* -8.1587e-01,   5.5021e-01+_Complex_I*  9.7149e-02,
       -2.5668e-02+_Complex_I*  1.8911e+00,  -8.2993e-01+_Complex_I*  1.0854e+00,
        1.0698e-01+_Complex_I*  1.0797e+00,  -3.9909e-01+_Complex_I*  1.5661e+00,
       -8.6674e-01+_Complex_I* -5.9017e-01,   1.2309e+00+_Complex_I*  1.6761e+00,
       -1.7210e+00+_Complex_I*  1.7973e+00,   1.0398e-01+_Complex_I* -1.6558e+00,
       -1.2311e+00+_Complex_I*  6.5291e-01,   1.5609e+00+_Complex_I* -6.0443e-01,
       -1.7433e+00+_Complex_I* -1.9199e+00,  -1.6919e-01+_Complex_I* -1.7476e+00,
       -1.0469e+00+_Complex_I*  1.8825e+00,   1.6088e+00+_Complex_I*  1.4037e+00 };

    float complex test[32] = {
        1.1716e+00+_Complex_I* -9.4147e-01,   6.7048e-01+_Complex_I*  1.2078e+00,
        8.7747e-01+_Complex_I* -1.3947e+00,  -4.3898e-01+_Complex_I*  1.0065e+00,
       -9.7768e-01+_Complex_I*  3.7722e-01,  -8.0395e-02+_Complex_I*  4.9650e-01,
       -5.7016e-01+_Complex_I*  6.3633e-02,   7.8546e-01+_Complex_I*  1.5918e+00,
        4.0539e-01+_Complex_I*  8.4256e-01,  -1.1968e+00+_Complex_I*  9.9741e-01,
       -1.0352e+00+_Complex_I* -1.4505e+00,  -8.0295e-01+_Complex_I*  1.3267e+00,
       -1.1968e+00+_Complex_I* -9.3995e-01,  -7.0503e-01+_Complex_I* -1.4678e+00,
        1.0130e+00+_Complex_I* -1.4999e+00,   3.0692e-02+_Complex_I*  1.1128e+00,
        3.4725e-01+_Complex_I* -7.8464e-01,   5.7823e-01+_Complex_I*  1.1575e-01,
       -1.1998e-02+_Complex_I*  1.3939e+00,  -5.4040e-01+_Complex_I*  1.0574e+00,
        7.2656e-02+_Complex_I*  9.3853e-01,  -2.1290e-01+_Complex_I*  1.2502e+00,
       -7.4285e-01+_Complex_I* -7.3366e-01,   5.8143e-01+_Complex_I*  1.4462e+00,
       -7.2379e-01+_Complex_I*  1.6089e+00,   5.3725e-02+_Complex_I* -1.2794e+00,
       -9.5454e-01+_Complex_I*  9.7013e-01,   1.1275e+00+_Complex_I* -1.1433e+00,
       -7.0078e-01+_Complex_I* -1.6516e+00,  -8.3905e-02+_Complex_I* -1.3278e+00,
       -4.6555e-01+_Complex_I*  1.4904e+00,   7.9838e-01+_Complex_I*  1.4487e+00 };

    unsigned int i;
    for (i=0; i<n; i++) {
        float complex t = liquid_casinf(z[i]);

        CONTEND_DELTA(crealf(t), crealf(test[i]), tol);
        CONTEND_DELTA(cimagf(t), cimagf(test[i]), tol);
    }
}
// 
// AUTOTEST: cacosf
//
void autotest_cacosf()
{
    float tol = 1e-3f;

    unsigned int n = 32;

    float complex z[32] = {
        1.3608e+00+_Complex_I* -4.2247e-01,   1.1324e+00+_Complex_I*  1.1938e+00,
        1.6466e+00+_Complex_I* -1.2098e+00,  -6.5911e-01+_Complex_I*  1.0729e+00,
       -8.8890e-01+_Complex_I*  2.1588e-01,  -9.0412e-02+_Complex_I*  5.1548e-01,
       -5.4086e-01+_Complex_I*  5.3604e-02,   1.8089e+00+_Complex_I*  1.6648e+00,
        5.4285e-01+_Complex_I*  8.6919e-01,  -1.4336e+00+_Complex_I*  4.2788e-01,
       -1.9348e+00+_Complex_I* -1.0285e+00,  -1.4511e+00+_Complex_I*  1.2167e+00,
       -1.3733e+00+_Complex_I* -3.9622e-01,  -1.4808e+00+_Complex_I* -1.5648e+00,
        1.9957e+00+_Complex_I* -1.1270e+00,   5.1730e-02+_Complex_I*  1.3564e+00,
        4.5056e-01+_Complex_I* -8.1587e-01,   5.5021e-01+_Complex_I*  9.7149e-02,
       -2.5668e-02+_Complex_I*  1.8911e+00,  -8.2993e-01+_Complex_I*  1.0854e+00,
        1.0698e-01+_Complex_I*  1.0797e+00,  -3.9909e-01+_Complex_I*  1.5661e+00,
       -8.6674e-01+_Complex_I* -5.9017e-01,   1.2309e+00+_Complex_I*  1.6761e+00,
       -1.7210e+00+_Complex_I*  1.7973e+00,   1.0398e-01+_Complex_I* -1.6558e+00,
       -1.2311e+00+_Complex_I*  6.5291e-01,   1.5609e+00+_Complex_I* -6.0443e-01,
       -1.7433e+00+_Complex_I* -1.9199e+00,  -1.6919e-01+_Complex_I* -1.7476e+00,
       -1.0469e+00+_Complex_I*  1.8825e+00,   1.6088e+00+_Complex_I*  1.4037e+00,
      };

    float complex test[32] = {
        3.9923e-01+_Complex_I*  9.4147e-01,   9.0032e-01+_Complex_I* -1.2078e+00,
        6.9333e-01+_Complex_I*  1.3947e+00,   2.0098e+00+_Complex_I* -1.0065e+00,
        2.5485e+00+_Complex_I* -3.7722e-01,   1.6512e+00+_Complex_I* -4.9650e-01,
        2.1410e+00+_Complex_I* -6.3633e-02,   7.8534e-01+_Complex_I* -1.5918e+00,
        1.1654e+00+_Complex_I* -8.4256e-01,   2.7676e+00+_Complex_I* -9.9741e-01,
        2.6060e+00+_Complex_I*  1.4505e+00,   2.3737e+00+_Complex_I* -1.3267e+00,
        2.7676e+00+_Complex_I*  9.3995e-01,   2.2758e+00+_Complex_I*  1.4678e+00,
        5.5780e-01+_Complex_I*  1.4999e+00,   1.5401e+00+_Complex_I* -1.1128e+00,
        1.2235e+00+_Complex_I*  7.8464e-01,   9.9257e-01+_Complex_I* -1.1575e-01,
        1.5828e+00+_Complex_I* -1.3939e+00,   2.1112e+00+_Complex_I* -1.0574e+00,
        1.4981e+00+_Complex_I* -9.3853e-01,   1.7837e+00+_Complex_I* -1.2502e+00,
        2.3137e+00+_Complex_I*  7.3366e-01,   9.8936e-01+_Complex_I* -1.4462e+00,
        2.2946e+00+_Complex_I* -1.6089e+00,   1.5171e+00+_Complex_I*  1.2794e+00,
        2.5253e+00+_Complex_I* -9.7013e-01,   4.4330e-01+_Complex_I*  1.1433e+00,
        2.2716e+00+_Complex_I*  1.6516e+00,   1.6547e+00+_Complex_I*  1.3278e+00,
        2.0363e+00+_Complex_I* -1.4904e+00,   7.7241e-01+_Complex_I* -1.4487e+00};

    unsigned int i;
    for (i=0; i<n; i++) {
        float complex t = liquid_cacosf(z[i]);

        CONTEND_DELTA(crealf(t), crealf(test[i]), tol);
        CONTEND_DELTA(cimagf(t), cimagf(test[i]), tol);
    }
}
