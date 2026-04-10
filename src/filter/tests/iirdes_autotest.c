/*
 * Copyright (c) 2007 - 2026 Joseph Gaeddert
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

#include "liquid.autotest.h"
#include "liquid.internal.h"

LIQUID_AUTOTEST(iirdes_butter_2,"design specific 2nd-order butterworth filter and compare to known coefficients; design comes from [Ziemer:1998], Example 9-7, pp. 440--442", "", 0.1)
{
    // design butterworth filter
    LIQUID_VLA(float, a, 3);
    LIQUID_VLA(float, b, 3);
    liquid_iirdes(LIQUID_IIRDES_BUTTER,
                  LIQUID_IIRDES_LOWPASS,
                  LIQUID_IIRDES_TF,
                  2,        // order
                  0.25f,    // fc, normalized cut-off frequency
                  0.0f,     // f0,center frequency (ignored for low-pass filter)
                  1.0f,     // Ap, pass-band ripple (ignored for Butterworth)
                  40.0f,    // as, stop-band attenuation (ignored for Butterworth)
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
        LIQUID_CHECK_DELTA( b[i], b_test[i], tol );
        LIQUID_CHECK_DELTA( a[i], a_test[i], tol );
    }
}

// Test infinite impulse response filter design responses
//
//  H(z)
//   ^          fc     fs
//   |           |     |
//   |************. . . . . . . . . . H0
//   |/\/\/\/\/\/\  *
//   |************\  *. . . . . . . . H1
//   |           * \  *
//   |           *  \  *
//   |           *   \  ************* H2
//   |           *    \ /^\ /^\ /^\ /|
//   |           *     |   |   |   | |
//   0           fc    fs            0.5

// check low-pass elliptical filter design
void testbench_iirdes_ellip_lowpass(liquid_autotest __q__,
                                    unsigned int _n,    // filter order
                                    float        _fc,   // filter cut-off
                                    float        _fs,   // empirical stop-band frequency
                                    float        _ap,   // pass-band ripple
                                    float        _as)   // stop-band suppression
{
    float        tol  = 1e-3f;  // error tolerance [dB], yes, that's dB
    unsigned int nfft = 800;    // number of points to evaluate

    // design filter from prototype
    iirfilt_rrrf filter = iirfilt_rrrf_create_prototype(
        LIQUID_IIRDES_ELLIP, LIQUID_IIRDES_LOWPASS, LIQUID_IIRDES_SOS,
        _n,_fc,0.0f,_ap,_as);

    // compute regions for testing
    float H0 = 0.0f, H1 = -_ap, H2 = -_as;

    // verify result
    autotest_psd_s regions[] = {
      {.fmin=0.0f, .fmax=_fc,   .pmin=H1-tol, .pmax=H0+tol, .test_lo=1, .test_hi=1},
      {.fmin=_fs,  .fmax=+0.5f, .pmin=0.0f,      .pmax=H2+tol, .test_lo=0, .test_hi=1},
    };
    liquid_autotest_validate_psd_iirfilt_rrrf(__q__, filter, nfft, regions, 2,
        "autotest/logs/iirdes_ellip_lowpass.m");

    // destroy filter object
    iirfilt_rrrf_destroy(filter);
}

// test different filter designs

LIQUID_AUTOTEST(iirdes_ellip_lowpass_0,"description","",0.1){ testbench_iirdes_ellip_lowpass(__q__,  5,0.20f,0.30f,1.0f, 60.0f); }
LIQUID_AUTOTEST(iirdes_ellip_lowpass_1,"description","",0.1){ testbench_iirdes_ellip_lowpass(__q__,  5,0.05f,0.09f,1.0f, 60.0f); }
LIQUID_AUTOTEST(iirdes_ellip_lowpass_2,"description","",0.1){ testbench_iirdes_ellip_lowpass(__q__,  5,0.20f,0.43f,1.0f,100.0f); }
LIQUID_AUTOTEST(iirdes_ellip_lowpass_3,"description","",0.1){ testbench_iirdes_ellip_lowpass(__q__,  5,0.20f,0.40f,0.1f, 60.0f); }
LIQUID_AUTOTEST(iirdes_ellip_lowpass_4,"description","",0.1){ testbench_iirdes_ellip_lowpass(__q__, 15,0.35f,0.37f,0.1f,120.0f); }

// check low-pass Chebyshev filter design (type 1)
void testbench_iirdes_cheby1_lowpass(liquid_autotest __q__,
                                     unsigned int _n,  // filter order
                                     float        _fc, // filter cut-off
                                     float        _fs, // empirical stop-band frequency at 60 dB
                                     float        _ap) // pass-band ripple
{
    float        tol  = 1e-3f;  // error tolerance [dB], yes, that's dB
    unsigned int nfft = 800;    // number of points to evaluate

    // design filter from prototype
    iirfilt_rrrf filter = iirfilt_rrrf_create_prototype(
        LIQUID_IIRDES_CHEBY1, LIQUID_IIRDES_LOWPASS, LIQUID_IIRDES_SOS,
        _n,_fc,0.0f,_ap,60.0f);

    // compute regions for testing
    float H0 = 0.0f, H1 = -_ap, H2 = -60;

    // verify result
    autotest_psd_s regions[] = {
      {.fmin=0.0f, .fmax=_fc,   .pmin=H1-tol, .pmax=H0+tol, .test_lo=1, .test_hi=1},
      {.fmin=_fs,  .fmax=+0.5f, .pmin=0.0f,      .pmax=H2+tol, .test_lo=0, .test_hi=1},
    };
    liquid_autotest_validate_psd_iirfilt_rrrf(__q__, filter, nfft, regions, 2,
        "autotest/logs/iirdes_cheby1_lowpass.m");

    // destroy filter object
    iirfilt_rrrf_destroy(filter);
}

// test different filter designs

LIQUID_AUTOTEST(iirdes_cheby1_lowpass_0,"description","",0.1){ testbench_iirdes_cheby1_lowpass(__q__,  5,0.20f,0.36f,1.0f); }
LIQUID_AUTOTEST(iirdes_cheby1_lowpass_1,"description","",0.1){ testbench_iirdes_cheby1_lowpass(__q__,  5,0.05f,0.14f,1.0f); }
LIQUID_AUTOTEST(iirdes_cheby1_lowpass_2,"description","",0.1){ testbench_iirdes_cheby1_lowpass(__q__,  5,0.20f,0.36f,1.0f); }
LIQUID_AUTOTEST(iirdes_cheby1_lowpass_3,"description","",0.1){ testbench_iirdes_cheby1_lowpass(__q__,  5,0.20f,0.40f,0.1f); }
LIQUID_AUTOTEST(iirdes_cheby1_lowpass_4,"description","",0.1){ testbench_iirdes_cheby1_lowpass(__q__, 15,0.35f,0.38f,0.1f); }

// check low-pass Chebyshev filter design (type 2)
void testbench_iirdes_cheby2_lowpass(liquid_autotest __q__,
                                     unsigned int _n,  // filter order
                                     float        _fp, // empirical pass-band frequency at 3 dB
                                     float        _fc, // filter cut-off
                                     float        _as) // stop-band suppression
{
    float        tol  = 1e-3f;  // error tolerance [dB], yes, that's dB
    unsigned int nfft = 800;    // number of points to evaluate

    // design filter from prototype
    iirfilt_rrrf filter = iirfilt_rrrf_create_prototype(
        LIQUID_IIRDES_CHEBY2, LIQUID_IIRDES_LOWPASS, LIQUID_IIRDES_SOS,
        _n,_fc,0.0f,0.1,_as);

    // compute regions for testing
    float H0 = 0.0f, H1 = -3, H2 = -_as;

    // verify result
    autotest_psd_s regions[] = {
      {.fmin=0.0f, .fmax=_fp,   .pmin=H1-tol, .pmax=H0+tol, .test_lo=1, .test_hi=1},
      {.fmin=_fc,  .fmax=+0.5f, .pmin=0.0f,      .pmax=H2+tol, .test_lo=0, .test_hi=1},
    };
    liquid_autotest_validate_psd_iirfilt_rrrf(__q__, filter, nfft, regions, 2,
        "autotest/logs/iirdes_cheby2_lowpass.m");

    // destroy filter object
    iirfilt_rrrf_destroy(filter);
}

// test different filter designs

LIQUID_AUTOTEST(iirdes_cheby2_lowpass_0,"description","",0.1){ testbench_iirdes_cheby2_lowpass(__q__, 5,0.08f,0.20f, 60.0f); }
LIQUID_AUTOTEST(iirdes_cheby2_lowpass_1,"description","",0.1){ testbench_iirdes_cheby2_lowpass(__q__, 5,0.02f,0.05f, 60.0f); }
LIQUID_AUTOTEST(iirdes_cheby2_lowpass_2,"description","",0.1){ testbench_iirdes_cheby2_lowpass(__q__, 5,0.07f,0.20f, /*100.0f*/ 70.0f); }
LIQUID_AUTOTEST(iirdes_cheby2_lowpass_3,"description","",0.1){ testbench_iirdes_cheby2_lowpass(__q__, 5,0.09f,0.20f, 60.0f); }
LIQUID_AUTOTEST(iirdes_cheby2_lowpass_4,"description","",0.1){ testbench_iirdes_cheby2_lowpass(__q__,15,0.30f,0.35f,/*120.0f*/ 70.0f); }

// check low-pass Butterworth filter design
void testbench_iirdes_butter_lowpass(liquid_autotest __q__,
                                     unsigned int _n,  // filter order
                                     float        _fc, // filter cut-off
                                     float        _fs) // empirical stop-band frequency at 60 dB
{
    float        tol  = 1e-3f;  // error tolerance [dB], yes, that's dB
    unsigned int nfft = 800;    // number of points to evaluate

    // design filter from prototype
    iirfilt_rrrf filter = iirfilt_rrrf_create_prototype(
        LIQUID_IIRDES_BUTTER, LIQUID_IIRDES_LOWPASS, LIQUID_IIRDES_SOS,
        _n,_fc,0.0f,1,60);

    // compute regions for testing
    float H0 = 0.0f, H1 = -3.0f, H2 = -60.0f;

    // verify result
    autotest_psd_s regions[] = {
      {.fmin=0.0f, .fmax=0.98f*_fc, .pmin=H1-tol, .pmax=H0+tol, .test_lo=1, .test_hi=1},
      {.fmin=_fs,  .fmax=+0.5f,    .pmin=0.0f,      .pmax=H2+tol, .test_lo=0, .test_hi=1},
    };
    liquid_autotest_validate_psd_iirfilt_rrrf(__q__, filter, nfft, regions, 2,
        "autotest/logs/iirdes_butter_lowpass.m");

    // destroy filter object
    iirfilt_rrrf_destroy(filter);
}

// test different filter designs

LIQUID_AUTOTEST(iirdes_butter_lowpass_0,"description","",0.1){ testbench_iirdes_butter_lowpass(__q__, 5,0.20f,0.40f); }
LIQUID_AUTOTEST(iirdes_butter_lowpass_1,"description","",0.1){ testbench_iirdes_butter_lowpass(__q__, 5,0.05f,0.19f); }
LIQUID_AUTOTEST(iirdes_butter_lowpass_2,"description","",0.1){ testbench_iirdes_butter_lowpass(__q__, 5,0.20f,0.40f); }
LIQUID_AUTOTEST(iirdes_butter_lowpass_3,"description","",0.1){ testbench_iirdes_butter_lowpass(__q__, 5,0.20f,0.40f); }
LIQUID_AUTOTEST(iirdes_butter_lowpass_4,"description","",0.1){ testbench_iirdes_butter_lowpass(__q__,15,0.35f,0.41f); }

LIQUID_AUTOTEST(iirdes_ellip_highpass,"check elliptical filter design with high-pass transformation", "", 0.1) {
    unsigned int n  =    9;   // filter order
    float        fc =  0.2;   // filter cut-off
    float        Ap =  0.1;   // pass-band ripple
    float        as = 60.0;   // stop-band suppression

    float        tol  = 1e-3f;  // error tolerance [dB], yes, that's dB
    unsigned int nfft = 800;    // number of points to evaluate

    // design filter from prototype
    iirfilt_rrrf filter = iirfilt_rrrf_create_prototype(LIQUID_IIRDES_ELLIP,
        LIQUID_IIRDES_HIGHPASS, LIQUID_IIRDES_SOS,n,fc,0.0f,Ap,as);

    // verify result
    autotest_psd_s regions[] = {
      {.fmin=-0.5,   .fmax=-fc,   .pmin=-Ap-tol, .pmax=   +tol, .test_lo=1, .test_hi=1},
      {.fmin=-0.184, .fmax=0.184, .pmin=0.0f,       .pmax=-as+tol, .test_lo=0, .test_hi=1},
      {.fmin=fc,     .fmax=0.5,   .pmin=-Ap-tol, .pmax=   +tol, .test_lo=1, .test_hi=1},
    };
    liquid_autotest_validate_psd_iirfilt_rrrf(__q__, filter, nfft, regions, 3,
        "autotest/logs/iirdes_ellip_highpass.m");

    // destroy filter object
    iirfilt_rrrf_destroy(filter);
}

LIQUID_AUTOTEST(iirdes_ellip_bandpass,"check elliptical filter design with band-pass transformation", "", 0.1) {
    unsigned int n  =    9;   // filter order
    float        fc =  0.3;   // filter cut-off
    float        f0 =  0.35;  // filter center frequency
    float        Ap =  0.1;   // pass-band ripple
    float        as = 60.0;   // stop-band suppression

    float        tol  = 1e-3f;  // error tolerance [dB], yes, that's dB
    unsigned int nfft = 2400;   // number of points to evaluate

    // design filter from prototype
    iirfilt_rrrf filter = iirfilt_rrrf_create_prototype(LIQUID_IIRDES_ELLIP,
        LIQUID_IIRDES_BANDPASS, LIQUID_IIRDES_SOS,n,fc,f0,Ap,as);

    // verify result
    autotest_psd_s regions[] = {
      {.fmin=-0.5,   .fmax=-0.396,.pmin=0.0f,       .pmax=-as+tol, .test_lo=0, .test_hi=1},
      {.fmin=-0.388, .fmax=-0.301,.pmin=-Ap-tol, .pmax=    tol, .test_lo=1, .test_hi=1},
      {.fmin=-0.293, .fmax=+0.293,.pmin=0.0f,       .pmax=-as+tol, .test_lo=0, .test_hi=1},
      {.fmin=+0.301, .fmax=+0.388,.pmin=-Ap-tol, .pmax=    tol, .test_lo=1, .test_hi=1},
      {.fmin=+0.396, .fmax=+0.5,  .pmin=0.0f,       .pmax=-as+tol, .test_lo=0, .test_hi=1},
    };
    liquid_autotest_validate_psd_iirfilt_rrrf(__q__, filter, nfft, regions, 5,
        "autotest/logs/iirdes_ellip_bandpass.m");

    // destroy filter object
    iirfilt_rrrf_destroy(filter);
}

LIQUID_AUTOTEST(iirdes_ellip_bandstop,"check elliptical filter design with band-stop transformation", "", 0.1) {
    unsigned int n  =    9;   // filter order
    float        fc =  0.3;   // filter cut-off
    float        f0 =  0.35;  // filter center frequency
    float        Ap =  0.1;   // pass-band ripple
    float        as = 60.0;   // stop-band suppression

    float        tol  = 1e-3f;  // error tolerance [dB], yes, that's dB
    unsigned int nfft = 2400;   // number of points to evaluate

    // design filter from prototype
    iirfilt_rrrf filter = iirfilt_rrrf_create_prototype(LIQUID_IIRDES_ELLIP,
        LIQUID_IIRDES_BANDSTOP, LIQUID_IIRDES_SOS,n,fc,f0,Ap,as);

    // verify result
    autotest_psd_s regions[] = {
      {.fmin=-0.5,   .fmax=-0.391,.pmin=-Ap-tol, .pmax=    tol, .test_lo=1, .test_hi=1},
      {.fmin=-0.387, .fmax=-0.306,.pmin=0.0f,       .pmax=-as+tol, .test_lo=0, .test_hi=1},
      {.fmin=-0.298, .fmax=+0.298,.pmin=-Ap-tol, .pmax=    tol, .test_lo=1, .test_hi=1},
      {.fmin=+0.306, .fmax=+0.387,.pmin=0.0f,       .pmax=-as+tol, .test_lo=0, .test_hi=1},
      {.fmin=+0.391, .fmax=+0.5,  .pmin=-Ap-tol, .pmax=    tol, .test_lo=1, .test_hi=1},
    };
    liquid_autotest_validate_psd_iirfilt_rrrf(__q__, filter, nfft, regions, 5,
        "autotest/logs/iirdes_ellip_bandstop.m");

    // destroy filter object
    iirfilt_rrrf_destroy(filter);
}

LIQUID_AUTOTEST(iirdes_bessel,"check Bessel filter design", "", 0.1) {
    unsigned int n  =    9;   // filter order
    float        fc =  0.1;   // filter cut-off
    unsigned int nfft = 960;  // number of points to evaluate

    // design filter from prototype
    iirfilt_rrrf filter = iirfilt_rrrf_create_prototype(LIQUID_IIRDES_BESSEL,
        LIQUID_IIRDES_LOWPASS, LIQUID_IIRDES_SOS,n,fc,0,1,60);

    // verify result
    autotest_psd_s regions[] = {
      {.fmin=-0.500, .fmax=-0.305,.pmin=0.0f, .pmax=-60.0f,   .test_lo=0, .test_hi=1},
      {.fmin=-0.095, .fmax=+0.095,.pmin=-3.0f, .pmax=  0.1, .test_lo=1, .test_hi=1},
      {.fmin=+0.305, .fmax=+0.500,.pmin=0.0f, .pmax=-60.0f,   .test_lo=0, .test_hi=1},
    };
    liquid_autotest_validate_psd_iirfilt_rrrf(__q__, filter, nfft, regions, 3,
        "autotest/logs/iirdes_bessel.m");

    // destroy filter object
    iirfilt_rrrf_destroy(filter);
}

