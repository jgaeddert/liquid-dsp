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

#include "liquid.autotest.h"
#include "liquid.internal.h"
#include "liquid_vla.h"

LIQUID_AUTOTEST(liquid_firdes_rcos,"description","",0.1) {

    // Initialize variables
    unsigned int k=2, m=3;
    float beta=0.3f;
    float offset=0.0f;

    // Initialize pre-determined coefficient array
    float h0[13] = {
       1.65502646542134e-17,
       7.20253052925685e-02,
      -1.26653717080575e-16,
      -1.74718023726940e-01,
       2.95450626814946e-16,
       6.23332275392119e-01,
       1.00000000000000e+00,
       6.23332275392119e-01,
      -2.23850244261176e-16,
      -1.74718023726940e-01,
      -2.73763990895627e-17,
       7.20253052925685e-02
    };

    // Create filter
    LIQUID_VLA(float, h, 13);
    liquid_firdes_rcos(k,m,beta,offset,h);

    // Ensure data are equal
    unsigned int i;
    for (i=0; i<13; i++)
        LIQUID_CHECK_DELTA( h[i], h0[i], 0.00001f );
}

LIQUID_AUTOTEST(liquid_firdes_rrcos,"description","",0.1) {

    // Initialize variables
    unsigned int k=2, m=3;
    float beta=0.3f;
    float offset=0.0f;

    // Initialize pre-determined coefficient array
    float h0[13] = {
       -3.311577E-02, 
        4.501582E-02, 
        5.659688E-02, 
       -1.536039E-01, 
       -7.500154E-02, 
        6.153450E-01, 
        1.081972E+00, 
        6.153450E-01, 
       -7.500154E-02, 
       -1.536039E-01, 
        5.659688E-02, 
        4.501582E-02,
       -3.311577E-02}; 

    // Create filter
    LIQUID_VLA(float, h, 13);
    liquid_firdes_rrcos(k,m,beta,offset,h);

    // Ensure data are equal
    unsigned int i;
    for (i=0; i<13; i++)
        LIQUID_CHECK_DELTA( h[i], h0[i], 0.00001f );
}

void test_harness_matched_filter(liquid_autotest __q__,
                                 int          _type,
                                 unsigned int _k,
                                 unsigned int _m,
                                 float        _beta,
                                 float        _tol_isi,
                                 float        _tol_as)
{
    // Create filter
    unsigned int h_len = 2*_k*_m+1;
    LIQUID_VLA(float, h, h_len);
    liquid_firdes_prototype((liquid_firfilt_type)_type,_k,_m,_beta,0.0f,h);

    // scale by samples per symbol
    liquid_vectorf_mulscalar(h, h_len, 1.0f/(float)_k, h);

    // compute filter ISI
    float isi_max;
    float isi_rms;
    liquid_filter_isi(h,_k,_m,&isi_rms,&isi_max);

    // ensure ISI is sufficiently small (log scale)
    LIQUID_CHECK(20*log10f(isi_max) < _tol_isi);
    LIQUID_CHECK(20*log10f(isi_rms) < _tol_isi);

    // verify spectrum response
    autotest_psd_s regions[] = {
      {.fmin=-0.50, .fmax=-0.35f, .pmin=0.0f, .pmax=_tol_as, .test_lo=0, .test_hi=1},
      {.fmin=-0.20, .fmax= 0.20f, .pmin=-1.0f, .pmax=     +1, .test_lo=1, .test_hi=1},
      {.fmin= 0.35, .fmax= 0.50f, .pmin=0.0f, .pmax=_tol_as, .test_lo=0, .test_hi=1},
    };
    LIQUID_VLA(char, filename, 256);
    sprintf(filename,"autotest/logs/firdes_%s.m", liquid_firfilt_type_str[_type][0]);
    liquid_autotest_validate_psd_signalf(__q__, h, h_len, regions, 3, filename);
}

// test matched filter responses for square-root nyquist filter prototypes

LIQUID_AUTOTEST(firdes_rrcos   ,"description","",0.1)
    { test_harness_matched_filter(__q__,LIQUID_FIRFILT_RRC,     2, 10, 0.3f, -60.0f, -40.0f); }

LIQUID_AUTOTEST(firdes_rkaiser ,"description","",0.1)
    { test_harness_matched_filter(__q__,LIQUID_FIRFILT_RKAISER, 2, 10, 0.3f, -60.0f, -70.0f); }

LIQUID_AUTOTEST(firdes_arkaiser,"description","",0.1)
    { test_harness_matched_filter(__q__,LIQUID_FIRFILT_ARKAISER,2, 10, 0.3f, -60.0f, -70.0f); }

LIQUID_AUTOTEST(liquid_firdes_dcblock,"description","",0.1)
{
    // options
    unsigned int m   = 20;      // filter semi-length
    float        as  = 60.0f;   // stop-band suppression/pass-band ripple

    // Create filter
    unsigned int h_len = 2*m+1;
    LIQUID_VLA(float, h, h_len);
    liquid_firdes_notch(m,0,as,h);

    // compute filter response and evaluate at several frequencies
    unsigned int  nfft = 1200;
    LIQUID_VLA(liquid_float_complex, buf_time, nfft);
    LIQUID_VLA(liquid_float_complex, buf_freq, nfft);
    unsigned int i;
    for (i=0; i<nfft; i++)
        buf_time[i] = i < h_len ? h[i] : 0;
    fft_run(nfft, buf_time, buf_freq, LIQUID_FFT_FORWARD, 0);

    // evaluate at several points
    float tol = 2*powf(10.0f, -as/20.0f); // generous
    LIQUID_CHECK_DELTA(cabsf(buf_freq[       0]), 0.0f, tol);   // notch at DC
    LIQUID_CHECK_DELTA(cabsf(buf_freq[  nfft/4]), 1.0f, tol);   // pass at  Fs/4
    LIQUID_CHECK_DELTA(cabsf(buf_freq[2*nfft/4]), 1.0f, tol);   // pass at  Fs/2
    LIQUID_CHECK_DELTA(cabsf(buf_freq[3*nfft/4]), 1.0f, tol);   // pass at -Fs/4
}

LIQUID_AUTOTEST(liquid_firdes_notch,"description","",0.1)
{
    // options
    unsigned int m   = 20;      // filter semi-length
    float        as  = 60.0f;   // stop-band suppression/pass-band ripple
    float        f0  = 0.2f;    // notch frequency (must be greater than zero here)

    // Create filter
    unsigned int h_len = 2*m+1;
    LIQUID_VLA(float, h, h_len);
    liquid_firdes_notch(m,f0,as,h);

    // compute filter response and evaluate at several frequencies
    unsigned int  nfft = 1200;
    LIQUID_VLA(liquid_float_complex, buf_time, nfft);
    LIQUID_VLA(liquid_float_complex, buf_freq, nfft);
    unsigned int i;
    for (i=0; i<nfft; i++)
        buf_time[i] = i < h_len ? h[i] : 0;
    fft_run(nfft, buf_time, buf_freq, LIQUID_FFT_FORWARD, 0);

    // indices to evaluate
    unsigned int i0 = (unsigned int)roundf(f0*nfft); // positive
    unsigned int i1 = nfft - i0;                     // negative

    // evaluate at several points
    float tol = 2*powf(10.0f, -as/20.0f); // generous
    LIQUID_CHECK_DELTA(cabsf(buf_freq[    i0]), 0.0f, tol);   // notch at +f0
    LIQUID_CHECK_DELTA(cabsf(buf_freq[    i1]), 0.0f, tol);   // notch at -f0
    LIQUID_CHECK_DELTA(cabsf(buf_freq[     0]), 1.0f, tol);   // pass at  0
    LIQUID_CHECK_DELTA(cabsf(buf_freq[nfft/2]), 1.0f, tol);   // pass at  Fs/2
}

LIQUID_AUTOTEST(liquid_getopt_str2firfilt,"description","",0.1)
{
    LIQUID_CHECK( liquid_getopt_str2firfilt("unknown"   ) ==  LIQUID_FIRFILT_UNKNOWN   );
    LIQUID_CHECK( liquid_getopt_str2firfilt("kaiser"    ) ==  LIQUID_FIRFILT_KAISER    );
    LIQUID_CHECK( liquid_getopt_str2firfilt("pm"        ) ==  LIQUID_FIRFILT_PM        );
    LIQUID_CHECK( liquid_getopt_str2firfilt("rcos"      ) ==  LIQUID_FIRFILT_RCOS      );
    LIQUID_CHECK( liquid_getopt_str2firfilt("fexp"      ) ==  LIQUID_FIRFILT_FEXP      );
    LIQUID_CHECK( liquid_getopt_str2firfilt("fsech"     ) ==  LIQUID_FIRFILT_FSECH     );
    LIQUID_CHECK( liquid_getopt_str2firfilt("farcsech"  ) ==  LIQUID_FIRFILT_FARCSECH  );
    LIQUID_CHECK( liquid_getopt_str2firfilt("arkaiser"  ) ==  LIQUID_FIRFILT_ARKAISER  );
    LIQUID_CHECK( liquid_getopt_str2firfilt("rkaiser"   ) ==  LIQUID_FIRFILT_RKAISER   );
    LIQUID_CHECK( liquid_getopt_str2firfilt("rrcos"     ) ==  LIQUID_FIRFILT_RRC       );
    LIQUID_CHECK( liquid_getopt_str2firfilt("hm3"       ) ==  LIQUID_FIRFILT_hM3       );
    LIQUID_CHECK( liquid_getopt_str2firfilt("gmsktx"    ) ==  LIQUID_FIRFILT_GMSKTX    );
    LIQUID_CHECK( liquid_getopt_str2firfilt("gmskrx"    ) ==  LIQUID_FIRFILT_GMSKRX    );
    LIQUID_CHECK( liquid_getopt_str2firfilt("rfexp"     ) ==  LIQUID_FIRFILT_RFEXP     );
    LIQUID_CHECK( liquid_getopt_str2firfilt("rfsech"    ) ==  LIQUID_FIRFILT_RFSECH    );
    LIQUID_CHECK( liquid_getopt_str2firfilt("rfarcsech" ) ==  LIQUID_FIRFILT_RFARCSECH );
}

LIQUID_AUTOTEST(liquid_firdes_config,"description","",0.1)
{
    _liquid_error_downgrade_enable();
    // check that estimate methods return zero for invalid configs
    LIQUID_CHECK( estimate_req_filter_len(-0.1f, 60.0f) ==  0 ); // invalid transition band
    LIQUID_CHECK( estimate_req_filter_len( 0.0f, 60.0f) ==  0 ); // invalid transition band
    LIQUID_CHECK( estimate_req_filter_len( 0.6f, 60.0f) ==  0 ); // invalid transition band
    LIQUID_CHECK( estimate_req_filter_len( 0.2f, -1.0f) ==  0 ); // invalid stop-band suppression
    LIQUID_CHECK( estimate_req_filter_len( 0.2f,  0.0f) ==  0 ); // invalid stop-band suppression

    LIQUID_CHECK( estimate_req_filter_len_Kaiser(-0.1f, 60.0f) ==  0 ); // invalid transition band
    LIQUID_CHECK( estimate_req_filter_len_Kaiser( 0.0f, 60.0f) ==  0 ); // invalid transition band
    LIQUID_CHECK( estimate_req_filter_len_Kaiser( 0.6f, 60.0f) ==  0 ); // invalid transition band
    LIQUID_CHECK( estimate_req_filter_len_Kaiser( 0.2f, -1.0f) ==  0 ); // invalid stop-band suppression
    LIQUID_CHECK( estimate_req_filter_len_Kaiser( 0.2f,  0.0f) ==  0 ); // invalid stop-band suppression

    LIQUID_CHECK( estimate_req_filter_len_Herrmann(-0.1f, 60.0f) ==  0 ); // invalid transition band
    LIQUID_CHECK( estimate_req_filter_len_Herrmann( 0.0f, 60.0f) ==  0 ); // invalid transition band
    LIQUID_CHECK( estimate_req_filter_len_Herrmann( 0.6f, 60.0f) ==  0 ); // invalid transition band
    LIQUID_CHECK( estimate_req_filter_len_Herrmann( 0.2f, -1.0f) ==  0 ); // invalid stop-band suppression
    LIQUID_CHECK( estimate_req_filter_len_Herrmann( 0.2f,  0.0f) ==  0 ); // invalid stop-band suppression

    unsigned int m     =  4;
    unsigned int h_len = 2*m+1;
    LIQUID_VLA(float, h, h_len);
    int          wtype = LIQUID_WINDOW_HAMMING;
    LIQUID_CHECK(liquid_firdes_windowf(wtype, h_len, 0.2f, 0, h) ==  LIQUID_OK      );
    LIQUID_CHECK(liquid_firdes_windowf(wtype,     0, 0.2f, 0, h) ==  LIQUID_EICONFIG);
    LIQUID_CHECK(liquid_firdes_windowf(wtype, h_len,-0.1f, 0, h) ==  LIQUID_EICONFIG);
    LIQUID_CHECK(liquid_firdes_windowf(wtype, h_len, 0.0f, 0, h) ==  LIQUID_EICONFIG);
    LIQUID_CHECK(liquid_firdes_windowf(wtype, h_len, 0.6f, 0, h) ==  LIQUID_EICONFIG);

    LIQUID_CHECK(liquid_firdes_kaiser(h_len, 0.2f, 60.0f, 0.0f, h) ==  LIQUID_OK      );
    LIQUID_CHECK(liquid_firdes_kaiser(    0, 0.2f, 60.0f, 0.0f, h) ==  LIQUID_EICONFIG);
    LIQUID_CHECK(liquid_firdes_kaiser(h_len,-0.1f, 60.0f, 0.0f, h) ==  LIQUID_EICONFIG);
    LIQUID_CHECK(liquid_firdes_kaiser(h_len, 0.0f, 60.0f, 0.0f, h) ==  LIQUID_EICONFIG);
    LIQUID_CHECK(liquid_firdes_kaiser(h_len, 0.6f, 60.0f, 0.0f, h) ==  LIQUID_EICONFIG);
    LIQUID_CHECK(liquid_firdes_kaiser(h_len, 0.2f, 60.0f,-0.7f, h) ==  LIQUID_EICONFIG);
    LIQUID_CHECK(liquid_firdes_kaiser(h_len, 0.2f, 60.0f, 0.7f, h) ==  LIQUID_EICONFIG);

    LIQUID_CHECK(liquid_firdes_notch(m, 0.2f, 60.0f, h) ==  LIQUID_OK);
    LIQUID_CHECK(liquid_firdes_notch(0, 0.2f, 60.0f, h) ==  LIQUID_EICONFIG);
    LIQUID_CHECK(liquid_firdes_notch(m,-0.7f, 60.0f, h) ==  LIQUID_EICONFIG);
    LIQUID_CHECK(liquid_firdes_notch(m, 0.7f, 60.0f, h) ==  LIQUID_EICONFIG);
    LIQUID_CHECK(liquid_firdes_notch(m, 0.2f, -8.0f, h) ==  LIQUID_EICONFIG);

    LIQUID_CHECK(liquid_firdes_prototype(LIQUID_FIRFILT_UNKNOWN,2,2,0.3f,0.0f,h) == LIQUID_EICONFIG);

    // test energy calculation configuration; design proper filter
    liquid_firdes_windowf(wtype, h_len, 0.2f, 0, h);
    LIQUID_CHECK(liquid_filter_energy(h,h_len,-0.1f,1200) ==  0.0f);
    LIQUID_CHECK(liquid_filter_energy(h,h_len, 0.7f,1200) ==  0.0f);
    LIQUID_CHECK(liquid_filter_energy(h,h_len, 0.3f,   0) ==  0.0f);

    LIQUID_CHECK( liquid_getopt_str2firfilt("unknown-filter-type" ) ==  LIQUID_FIRFILT_UNKNOWN);
    _liquid_error_downgrade_disable();
}

LIQUID_AUTOTEST(liquid_firdes_estimate,"description","",0.1)
{
    float tol = 0.05f; // dB

    // Kaiser's method
    LIQUID_CHECK_DELTA( estimate_req_filter_len_Kaiser( 0.05f, 60.0f), 73.00140381, tol);
    LIQUID_CHECK_DELTA( estimate_req_filter_len_Kaiser( 0.10f, 60.0f), 36.50070190, tol);
    LIQUID_CHECK_DELTA( estimate_req_filter_len_Kaiser( 0.20f, 60.0f), 18.25035095, tol);
    LIQUID_CHECK_DELTA( estimate_req_filter_len_Kaiser( 0.30f, 60.0f), 12.16689968, tol);
    LIQUID_CHECK_DELTA( estimate_req_filter_len_Kaiser( 0.40f, 60.0f),  9.12517548, tol);
    LIQUID_CHECK_DELTA( estimate_req_filter_len_Kaiser( 0.05f, 80.0f),101.05189514, tol);
    LIQUID_CHECK_DELTA( estimate_req_filter_len_Kaiser( 0.05f,100.0f),129.10238647, tol);
    LIQUID_CHECK_DELTA( estimate_req_filter_len_Kaiser( 0.05f,120.0f),157.15287781, tol);

    // Herrmann's method
    LIQUID_CHECK_DELTA( estimate_req_filter_len_Herrmann( 0.05f, 60.0f), 75.51549530, tol);
    LIQUID_CHECK_DELTA( estimate_req_filter_len_Herrmann( 0.10f, 60.0f), 37.43184662, tol);
    LIQUID_CHECK_DELTA( estimate_req_filter_len_Herrmann( 0.20f, 60.0f), 17.56412315, tol);
    LIQUID_CHECK_DELTA( estimate_req_filter_len_Herrmann( 0.30f, 60.0f), 10.20741558, tol);
    LIQUID_CHECK_DELTA( estimate_req_filter_len_Herrmann( 0.40f, 60.0f),  5.97846174, tol);
    LIQUID_CHECK_DELTA( estimate_req_filter_len_Herrmann( 0.05f, 80.0f),102.72290039, tol);
    LIQUID_CHECK_DELTA( estimate_req_filter_len_Herrmann( 0.05f,100.0f),129.88548279, tol);
    LIQUID_CHECK_DELTA( estimate_req_filter_len_Herrmann( 0.05f,120.0f),157.15287781, tol);
}

void testbench_firdes_prototype(liquid_autotest __q__,
                                const char * _type,
                                unsigned int _k,
                                unsigned int _m,
                                float        _beta,
                                float        _as)
{
    // design filter
    unsigned int h_len = 2*_k*_m+1;
    LIQUID_VLA(float, h, h_len);
    liquid_firfilt_type type = (liquid_firfilt_type)liquid_getopt_str2firfilt(_type);
    if (type == LIQUID_FIRFILT_UNKNOWN) {
        LIQUID_FAIL("invalid configuration");
        return;
    }
    liquid_firdes_prototype(type, _k, _m, _beta, 0.0f, h);

    // scale by samples per symbol
    liquid_vectorf_mulscalar(h, h_len, 1.0f/(float)_k, h);

    // verify spectrum
    float bw = 1.0f / (float)_k;
    float f0 = 0.45*bw*(1-_beta);
    float f1 = 0.55*bw*(1+_beta);
    autotest_psd_s regions[] = {
      {.fmin=-0.5,.fmax=-f1, .pmin=0.0f, .pmax=-_as, .test_lo=0, .test_hi=1},
      {.fmin=-f0, .fmax= f0, .pmin=-1.0f, .pmax=1.0f,   .test_lo=1, .test_hi=1},
      {.fmin= f1, .fmax=+0.5,.pmin=0.0f, .pmax=-_as, .test_lo=0, .test_hi=1},
    };
    LIQUID_VLA(char, filename, 256);
    sprintf(filename,"autotest/logs/firdes_prototype_%s.m", _type);
    liquid_autotest_validate_psd_signalf(__q__, h, h_len, regions, 3, filename);
}

LIQUID_AUTOTEST(firdes_prototype_kaiser   ,"description","",0.1){ testbench_firdes_prototype(__q__,"kaiser",   4, 12, 0.3f, 60.0f); }
LIQUID_AUTOTEST(firdes_prototype_pm       ,"description","",0.1){ testbench_firdes_prototype(__q__,"pm",       4, 12, 0.3f, 80.0f); }
LIQUID_AUTOTEST(firdes_prototype_rcos     ,"description","",0.1){ testbench_firdes_prototype(__q__,"rcos",     4, 12, 0.3f, 60.0f); }
LIQUID_AUTOTEST(firdes_prototype_fexp     ,"description","",0.1){ testbench_firdes_prototype(__q__,"fexp",     4, 12, 0.3f, 40.0f); }
LIQUID_AUTOTEST(firdes_prototype_fsech    ,"description","",0.1){ testbench_firdes_prototype(__q__,"fsech",    4, 12, 0.3f, 60.0f); }
LIQUID_AUTOTEST(firdes_prototype_farcsech ,"description","",0.1){ testbench_firdes_prototype(__q__,"farcsech", 4, 12, 0.3f, 40.0f); }
LIQUID_AUTOTEST(firdes_prototype_arkaiser ,"description","",0.1){ testbench_firdes_prototype(__q__,"arkaiser", 4, 12, 0.3f, 90.0f); }
LIQUID_AUTOTEST(firdes_prototype_rkaiser  ,"description","",0.1){ testbench_firdes_prototype(__q__,"rkaiser",  4, 12, 0.3f, 90.0f); }
LIQUID_AUTOTEST(firdes_prototype_rrcos    ,"description","",0.1){ testbench_firdes_prototype(__q__,"rrcos",    4, 12, 0.3f, 45.0f); }
LIQUID_AUTOTEST(firdes_prototype_hm3      ,"description","",0.1){ testbench_firdes_prototype(__q__,"hm3",      4, 12, 0.3f,100.0f); }
LIQUID_AUTOTEST(firdes_prototype_rfexp    ,"description","",0.1){ testbench_firdes_prototype(__q__,"rfexp",    4, 12, 0.3f, 30.0f); }
LIQUID_AUTOTEST(firdes_prototype_rfsech   ,"description","",0.1){ testbench_firdes_prototype(__q__,"rfsech",   4, 12, 0.3f, 40.0f); }
LIQUID_AUTOTEST(firdes_prototype_rfarcsech,"description","",0.1){ testbench_firdes_prototype(__q__,"rfarcsech",4, 12, 0.3f, 30.0f); }

// ignore gmsk filters as these weren't designed for flat pass-band responses
//void xautotest_firdes_prototype_gmsktx   (){ testbench_firdes_prototype(__q__, "gmsktx",   4, 12, 0.3f, 60.0f); }
//void xautotest_firdes_prototype_gmskrx   (){ testbench_firdes_prototype(__q__, "gmskrx",   4, 12, 0.3f, 60.0f); }

LIQUID_AUTOTEST(firdes_doppler,"description","",0.1)
{
    // design filter
    float        fd     = 0.2f;  // Normalized Doppler frequency
    float        K      = 10.0f; // Rice fading factor
    float        theta  = 0.0f;  // LoS component angle of arrival
    unsigned int h_len  = 161;   // filter length
    LIQUID_VLA(float, h, h_len);
    liquid_firdes_doppler(h_len,fd,K,theta,h);

    // verify resulting spectrum
    autotest_psd_s regions[] = {
      {.fmin=-0.5,   .fmax=-0.25,  .pmin=0.0f, .pmax=0.0f, .test_lo=0, .test_hi=1},
      {.fmin=-0.205, .fmax=-0.195, .pmin=30, .pmax=40, .test_lo=1, .test_hi=1},
      {.fmin=-0.14,  .fmax=+0.14,  .pmin= 6, .pmax=12, .test_lo=1, .test_hi=1},
      {.fmin=+0.195, .fmax=+0.205, .pmin=30, .pmax=40, .test_lo=1, .test_hi=1},
      {.fmin= 0.25,  .fmax=+0.5,   .pmin=0.0f, .pmax=0.0f, .test_lo=0, .test_hi=1},
    };
    liquid_autotest_validate_psd_signalf(__q__, h, h_len, regions, 5,
        "autotest/logs/firdes_doppler.m");
}

LIQUID_AUTOTEST(liquid_freqrespf,"check frequency response (real-valued coefficients)", "", 0.1)
{
    // design filter
    unsigned int h_len = 41;
    LIQUID_VLA(float, h, h_len);
    liquid_firdes_kaiser(h_len, 0.27f, 80.0f, 0.3f, h);

    // compute frequency response with FFT
    unsigned int i, nfft = 400;
    LIQUID_VLA(liquid_float_complex, buf_time, nfft);
    LIQUID_VLA(liquid_float_complex, buf_freq, nfft);
    for (i=0; i<nfft; i++)
        buf_time[i] = i < h_len ? h[i] : 0.0f;
    fft_run(nfft, buf_time, buf_freq, LIQUID_FFT_FORWARD, 0);

    // compare to manual calculation
    float tol = 1e-5f;
    for (i=0; i<nfft; i++) {
        float fc = (float)i/(float)nfft + (i >= nfft/2 ? -1.0f : 0.0f);
        liquid_float_complex H;
        liquid_freqrespf(h, h_len, fc, &H);

        LIQUID_CHECK_DELTA(crealf(buf_freq[i]), crealf(H), tol);
        LIQUID_CHECK_DELTA(cimagf(buf_freq[i]), cimagf(H), tol);
    }
}

LIQUID_AUTOTEST(liquid_freqrespcf,"check frequency response (complex-valued coefficients)", "", 0.1)
{
    // design filter and apply complex phasor
    unsigned int i, h_len = 41;
    LIQUID_VLA(float, hf, h_len);
    liquid_firdes_kaiser(h_len, 0.27f, 80.0f, 0.3f, hf);
    LIQUID_VLA(liquid_float_complex, h, h_len);
    for (i=0; i<h_len; i++)
        h[i] = hf[i] * cexpf(_Complex_I*0.1f*(float)(i*i));

    // compute frequency response with FFT
    unsigned int  nfft = 400;
    LIQUID_VLA(liquid_float_complex, buf_time, nfft);
    LIQUID_VLA(liquid_float_complex, buf_freq, nfft);
    for (i=0; i<nfft; i++)
        buf_time[i] = i < h_len ? h[i] : 0.0f;
    fft_run(nfft, buf_time, buf_freq, LIQUID_FFT_FORWARD, 0);

    // compare to manual calculation
    float tol = 1e-5f;
    for (i=0; i<nfft; i++) {
        float fc = (float)i/(float)nfft + (i >= nfft/2 ? -1.0f : 0.0f);
        liquid_float_complex H;
        liquid_freqrespcf(h, h_len, fc, &H);

        LIQUID_CHECK_DELTA(crealf(buf_freq[i]), crealf(H), tol);
        LIQUID_CHECK_DELTA(cimagf(buf_freq[i]), cimagf(H), tol);
    }
}

