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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "autotest/autotest.h"
#include "liquid.h"

// AUTOTEST : test simple recovery of frame in noise
void testbench_ofdmflexframe(unsigned int      _M,
                             unsigned int      _cp_len,
                             unsigned int      _taper_len,
                             unsigned int      _payload_len,
                             modulation_scheme _ms)
{
    // create frame generator/synchronizer
    ofdmflexframegenprops_s fgprops;
    ofdmflexframegenprops_init_default(&fgprops);
    fgprops.check        = LIQUID_CRC_32;
    fgprops.fec0         = LIQUID_FEC_NONE;
    fgprops.fec1         = LIQUID_FEC_NONE;
    fgprops.mod_scheme   = _ms;
    ofdmflexframegen  fg = ofdmflexframegen_create( _M, _cp_len, _taper_len, NULL, &fgprops);
    ofdmflexframesync fs = ofdmflexframesync_create(_M, _cp_len, _taper_len, NULL, NULL, NULL);

    // initialize header and payload
    unsigned char header[8] = {0, 1, 2, 3, 4, 5, 6, 7};
    unsigned char payload[_payload_len];
    memset(payload, 0x00, _payload_len);

    // assemble the frame
    ofdmflexframegen_assemble(fg, header, payload, _payload_len);
    if (liquid_autotest_verbose)
        ofdmflexframegen_print(fg);

    // generate the frame
    unsigned int  buf_len = 1024;
    float complex buf[buf_len];
    int frame_complete = 0;
    while (!frame_complete) {
        frame_complete = ofdmflexframegen_write(fg, buf, buf_len);
        ofdmflexframesync_execute(fs, buf, buf_len);
    }

    // get frame data statistics
    if (liquid_autotest_verbose)
        ofdmflexframesync_print(fs);

    // verify frame data statistics
    framedatastats_s stats = ofdmflexframesync_get_framedatastats(fs);
    CONTEND_EQUALITY( stats.num_frames_detected, 1 );
    CONTEND_EQUALITY( stats.num_headers_valid,   1 );
    CONTEND_EQUALITY( stats.num_payloads_valid,  1 );
    CONTEND_EQUALITY( stats.num_bytes_received,  _payload_len );

    // destroy objects
    ofdmflexframegen_destroy(fg);
    ofdmflexframesync_destroy(fs);
}

//                          ID                                M  CP  TP  PAYL  modulation scheme
void autotest_ofdmflexframe_00() { testbench_ofdmflexframe(  32,  8,  4,  800, LIQUID_MODEM_QPSK); }
void autotest_ofdmflexframe_01() { testbench_ofdmflexframe(  64,  8,  4,  800, LIQUID_MODEM_QPSK); }
void autotest_ofdmflexframe_02() { testbench_ofdmflexframe( 256,  8,  4,  800, LIQUID_MODEM_QPSK); }
void autotest_ofdmflexframe_03() { testbench_ofdmflexframe(1024, 16,  8,  800, LIQUID_MODEM_QPSK); }
void autotest_ofdmflexframe_04() { testbench_ofdmflexframe(2048, 32, 16,  800, LIQUID_MODEM_QPSK); }
void autotest_ofdmflexframe_05() { testbench_ofdmflexframe(4096, 64, 32,  800, LIQUID_MODEM_QPSK); }
void autotest_ofdmflexframe_06() { testbench_ofdmflexframe(8192, 80, 40,  800, LIQUID_MODEM_QPSK); }
void autotest_ofdmflexframe_07() { testbench_ofdmflexframe(1200, 40, 20,    1, LIQUID_MODEM_QPSK); }
void autotest_ofdmflexframe_08() { testbench_ofdmflexframe(1200,  0,  0,  800, LIQUID_MODEM_QPSK); }
void autotest_ofdmflexframe_09() { testbench_ofdmflexframe(1200, 40, 20, 8217, LIQUID_MODEM_QPSK); }

