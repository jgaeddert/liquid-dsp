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

#include <string.h>
#include "liquid.autotest.h"
#include "liquid.h"

// check square-root Nyquist filter design and response
void testbench_firfilt_rnyquist(liquid_autotest __q__,
                                int          _ftype,// filter type
                                unsigned int _k,    // samples/symbol
                                unsigned int _m,    // semi-length
                                float        _beta, // excess bandwidth factor
                                float        _dt)   // fractional delay
{
    // derived values
    unsigned int i;
    unsigned int h_len  = 2*_k*_m+1;  // transmit/receive filter length
    unsigned int hc_len = 4*_k*_m+1;  // composite filter length

    // arrays
    LIQUID_VLA(float, ht, h_len);    // transmit filter
    LIQUID_VLA(float, hr, h_len);    // receive filter (different for GMSK)
    LIQUID_VLA(float, hc, hc_len);   // composite filter

    // design the filter(s)
    liquid_firdes_prototype((liquid_firfilt_type)_ftype, _k, _m, _beta, _dt, ht);

    // special case for GMSK
    if (_ftype == LIQUID_FIRFILT_GMSKTX)
        liquid_firdes_prototype(LIQUID_FIRFILT_GMSKRX, _k, _m, _beta, _dt, hr);
    else
        memcpy(hr, ht, h_len*sizeof(float));

    // TODO: check group delay

    // compute composite filter response (auto-correlation)
    for (i=0; i<hc_len; i++) {
        int lag = (int)i - (int)(2*_k*_m);
        hc[i] = liquid_filter_crosscorr(ht,h_len, hr,h_len, lag);
    }

    // compute filter inter-symbol interference
    float rxx0 = hc[2*_k*_m];
    float isi_rms=0;
    for (i=1; i<2*_m; i++) {
        float e = hc[i*_k] / rxx0;  // composite at integer multiple of _k except for i==0 ideally 0
        isi_rms += e*e;             // squared magnitude, doubled to account for filter symmetry
    }
    isi_rms = 10.0f*log10f( isi_rms / (float)(2*_m-1) );

    // compute relative stop-band energy
    unsigned int nfft = 2048;
    float As = 20*log10f( liquid_filter_energy(ht, h_len, 0.5f*(1.0f + _beta)/(float)_k, nfft) );

    liquid_log_debug("ftype: %s, k: %u, m: %u, beta: %.3f, dt: %.3f",
        liquid_firfilt_type_str[_ftype][0], _k, _m, _beta, _dt);
#if 0
    for (i=0; i<h_len; i++)
        printf("  %3d: ht = %12.8f, hr = %12.8f\n", (int)i - 2*(int)_m, ht[i], hr[i]);
    printf("composite:\n");
    for (i=0; i<hc_len; i++)
        printf("  %3d: hc = %12.8f%s\n", (int)i - 4*(int)_m, hc[i], i%_k ? "" : " *");
#endif
    liquid_log_debug("  rxx[0]    : %12.8f (expected %u)", rxx0, _k);
    liquid_log_debug("  isi (rms) : %12.8f dB", isi_rms);
    liquid_log_debug("  As        : %12.8f dB", As);

    LIQUID_CHECK_DELTA    ( rxx0,   (float)_k, 0.01f );
    LIQUID_CHECK( isi_rms< -50.0f );
    LIQUID_CHECK( As<      -50.0f );
}

// test different filter designs, nominal parameters

LIQUID_AUTOTEST(firfilt_rnyquist_baseline_arkaiser ,"description","",0.1){ testbench_firfilt_rnyquist(__q__, LIQUID_FIRFILT_ARKAISER, 2,9,0.3f,0.0f); }
LIQUID_AUTOTEST(firfilt_rnyquist_baseline_rkaiser  ,"description","",0.1){ testbench_firfilt_rnyquist(__q__, LIQUID_FIRFILT_RKAISER,  2,9,0.3f,0.0f); }
LIQUID_AUTOTEST(firfilt_rnyquist_baseline_rrc      ,"description","",0.1){ testbench_firfilt_rnyquist(__q__, LIQUID_FIRFILT_RRC,      2,9,0.3f,0.0f); }
LIQUID_AUTOTEST(firfilt_rnyquist_baseline_hm3      ,"description","",0.1){ testbench_firfilt_rnyquist(__q__, LIQUID_FIRFILT_hM3,      2,9,0.3f,0.0f); }
LIQUID_AUTOTEST(firfilt_rnyquist_baseline_gmsktxrx ,"description","",0.1){ testbench_firfilt_rnyquist(__q__, LIQUID_FIRFILT_GMSKTX,   2,9,0.3f,0.0f); }
LIQUID_AUTOTEST(firfilt_rnyquist_baseline_rfexp    ,"description","",0.1){ testbench_firfilt_rnyquist(__q__, LIQUID_FIRFILT_RFEXP,    2,9,0.3f,0.0f); }
LIQUID_AUTOTEST(firfilt_rnyquist_baseline_rfsech   ,"description","",0.1){ testbench_firfilt_rnyquist(__q__, LIQUID_FIRFILT_RFSECH,   2,9,0.3f,0.0f); }
LIQUID_AUTOTEST(firfilt_rnyquist_baseline_rfarcsech,"description","",0.1){ testbench_firfilt_rnyquist(__q__, LIQUID_FIRFILT_RFARCSECH,2,9,0.3f,0.0f); }

// test different parameters

LIQUID_AUTOTEST(firfilt_rnyquist_0,"description","",0.1){ testbench_firfilt_rnyquist(__q__, LIQUID_FIRFILT_ARKAISER, 2, 4,0.33f,0.0f); }
LIQUID_AUTOTEST(firfilt_rnyquist_1,"description","",0.1){ testbench_firfilt_rnyquist(__q__, LIQUID_FIRFILT_ARKAISER, 2,12,0.20f,0.0f); }
LIQUID_AUTOTEST(firfilt_rnyquist_2,"description","",0.1){ testbench_firfilt_rnyquist(__q__, LIQUID_FIRFILT_ARKAISER, 2,40,0.20f,0.0f); }
LIQUID_AUTOTEST(firfilt_rnyquist_3,"description","",0.1){ testbench_firfilt_rnyquist(__q__, LIQUID_FIRFILT_ARKAISER, 3,12,0.20f,0.0f); }
LIQUID_AUTOTEST(firfilt_rnyquist_4,"description","",0.1){ testbench_firfilt_rnyquist(__q__, LIQUID_FIRFILT_ARKAISER, 4,12,0.20f,0.0f); }
LIQUID_AUTOTEST(firfilt_rnyquist_5,"description","",0.1){ testbench_firfilt_rnyquist(__q__, LIQUID_FIRFILT_ARKAISER, 5,12,0.20f,0.0f); }
LIQUID_AUTOTEST(firfilt_rnyquist_6,"description","",0.1){ testbench_firfilt_rnyquist(__q__, LIQUID_FIRFILT_ARKAISER,20,12,0.20f,0.0f); }
LIQUID_AUTOTEST(firfilt_rnyquist_7,"description","",0.1){ testbench_firfilt_rnyquist(__q__, LIQUID_FIRFILT_ARKAISER, 2,12,0.80f,0.0f); }
LIQUID_AUTOTEST(firfilt_rnyquist_8,"description","",0.1){ testbench_firfilt_rnyquist(__q__, LIQUID_FIRFILT_RKAISER,  2,12,0.20f,0.5f); }
LIQUID_AUTOTEST(firfilt_rnyquist_9,"description","",0.1){ testbench_firfilt_rnyquist(__q__, LIQUID_FIRFILT_RKAISER, 20,40,0.20f,0.5f); }

