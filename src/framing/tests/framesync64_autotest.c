/*
 * Copyright (c) 2007 - 2022 Joseph Gaeddert
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
#include "autotest/autotest.h"
#include "liquid.h"

static int callback_framesync64_autotest(
    unsigned char *  _header,
    int              _header_valid,
    unsigned char *  _payload,
    unsigned int     _payload_len,
    int              _payload_valid,
    framesyncstats_s _stats,
    void *           _userdata)
{
    //printf("callback invoked, payload valid: %s\n", _payload_valid ? "yes" : "no");
    int * frames_recovered = (int*) _userdata;

    *frames_recovered += _header_valid && _payload_valid ? 1 : 0;
    return 0;
}

// AUTOTEST : test simple recovery of frame in noise
void autotest_framesync64()
{
    unsigned int i;
    int frames_recovered = 0;

    // create objects
    framegen64 fg = framegen64_create();
    framesync64 fs = framesync64_create(callback_framesync64_autotest,
            (void*)&frames_recovered);

    if (liquid_autotest_verbose) {
        framesync64_print(fs);
        framegen64_print(fg);
    }

    // generate the frame
    float complex frame[LIQUID_FRAME64_LEN];
    framegen64_execute(fg, NULL, NULL, frame);

    // add some noise
    for (i=0; i<LIQUID_FRAME64_LEN; i++)
        frame[i] += 0.01f*(randnf() + _Complex_I*randnf()) * M_SQRT1_2;

    // try to receive the frame
    framesync64_execute(fs, frame, LIQUID_FRAME64_LEN);

    // check to see that exactly one frame was recovered
    CONTEND_EQUALITY( frames_recovered, 1 );

    // parse statistics
    framedatastats_s stats = framesync64_get_framedatastats(fs);
    CONTEND_EQUALITY(stats.num_frames_detected, 1);
    CONTEND_EQUALITY(stats.num_headers_valid,   1);
    CONTEND_EQUALITY(stats.num_payloads_valid,  1);
    CONTEND_EQUALITY(stats.num_bytes_received, 64);

    // destroy objects
    framegen64_destroy(fg);
    framesync64_destroy(fs);
}

// test copying from one object to another
void autotest_framegen64_copy()
{
    framegen64 q_orig = framegen64_create();
    framegen64 q_copy = framegen64_copy(q_orig);

    if (q_copy == NULL) {
        AUTOTEST_FAIL("could not copy framegen64 object");
    } else {
        AUTOTEST_PASS();
        framegen64_destroy(q_copy);
    }
    framegen64_destroy(q_orig);
}

// test copying from one object to another
void autotest_framesync64_copy()
{
    unsigned int i;
    int frames_recovered_0 = 0;
    int frames_recovered_1 = 0;

    // create objects
    framegen64  fg  = framegen64_create();
    framesync64 fs0 = framesync64_create(callback_framesync64_autotest,
            (void*)&frames_recovered_0);

    // feed random samples into synchronizer
    float complex buf[LIQUID_FRAME64_LEN];
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
    framesync64_set_userdata(fs1, (void*)&frames_recovered_1);

    // try to receive the frame with each receiver
    for (i=0; i<LIQUID_FRAME64_LEN; i++) {
        // step one sample through at a time
        framesync64_execute(fs0, buf+i, 1);
        framesync64_execute(fs1, buf+i, 1);

        // ensure that the frames are recovered at exactly the same time
        CONTEND_EQUALITY( frames_recovered_0, frames_recovered_1 );
    }

    // check that frame was actually recovered by each object
    CONTEND_EQUALITY( frames_recovered_0, 1 );
    CONTEND_EQUALITY( frames_recovered_1, 1 );

    // parse statistics
    framedatastats_s stats_0 = framesync64_get_framedatastats(fs0);
    framedatastats_s stats_1 = framesync64_get_framedatastats(fs1);

    CONTEND_EQUALITY(stats_0.num_frames_detected, stats_1.num_frames_detected);
    CONTEND_EQUALITY(stats_0.num_headers_valid  , stats_1.num_headers_valid  );
    CONTEND_EQUALITY(stats_0.num_payloads_valid , stats_1.num_payloads_valid );
    CONTEND_EQUALITY(stats_0.num_bytes_received , stats_1.num_bytes_received );

    // destroy objects
    framegen64_destroy(fg);
    framesync64_destroy(fs0);
    framesync64_destroy(fs1);
}

void autotest_framesync64_config()
{
#if LIQUID_STRICT_EXIT
    AUTOTEST_WARN("skipping framesync64 config test with strict exit enabled\n");
    return;
#endif
#if !LIQUID_SUPPRESS_ERROR_OUTPUT
    fprintf(stderr,"warning: ignore potential errors here; checking for invalid configurations\n");
#endif
    // check invalid function calls
    CONTEND_ISNULL(framesync64_copy(NULL));
    CONTEND_ISNULL(framegen64_copy (NULL));

    // create proper object and test configurations
    framesync64 q = framesync64_create(NULL, NULL);

    CONTEND_EQUALITY(LIQUID_OK, framesync64_print(q))
    CONTEND_EQUALITY(LIQUID_OK, framesync64_set_callback(q,callback_framesync64_autotest))
    CONTEND_EQUALITY(LIQUID_OK, framesync64_set_userdata(q,NULL))

    CONTEND_EQUALITY(LIQUID_OK, framesync64_set_threshold(q,0.654321f))
    CONTEND_EQUALITY(0.654321f, framesync64_get_threshold(q))


    framesync64_destroy(q);
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
void testbench_framesync64_debug(int _code)
{
    // create objects
    framegen64  fg = framegen64_create();
    framesync64 fs = framesync64_create(callback_framesync64_autotest_debug,(void*)(&_code));

    // set prefix for filename
    const char prefix[] = "autotest/logs/framesync64";
    framesync64_set_prefix(fs,prefix);
    CONTEND_SAME_DATA(framesync64_get_prefix(fs), prefix, strlen(prefix));

    // generate the frame
    float complex frame[LIQUID_FRAME64_LEN];
    unsigned char header [ 8] = {80,81,82,83,84,85,86,87};
    unsigned char payload[64];
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
    CONTEND_EQUALITY(stats.num_frames_detected, 1);
    CONTEND_EQUALITY(stats.num_headers_valid,   1);
    CONTEND_EQUALITY(stats.num_payloads_valid,  1);
    CONTEND_EQUALITY(stats.num_bytes_received, 64);

    // get filename from the last file written
    const char * filename = framesync64_get_filename(fs);
    printf("filename: %s\n", filename);

    // destroy objects
    framegen64_destroy(fg);
    framesync64_destroy(fs);

    // check if output file should exist
    if (filename == NULL) {
        if (_code==0) {
            AUTOTEST_PASS();
        } else {
            AUTOTEST_FAIL("no output file written when one was expected");
        }
        return;
    }

    // load file
    FILE * fid = fopen(filename,"rb");
    if (fid == NULL) {
        AUTOTEST_FAIL("could not open file for reading");
        return;
    }

    // skip to location in file that includes payload and check that
    // both the header and payload match
    fseek(fid, LIQUID_FRAME64_LEN*sizeof(float complex) +
               5*sizeof(float) +
               (630 + 600)*sizeof(float complex),
          SEEK_SET);
    unsigned char payload_dec[72];
    fread(payload_dec, sizeof(unsigned char), 72, fid);
    CONTEND_SAME_DATA(payload_dec,   header,   8);
    CONTEND_SAME_DATA(payload_dec+8, payload, 64);
    fclose(fid);
}

// test exporting debugging files with different return codes
void autotest_framesync64_debug_none() { testbench_framesync64_debug( 0); }
void autotest_framesync64_debug_user() { testbench_framesync64_debug( 1); }
void autotest_framesync64_debug_ndet() { testbench_framesync64_debug(-1); }
void autotest_framesync64_debug_head() { testbench_framesync64_debug(-2); }
void autotest_framesync64_debug_rand() { testbench_framesync64_debug(-3); }

