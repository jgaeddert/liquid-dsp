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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "liquid.autotest.h"
#include "liquid.internal.h"

LIQUID_AUTOTEST(framesync64,"simple recovery of frame64 in noise","",0.1)
{
    unsigned int i;

    // create objects
    unsigned int context = 0;
    framegen64 fg = framegen64_create();
    framesync64 fs = framesync64_create(framing_autotest_callback, (void*)&context);

    // generate the frame
    LIQUID_VLA(liquid_float_complex, frame, LIQUID_FRAME64_LEN);
    framegen64_execute(fg, NULL, NULL, frame);

    // add some noise
    for (i=0; i<LIQUID_FRAME64_LEN; i++)
        frame[i] += 0.01f*(randnf() + _Complex_I*randnf()) * M_SQRT1_2;

    // try to receive the frame
    framesync64_execute(fs, frame, LIQUID_FRAME64_LEN);

    // ensure callback was actually invoked
    LIQUID_CHECK(context ==  FRAMING_AUTOTEST_SECRET);

    // parse statistics
    framedatastats_s stats = framesync64_get_framedatastats(fs);
    LIQUID_CHECK(stats.num_frames_detected ==  1);
    LIQUID_CHECK(stats.num_headers_valid ==    1);
    LIQUID_CHECK(stats.num_payloads_valid ==   1);
    LIQUID_CHECK(stats.num_bytes_received ==  64);

    // destroy objects
    framegen64_destroy(fg);
    framesync64_destroy(fs);
}

LIQUID_AUTOTEST(framegen64_copy,"copying from one framegen64 object to another","",0.1)
{
    framegen64 q_orig = framegen64_create();
    framegen64 q_copy = framegen64_copy(q_orig);

    LIQUID_REQUIRE( q_copy != NULL );

    framegen64_destroy(q_orig);
    framegen64_destroy(q_copy);
}

LIQUID_AUTOTEST(framesync64_copy,"copying from one framesync64 object to another","",0.1)
{
    unsigned int i;
    int context_0 = 0;
    int context_1 = 0;

    // create objects
    framegen64  fg  = framegen64_create();
    framesync64 fs0 = framesync64_create(framing_autotest_callback, (void*)&context_0);

    // feed random samples into synchronizer
    LIQUID_VLA(liquid_float_complex, buf, LIQUID_FRAME64_LEN);
    for (i=0; i<LIQUID_FRAME64_LEN; i++)
        buf[i] = 0.01f*(randnf() + _Complex_I*randnf()) * M_SQRT1_2;
    framesync64_execute(fs0, buf, LIQUID_FRAME64_LEN);

    // generate the frame
    framegen64_execute(fg, NULL, NULL, buf);

    // add some noise
    for (i=0; i<LIQUID_FRAME64_LEN; i++)
        buf[i] += 0.01f*(randnf() + _Complex_I*randnf()) * M_SQRT1_2;

    // copy object, but set different context
    framesync64 fs1 = framesync64_copy(fs0);
    framesync64_set_userdata(fs1, (void*)&context_1);
    framesync64_print(fs0);
    framesync64_print(fs1);

    // try to receive the frame with each receiver
    for (i=0; i<LIQUID_FRAME64_LEN; i++) {
        // step one sample through at a time
        framesync64_execute(fs0, buf+i, 1);
        framesync64_execute(fs1, buf+i, 1);

        // ensure that the frames are recovered at exactly the same time
        LIQUID_CHECK( context_0 ==  context_1 );
    }

    // check that frame was actually recovered by each object
    LIQUID_CHECK( context_0 ==  0x01234567 );
    LIQUID_CHECK( context_1 ==  0x01234567 );

    // parse statistics
    framedatastats_s stats_0 = framesync64_get_framedatastats(fs0);
    framedatastats_s stats_1 = framesync64_get_framedatastats(fs1);
    liquid_log_debug(" detected:%u(%u), headers valid:%u(%u), payloads valid:%u(%u), bytes rx:%u(%u)",
        stats_0.num_frames_detected, stats_1.num_frames_detected,
        stats_0.num_headers_valid,   stats_1.num_headers_valid,
        stats_0.num_payloads_valid,  stats_1.num_payloads_valid,
        stats_0.num_bytes_received,  stats_1.num_bytes_received);

    LIQUID_CHECK(stats_0.num_frames_detected ==  stats_1.num_frames_detected);
    LIQUID_CHECK(stats_0.num_headers_valid   ==  stats_1.num_headers_valid  );
    LIQUID_CHECK(stats_0.num_payloads_valid  ==  stats_1.num_payloads_valid );
    LIQUID_CHECK(stats_0.num_bytes_received  ==  stats_1.num_bytes_received );

    // destroy objects
    framegen64_destroy(fg);
    framesync64_destroy(fs0);
    framesync64_destroy(fs1);
}

LIQUID_AUTOTEST(framesync64_config,"","",0.1)
{
    _liquid_error_downgrade_enable();
    // check invalid function calls
    LIQUID_CHECK(NULL ==framesync64_copy(NULL));
    LIQUID_CHECK(NULL ==framegen64_copy (NULL));

    // create proper object and test configurations
    framesync64 q = framesync64_create(NULL, NULL);

    LIQUID_CHECK(LIQUID_OK == framesync64_print(q))
    LIQUID_CHECK(LIQUID_OK == framesync64_set_callback(q,NULL))
    LIQUID_CHECK(LIQUID_OK == framesync64_set_userdata(q,NULL))

    LIQUID_CHECK(LIQUID_OK == framesync64_set_threshold(q,0.654321f))
    LIQUID_CHECK(0.654321f ==  framesync64_get_threshold(q))

    framesync64_destroy(q);
    _liquid_error_downgrade_disable();
}

static int callback_framesync64_autotest_debug(
    unsigned char *  _header,
    int              _header_valid,
    unsigned char *  _payload,
    unsigned int     _payload_len,
    int              _payload_valid,
    framesyncstats_s _stats,
    void *           _userdata)
{
    // return custom code based on user context
    return *((int*)_userdata);
}

// test debug interface to write file
void testbench_framesync64_debug(liquid_autotest __q__, int _code)
{
    // create objects
    framegen64  fg = framegen64_create();
    framesync64 fs = framesync64_create(callback_framesync64_autotest_debug,(void*)(&_code));

    // set prefix for filename
    const char prefix[] = "autotest/logs/framesync64";
    framesync64_set_prefix(fs,prefix);
    LIQUID_CHECK_ARRAY(framesync64_get_prefix(fs), prefix, strlen(prefix));

    // generate the frame
    LIQUID_VLA(liquid_float_complex, frame, LIQUID_FRAME64_LEN);
    unsigned char header [ 8] = {80,81,82,83,84,85,86,87};
    LIQUID_VLA(unsigned char, payload, 64);
    unsigned int i;
    for (i=0; i<64; i++)
        payload[i] = rand() & 0xff;
    framegen64_execute(fg, header, payload, frame);

    // add some noise
    for (i=0; i<LIQUID_FRAME64_LEN; i++)
        frame[i] += 0.01f*(randnf() + _Complex_I*randnf()) * M_SQRT1_2;

    // try to receive the frame
    framesync64_execute(fs, frame, LIQUID_FRAME64_LEN);

    // parse statistics
    framedatastats_s stats = framesync64_get_framedatastats(fs);
    LIQUID_CHECK(stats.num_frames_detected ==  1);
    LIQUID_CHECK(stats.num_headers_valid ==    1);
    LIQUID_CHECK(stats.num_payloads_valid ==   1);
    LIQUID_CHECK(stats.num_bytes_received ==  64);

    // get filename from the last file written and copy before destroying
    // objects
    const char * fn = framesync64_get_filename(fs);
    char filename[256] = "";
    snprintf(filename,255,"%s",fn==NULL ? "" : fn);
    liquid_log_debug("filename: %s", filename);

    // destroy objects
    framegen64_destroy(fg);
    framesync64_destroy(fs);

    // for code '0' no output should be written
    if (_code == 0)
    {
        LIQUID_REQUIRE( strlen(filename) == 0 );
        return;
    }

    // check if output file should exist
    LIQUID_REQUIRE( strlen(filename) > 0 );

    // load file
    FILE * fid = fopen(filename,"rb");
    if (fid == NULL) {
        LIQUID_FAIL("could not open for reading");
        return;
    }

    fclose(fid);
}

// test exporting debugging files with different return codes
LIQUID_AUTOTEST(framesync64_debug_none,"","",0.1) { testbench_framesync64_debug(__q__,  0); }
LIQUID_AUTOTEST(framesync64_debug_user,"","",0.1) { testbench_framesync64_debug(__q__,  1); }
LIQUID_AUTOTEST(framesync64_debug_ndet,"","",0.1) { testbench_framesync64_debug(__q__, -1); }
LIQUID_AUTOTEST(framesync64_debug_head,"","",0.1) { testbench_framesync64_debug(__q__, -2); }
LIQUID_AUTOTEST(framesync64_debug_rand,"","",0.1) { testbench_framesync64_debug(__q__, -3); }


static int callback_framesync64_autotest_estimation(
    unsigned char *  _header,
    int              _header_valid,
    unsigned char *  _payload,
    unsigned int     _payload_len,
    int              _payload_valid,
    framesyncstats_s _stats,
    void *           _userdata)
{
    liquid_log_debug("framesync64 estimation callback invoked, payload valid: %s",
        _payload_valid ? "yes" : "no");
    memmove(_userdata, &_stats, sizeof(framesyncstats_s));
    return 0;
}

// add channel offsets to frame
void framesync64_channel(liquid_float_complex * _frame,
                         float           _rssi,
                         float           _SNRdB,
                         float           _dphi)
{
    // derived values
    float gain = powf(10.0f, _rssi/20.0f); // for RSSI, not PSD (given 2 samples/symbol)
    float n0   = _rssi - _SNRdB + 10*log10f(2.0f);  // noise floor accounting for 2 samples/symbol
    float nstd = powf(10.0f, n0/20.0f);

    unsigned int i;
    for (i=0; i<LIQUID_FRAME64_LEN; i++)
        _frame[i] = _frame[i]*cexpf(_Complex_I*_dphi*(float)i)*gain + nstd*(randnf() + _Complex_I*randnf())*M_SQRT1_2;
}

LIQUID_AUTOTEST(framesync64_estimation,"simple recovery of frame64 in noise","",0.1)
{
    // create objects
    framegen64 fg = framegen64_create();
    framesyncstats_s stats;
    framesync64 fs = framesync64_create(callback_framesync64_autotest_estimation,
            (void*)&stats);

    // generate the frame
    LIQUID_VLA(liquid_float_complex, frame, LIQUID_FRAME64_LEN);
    framegen64_execute(fg, NULL, NULL, frame);

    // add offsets
    float rssi  = -43.0f;
    float SNRdB =  25.0f;
    float dphi  =   1e-2f;
    framesync64_channel(frame, rssi, SNRdB, dphi);

    // try to receive the frame
    framesync64_execute(fs, frame, LIQUID_FRAME64_LEN);

    // check results (relatively high tolerance)
    liquid_log_debug(" rssi:%.3f dB, SNRdB:%.3f dB, cfo:%.6f", stats.rssi, -stats.evm, stats.cfo);
    LIQUID_CHECK_DELTA( stats.rssi, rssi,  1.0f );
    LIQUID_CHECK_DELTA( -stats.evm, SNRdB, 3.0f ); // error biased negative
    LIQUID_CHECK_DELTA( stats.cfo,  dphi,  4e-3f);

    // destroy objects
    framegen64_destroy(fg);
    framesync64_destroy(fs);

#if 0
    FILE * fp = fopen("framesync64_errors.txt", "a");
    fprintf(fp,"%12.8f %12.8f %12.4e\n",(stats.rssi-rssi),(-stats.evm-SNRdB),(stats.cfo-dphi));
    fclose(fp);
#endif

#if 0
    FILE * fid = fopen("framesync64_estimation.m","w");
    fprintf(fid,"clear all; close all; n=%u; y=zeros(1,n);\n", LIQUID_FRAME64_LEN);
    unsigned int nfft=240;
    spgramcf q = spgramcf_create_default(nfft);
    spgramcf_write(q, frame, LIQUID_FRAME64_LEN);
    LIQUID_VLA(float, psd, nfft);
    spgramcf_get_psd(q, psd);
    spgramcf_destroy(q);
    unsigned int i;
    for (i=0; i<LIQUID_FRAME64_LEN; i++)
        fprintf(fid,"y(%3u) = %12.4e + %12.4ej;\n", i+1, crealf(frame[i]), cimagf(frame[i]));
    for (i=0; i<nfft; i++)
        fprintf(fid,"Y(%3u) = %12.4e;\n", i+1, psd[i]);
    fclose(fid);
#endif
}

