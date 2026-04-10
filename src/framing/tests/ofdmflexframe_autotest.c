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
#include "liquid.autotest.h"
#include "liquid.internal.h"

// AUTOTEST : test simple recovery of frame in noise
void testbench_ofdmflexframe(liquid_autotest   __q__,
                             unsigned int      _M,
                             unsigned int      _cp_len,
                             unsigned int      _taper_len,
                             unsigned int      _payload_len,
                             modulation_scheme _ms)
{
    liquid_log_debug("ofdmflexframe, M:%u, cp_len:%u, taper:%u, payload:%u, ms:%s",
        _M,_cp_len,_taper_len,_payload_len,modulation_types[_ms].name);

    // create frame generator/synchronizer
    unsigned int context = 0;
    ofdmflexframegenprops_s fgprops;
    ofdmflexframegenprops_init_default(&fgprops);
    fgprops.check        = LIQUID_CRC_32;
    fgprops.fec0         = LIQUID_FEC_NONE;
    fgprops.fec1         = LIQUID_FEC_NONE;
    fgprops.mod_scheme   = _ms;
    ofdmflexframegen  fg = ofdmflexframegen_create( _M, _cp_len, _taper_len, NULL, &fgprops);
    ofdmflexframesync fs = ofdmflexframesync_create(_M, _cp_len, _taper_len, NULL,
                            framing_autotest_callback, (void*)&context);

    // initialize header and payload
    unsigned char header[8] = {0, 1, 2, 3, 4, 5, 6, 7};
    LIQUID_VLA(unsigned char, payload, _payload_len);
    memset(payload, 0x00, _payload_len);

    // assemble the frame
    ofdmflexframegen_assemble(fg, header, payload, _payload_len);

    // generate the frame
    unsigned int  buf_len = 1024;
    LIQUID_VLA(liquid_float_complex, buf, buf_len);
    int frame_complete = 0;
    while (!frame_complete) {
        frame_complete = ofdmflexframegen_write(fg, buf, buf_len);
        ofdmflexframesync_execute(fs, buf, buf_len);
    }

    // verify callback was invoked
    LIQUID_CHECK( context ==  FRAMING_AUTOTEST_SECRET );

    // verify frame data statistics
    framedatastats_s stats = ofdmflexframesync_get_framedatastats(fs);
    liquid_log_debug(" detected:%u, headers valid:%u, payloads valid:%u, bytes rx:%u",
        stats.num_frames_detected, stats.num_headers_valid, stats.num_payloads_valid, stats.num_bytes_received);
    LIQUID_CHECK( stats.num_frames_detected ==  1 );
    LIQUID_CHECK( stats.num_headers_valid ==    1 );
    LIQUID_CHECK( stats.num_payloads_valid ==   1 );
    LIQUID_CHECK( stats.num_bytes_received ==   _payload_len );

    // destroy objects
    ofdmflexframegen_destroy(fg);
    ofdmflexframesync_destroy(fs);
}

//                          ID                                M  CP  TP  PAYL  modulation scheme
LIQUID_AUTOTEST(ofdmflexframe_00,"","",0.1) { testbench_ofdmflexframe(__q__,  32,  8,  4,  800, LIQUID_MODEM_QPSK); }
LIQUID_AUTOTEST(ofdmflexframe_01,"","",0.1) { testbench_ofdmflexframe(__q__,  64,  8,  4,  800, LIQUID_MODEM_QPSK); }
LIQUID_AUTOTEST(ofdmflexframe_02,"","",0.1) { testbench_ofdmflexframe(__q__, 256,  8,  4,  800, LIQUID_MODEM_QPSK); }
LIQUID_AUTOTEST(ofdmflexframe_03,"","",0.1) { testbench_ofdmflexframe(__q__,1024, 16,  8,  800, LIQUID_MODEM_QPSK); }
LIQUID_AUTOTEST(ofdmflexframe_04,"","",0.1) { testbench_ofdmflexframe(__q__,2048, 32, 16,  800, LIQUID_MODEM_QPSK); }
LIQUID_AUTOTEST(ofdmflexframe_05,"","",0.1) { testbench_ofdmflexframe(__q__,4096, 64, 32,  800, LIQUID_MODEM_QPSK); }
LIQUID_AUTOTEST(ofdmflexframe_06,"","",0.1) { testbench_ofdmflexframe(__q__,8192, 80, 40,  800, LIQUID_MODEM_QPSK); }
LIQUID_AUTOTEST(ofdmflexframe_07,"","",0.1) { testbench_ofdmflexframe(__q__,1200, 40, 20,    1, LIQUID_MODEM_QPSK); }
LIQUID_AUTOTEST(ofdmflexframe_08,"","",0.1) { testbench_ofdmflexframe(__q__,1200,  0,  0,  800, LIQUID_MODEM_QPSK); }
LIQUID_AUTOTEST(ofdmflexframe_09,"","",0.1) { testbench_ofdmflexframe(__q__,1200, 40, 20, 8217, LIQUID_MODEM_QPSK); }

LIQUID_AUTOTEST(ofdmflexframegen_config,"ofdmflexframegen config","",0.1)
{
    // check invalid function calls
    _liquid_error_downgrade_enable();
    //LIQUID_CHECK(NULL ==ofdmflexframegen_copy(NULL));
    LIQUID_CHECK(NULL ==ofdmflexframegen_create( 0, 16, 4, NULL, NULL)) // too few subcarriers
    LIQUID_CHECK(NULL ==ofdmflexframegen_create( 7, 16, 4, NULL, NULL)) // too few subcarriers
    LIQUID_CHECK(NULL ==ofdmflexframegen_create(65, 16, 4, NULL, NULL)) // odd-length subcarriers
    LIQUID_CHECK(NULL ==ofdmflexframegen_create(64, 66, 4, NULL, NULL)) // cyclic prefix length too large

    // create proper object and test configurations
    ofdmflexframegen q = ofdmflexframegen_create(64, 16, 4, NULL, NULL);

    LIQUID_CHECK(LIQUID_OK == ofdmflexframegen_print(q))

    ofdmflexframegen_destroy(q);
    _liquid_error_downgrade_disable();
}

LIQUID_AUTOTEST(ofdmflexframesync_config,"ofdmflexframesync config","",0.1)
{
    // check invalid function calls
    _liquid_error_downgrade_enable();
    //LIQUID_CHECK(NULL ==ofdmflexframesync_copy(NULL));
    LIQUID_CHECK(NULL ==ofdmflexframesync_create( 0, 16, 4, NULL, NULL, NULL)) // too few subcarriers
    LIQUID_CHECK(NULL ==ofdmflexframesync_create( 7, 16, 4, NULL, NULL, NULL)) // too few subcarriers
    LIQUID_CHECK(NULL ==ofdmflexframesync_create(65, 16, 4, NULL, NULL, NULL)) // odd-length subcarriers
    LIQUID_CHECK(NULL ==ofdmflexframesync_create(64, 66, 4, NULL, NULL, NULL)) // cyclic prefix length too large

    // create proper object and test configurations
    ofdmflexframesync q = ofdmflexframesync_create(64, 16, 4, NULL, NULL, NULL);

    LIQUID_CHECK(LIQUID_OK == ofdmflexframesync_print(q))

    ofdmflexframesync_destroy(q);
    _liquid_error_downgrade_disable();
}

