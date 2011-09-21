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

#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <math.h>

#include "liquid.h"

static int callback(unsigned char *  _header,
                    int              _header_valid,
                    unsigned char *  _payload,
                    unsigned int     _payload_len,
                    int              _payload_valid,
                    framesyncstats_s _stats,
                    void *           _userdata)
{
    unsigned int * num_frames_rx = (unsigned int *) _userdata;

    // increment number of received frames if valid
    *num_frames_rx += _payload_valid ? 1 : 0;

    return 0;
}

// benchmark regular frame synchronizer with short frames; effectively
// test acquisition complexity
void benchmark_gmskframesync(struct rusage *_start,
                             struct rusage *_finish,
                             unsigned long int *_num_iterations)
{
    *_num_iterations /= 128;
    unsigned long int i;

    // options
    unsigned int k = 2;                 // filter samples/symbol
    unsigned int m = 4;                 // filter semi-length (symbols)
    float BT = 0.4f;                    // filter excess bandwidth
    unsigned int payload_len = 8;       // length of payload (bytes)
    crc_scheme check = LIQUID_CRC_32;   // data validity check
    fec_scheme fec0  = LIQUID_FEC_NONE; // inner forward error correction
    fec_scheme fec1  = LIQUID_FEC_NONE; // outer forward error correction
    float SNRdB = 20.0f;                // SNR

    // derived values
    float nstd  = powf(10.0f, -SNRdB/20.0f);

    // allocate memory for payload and initialize
    unsigned char header[8];
    unsigned char payload[payload_len];
    for (i=0; i<8; i++)           header[i]  = rand() & 0xff;
    for (i=0; i<payload_len; i++) payload[i] = rand() & 0xff;
    
    // frames received counter
    unsigned int num_frames_rx = 0;

    // create frame generator
    gmskframegen fg = gmskframegen_create(k, m, BT);

    // create frame synchronizer
    gmskframesync fs = gmskframesync_create(k, m, BT, callback, (void*)&num_frames_rx);

    // assemble frame and print
    gmskframegen_assemble(fg, header, payload, payload_len, check, fec0, fec1);
    gmskframegen_print(fg);

    // allocate memory for full frame (with noise)
    unsigned int frame_len = gmskframegen_get_frame_len(fg);
    unsigned int num_samples = (frame_len * k) + 40;
    float complex x[num_samples];
    float complex y[num_samples];

    // 
    // generate frame
    //
    unsigned int n=0;
    for (n=0; n<20; n++)
        x[n] = 0.0f;
    int frame_complete = 0;
    while (!frame_complete) {
        frame_complete = gmskframegen_write_samples(fg, &x[n]);
        n += k;
    }
    for ( ; n<num_samples; n++)
        x[n] = 0.0f;

    // add channel impairments
    for (i=0; i<num_samples; i++) {
        y[i] = x[i] += nstd*(randnf() + randnf()*_Complex_I)*M_SQRT1_2;
    }

    // 
    // start trials
    //
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        // push samples through synchronizer
        gmskframesync_execute(fs, y, num_samples);
    }
    getrusage(RUSAGE_SELF, _finish);

    // destroy framing objects
    gmskframegen_destroy(fg);
    gmskframesync_destroy(fs);
    
    // print results
    printf("  gmsk frames received  :   %6u / %6lu\n",
            num_frames_rx,
            *_num_iterations);
}

// benchmark regular frame synchronizer with noise; essentially test
// complexity when no signal is present
void benchmark_gmskframesync_noise(struct rusage *_start,
                                   struct rusage *_finish,
                                   unsigned long int *_num_iterations)
{
    *_num_iterations /= 400;
    unsigned long int i;

    // options
    unsigned int k = 2;                 // filter samples/symbol
    unsigned int m = 4;                 // filter semi-length (symbols)
    float BT = 0.4f;                    // filter excess bandwidth
    float SNRdB = 20.0f;                // SNR

    // derived values
    float nstd  = powf(10.0f, -SNRdB/20.0f);

    // create frame synchronizer
    gmskframesync fs = gmskframesync_create(k, m, BT, NULL, NULL);

    // allocate memory for noise buffer and initialize
    unsigned int num_samples = 1024;
    float complex y[num_samples];
    for (i=0; i<num_samples; i++)
        y[i] = nstd*(randnf() + randnf()*_Complex_I)*M_SQRT1_2;

    // 
    // start trials
    //
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        // push samples through synchronizer
        gmskframesync_execute(fs, y, num_samples);
    }
    getrusage(RUSAGE_SELF, _finish);

    // scale result by number of samples in buffer
    *_num_iterations *= num_samples;

    // destroy framing objects
    gmskframesync_destroy(fs);
}

