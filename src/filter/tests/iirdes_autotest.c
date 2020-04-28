/*
 * Copyright (c) 2007 - 2020 Joseph Gaeddert
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

// References
//  [Ziemer:1998] Ziemer, Tranter, Fannin, "Signals & Systems,
//      Continuous and Discrete," 4th ed., Prentice Hall, Upper

#include "autotest/autotest.h"
#include "liquid.internal.h"

// check low-pass elliptical filter design
void testbench_iirdes_ellip_lowpass(unsigned int _n,    // filter order
                                    float        _fc,   // filter cut-off
                                    float        _fs,   // empirical stop-band frequency
                                    float        _Ap,   // pass-band ripple
                                    float        _As)   // stop-band suppression
{
    float        tol  = 1e-3f;  // error tolerance [dB], yes, that's dB
    unsigned int nfft = 200;    // number of points to evaluate

    // design filter from prototype
    iirfilt_crcf q = iirfilt_crcf_create_prototype(
        LIQUID_IIRDES_ELLIP, LIQUID_IIRDES_LOWPASS, LIQUID_IIRDES_SOS,
        _n,_fc,0.0f,_Ap,_As);
    if (liquid_autotest_verbose)
        iirfilt_crcf_print(q);

    //            fc fr  fs
    //             | |   |
    // |**************  . . . . . . . . H0
    // |/\/\/\/\/\/\  *
    // |************\  *. . . . . . . . H1
    // |           * \  *
    // |           *  \  *
    // |           *   \  ************* H2
    // |           *    \ /^\ /^\ /^\ /
    // |           *     |   |   |   |
    // 0           fc    fs
    float gamma = 0.1, fr = _fc + gamma*(_fs - _fc);
    float H0 = 0.0f, H1 = -_Ap, H2 = -_As;

    // compute response and compare to expected or mask
    unsigned int i;
    for (i=0; i<nfft; i++) {
        float f = 0.5f * (float)i / (float)nfft;
        float complex h;
        iirfilt_crcf_freqresponse(q, f, &h);
        float H = 10.*log10f(crealf(h*conjf(h)));

        // determine what portion of the band this is
        if (f < _fc) {
            float mask_hi = H0 + tol;
            float mask_lo = H1 - tol;
            if (liquid_autotest_verbose)
                printf("%6u, f=%6.3f (pass       band): %12.6f < %12.6f < %12.6f\n", i, f, mask_lo, H, mask_hi);
            CONTEND_GREATER_THAN(H, mask_lo);
            CONTEND_LESS_THAN   (H, mask_hi);
        } else if (f < _fs) {
            float mask_hi = (f < fr) ? H0 : H2 + (H2 - H0) / (_fs - fr) * (f - _fs);
            if (liquid_autotest_verbose)
                printf("%6u, f=%6.3f (transition band):                %12.6f < %12.6f\n", i, f, H, mask_hi);
            CONTEND_LESS_THAN(H, mask_hi);
        } else {
            float mask_hi = H2 + tol;
            if (liquid_autotest_verbose)
                printf("%6u, f=%6.3f (stop       band):                %12.6f < %12.6f\n", i, f, H, mask_hi);
            CONTEND_LESS_THAN(H, mask_hi);
        }
    }
    iirfilt_crcf_destroy(q);    // destroy filter object
}

// test different filter designs
void autotest_iirdes_ellip_lowpass_0(){ testbench_iirdes_ellip_lowpass( 5,0.20f,0.30f,1.0f, 60.0f); }
void autotest_iirdes_ellip_lowpass_1(){ testbench_iirdes_ellip_lowpass( 5,0.05f,0.09f,1.0f, 60.0f); }
void autotest_iirdes_ellip_lowpass_2(){ testbench_iirdes_ellip_lowpass( 5,0.20f,0.43f,1.0f,100.0f); }
void autotest_iirdes_ellip_lowpass_3(){ testbench_iirdes_ellip_lowpass( 5,0.20f,0.40f,0.1f, 60.0f); }
void autotest_iirdes_ellip_lowpass_4(){ testbench_iirdes_ellip_lowpass(15,0.35f,0.37f,0.1f,120.0f); }

// design specific 2nd-order butterworth filter and compare to known coefficients;
// design comes from [Ziemer:1998], Example 9-7, pp. 440--442
void autotest_iirdes_butter_2()
{
    // design butterworth filter
    float a[3];
    float b[3];
    liquid_iirdes(LIQUID_IIRDES_BUTTER,
                  LIQUID_IIRDES_LOWPASS,
                  LIQUID_IIRDES_TF,
                  2,        // order
                  0.25f,    // fc, normalized cut-off frequency
                  0.0f,     // f0,center frequency (ignored for low-pass filter)
                  1.0f,     // Ap, pass-band ripple (ignored for Butterworth)
                  40.0f,    // As, stop-band attenuation (ignored for Butterworth)
                  b, a);    // output coefficients

    // initialize pre-determined coefficient array
    // for 2^nd-order low-pass Butterworth filter
    // with cutoff frequency 0.25
    float a_test[3] = {1.0f,               0.0f,               0.171572875253810f};
    float b_test[3] = {0.292893218813452f, 0.585786437626905f, 0.292893218813452f};

    // Ensure data are equal to within tolerance
    float tol = 1e-6f;      // error tolerance
    unsigned int i;
    for (i=0; i<3; i++) {
        CONTEND_DELTA( b[i], b_test[i], tol );
        CONTEND_DELTA( a[i], a_test[i], tol );
    }
}

