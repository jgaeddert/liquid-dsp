/*
 * Copyright (c) 2007, 2009 Joseph Gaeddert
 * Copyright (c) 2007, 2009 Virginia Polytechnic Institute & State University
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

#ifndef __LIQUID_FLEXFRAMESYNC_BENCHMARK_H__
#define __LIQUID_FLEXFRAMESYNC_BENCHMARK_H__

#include <sys/resource.h>
#include "liquid.h"

static int callback(unsigned char * _rx_header,
                    int _rx_header_valid,
                    unsigned char * _rx_payload,
                    unsigned int _rx_payload_len,
                    void * _userdata)
{
    //printf("callback invoked\n");
    return 0;
}

// Helper function to keep code base small
void benchmark_flexframesync(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations)
{
    *_num_iterations /= 128;
    unsigned long int i;

    // create flexframegen object
    flexframegenprops_s fgprops;
    fgprops.rampup_len = 16;
    fgprops.phasing_len = 64;
    fgprops.payload_len = 2;
    fgprops.mod_scheme = MOD_PSK;
    fgprops.mod_bps = 3;
    fgprops.rampdn_len = 16;
    flexframegen fg = flexframegen_create(&fgprops);
    flexframegen_print(fg);

    // frame data
    unsigned char header[8];
    unsigned char payload[fgprops.payload_len];
    // initialize header, payload
    for (i=0; i<8; i++)
        header[i] = i;
    for (i=0; i<fgprops.payload_len; i++)
        payload[i] = rand() & 0xff;

    // create interpolator
    unsigned int m=3;
    float beta=0.7f;
    unsigned int h_len = 2*2*m + 1;
    float h[h_len];
    design_rrc_filter(2,m,beta,0,h);
    interp_crcf interp = interp_crcf_create(2,h,h_len);

    // create flexframesync object with default properties
    //flexframesyncprops_s fsprops;
    flexframesync fs = flexframesync_create(NULL,callback,NULL);
    flexframesync_print(fs);

    // generate the frame
    unsigned int frame_len = flexframegen_getframelen(fg);
    float complex frame[frame_len];
    flexframegen_execute(fg, header, payload, frame);
    unsigned int frame_interp_len = frame_len + m + 16;
    float complex frame_interp[2*frame_interp_len];
    float complex x;
    for (i=0; i<frame_interp_len; i++) {
        x = (i<frame_len) ? frame[i] : 0.0f;
        interp_crcf_execute(interp, x, &frame_interp[2*i]);
    }
    // add some noise
    for (i=0; i<2*frame_interp_len; i++)
        cawgn(&frame_interp[i], 0.01f);

    // 
    // start trials
    //
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        flexframesync_execute(fs, frame_interp, 2*frame_interp_len);
    }
    getrusage(RUSAGE_SELF, _finish);

    flexframegen_destroy(fg);
    flexframesync_destroy(fs);
    interp_crcf_destroy(interp);
}

#endif // __LIQUID_FLEXFRAMESYNC_BENCHMARK_H__
