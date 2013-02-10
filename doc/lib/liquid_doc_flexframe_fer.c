//
// flexframesync_sim.c
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>

#include "liquid.doc.h"

static int flexframe_fer_callback(unsigned char *  _header,
                                      int              _header_valid,
                                      unsigned char *  _payload,
                                      unsigned int     _payload_len,
                                      int              _payload_valid,
                                      framesyncstats_s _stats,
                                      void *           _userdata);

typedef struct {
    int frame_detected;
    int header_decoded;
    int payload_decoded;
} flexframe_fer_simdata;

// simulate frame detection and error rates for flexframe
void flexframe_fer(flexframe_fer_opts _opts,
                       float                  _SNRdB,
                       fer_results *          _results)
{
    // TODO: validate options
    // get options
    unsigned int num_frames = _opts.num_frames;
    unsigned int payload_len= _opts.payload_len;
    float noise_floor       = -60.0f;
    float SNRdB             = _SNRdB;
    int verbose             = _opts.verbose;

    // frame generater properties
    flexframegenprops_s fgprops;
    flexframegenprops_init_default(&fgprops);
    fgprops.check           = _opts.check;
    fgprops.fec0            = _opts.fec0;
    fgprops.fec1            = _opts.fec1;
    fgprops.mod_scheme      = _opts.ms;

    // bookkeeping variables
    unsigned int i, j;
    flexframe_fer_simdata simdata;

    // create flexframegen object
    flexframegen fg = flexframegen_create(&fgprops);
    //flexframegen_print(fg);

    // frame synchronizer
    flexframesync fs = flexframesync_create(flexframe_fer_callback,(void*)&simdata);
    //flexframesync_print(fs);

    unsigned char header[14];
    unsigned char payload[payload_len];
    unsigned int symbol_len = 2;
    float complex buffer[symbol_len];

    // channel objects
    nco_crcf nco_channel = nco_crcf_create(LIQUID_VCO);
    float nstd  = powf(10.0f, noise_floor/20.0f);         // noise std. dev.
    float gamma = powf(10.0f, (SNRdB+noise_floor)/20.0f); // channel gain

    //
    // ---------- BEGIN TRIALS ----------
    //

    // initalize results field
    _results->num_frames        = 0;
    _results->num_missed_frames = 0;
    _results->num_header_errors = 0;
    _results->num_packet_errors = 0;

    for (j=0; j<num_frames; j++) {
        // reset generator/synchronizer objects
        flexframegen_reset(fg);
        flexframesync_reset(fs);

        _results->num_frames++;

        simdata.frame_detected  = 0;
        simdata.header_decoded  = 0;
        simdata.payload_decoded = 0;

        // generate random packet data, encode
        for (i=0; i<14; i++)
            header[i] = rand() & 0xff;
        for (i=0; i<payload_len; i++)
            payload[i] = rand() & 0xff;

        // encode
        flexframegen_assemble(fg,header,payload,payload_len);

        // initialize frame synchronizer with noise
        for (i=0; i<(rand()%100)+400; i++) {
            float complex noise = nstd * (randnf() + _Complex_I*randnf()) * M_SQRT1_2;
            flexframesync_execute(fs, &noise, 1);
        }

        // generate frame
        int frame_complete=0;
        while (!frame_complete) {
            // generate symbol
            frame_complete = flexframegen_write_samples(fg, buffer);

            // apply channel
            for (i=0; i<symbol_len; i++) {
                buffer[i] *= gamma;
                buffer[i] += nstd * (randnf() + _Complex_I*randnf()) * M_SQRT1_2;
            }

            // receive symbol
            flexframesync_execute(fs, buffer, symbol_len);
        }

        // accumulate results
        _results->num_missed_frames += simdata.frame_detected  ? 0 : 1;
        _results->num_header_errors += simdata.header_decoded  ? 0 : 1;
        _results->num_packet_errors += simdata.payload_decoded ? 0 : 1;

        // peridically print results
        if ( (((j+1) % 10)==0 || (j==num_frames-1)) && verbose) {
            float FER = (float) _results->num_missed_frames / (float) _results->num_frames;
            float HER = (float) _results->num_header_errors / (float) _results->num_frames;
            float PER = (float) _results->num_packet_errors / (float) _results->num_frames;

            printf(" SNR: %5.2f[%6u/%6u] frames:%6u(%5.1f%%) headers:%6u(%5.1f%%) packets:%6u(%5.1f%%)\r",
                    SNRdB,
                    j+1, num_frames,
                    _results->num_missed_frames, FER*100,
                    _results->num_header_errors, HER*100,
                    _results->num_packet_errors, PER*100);
            fflush(stdout);
        }

    } // num_frames
        
    // clean up objects
    flexframegen_destroy(fg);
    flexframesync_destroy(fs);
    nco_crcf_destroy(nco_channel);

    // update statistics
    _results->FER = (float) _results->num_missed_frames / (float) _results->num_frames;
    _results->HER = (float) _results->num_header_errors / (float) _results->num_frames;
    _results->PER = (float) _results->num_packet_errors / (float) _results->num_frames;

    if ( verbose) {
        // print new line (refesh buffer)
        printf("\n");
    }
    return;
}

// static callback function
static int flexframe_fer_callback(unsigned char *  _header,
                                  int              _header_valid,
                                  unsigned char *  _payload,
                                  unsigned int     _payload_len,
                                  int              _payload_valid,
                                  framesyncstats_s _stats,
                                  void *           _userdata)
{
    flexframe_fer_simdata * simdata = (flexframe_fer_simdata*) _userdata;

    // specify that frame was detected
    simdata->frame_detected = 1;

    // check if header was properly decoded
    if (_header_valid)
        simdata->header_decoded = 1;

    // check if payload was properly decoded
    if (_payload_valid)
        simdata->payload_decoded = 1;

    return 0;
}

