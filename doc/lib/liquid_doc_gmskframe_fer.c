//
// gmskframesync_sim.c
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>

#include "liquid.doc.h"

static int gmskframe_fer_callback(unsigned char *  _rx_header,
                                  int              _rx_header_valid,
                                  unsigned char *  _rx_payload,
                                  unsigned int     _rx_payload_len,
                                  int              _rx_payload_valid,
                                  framesyncstats_s _stats,
                                  void *           _userdata);

typedef struct {
    int frame_detected;
    int header_decoded;
    int payload_decoded;
} gmskframe_fer_simdata;

// simulate frame detection and error rates for gmskframe
void gmskframe_fer(gmskframe_fer_opts _opts,
                   float              _SNRdB,
                   fer_results *      _results)
{
    // TODO: validate options
    // get options
    unsigned int k          = _opts.k;
    unsigned int m          = _opts.m;
    float BT                = _opts.BT;
    crc_scheme check        = _opts.check;
    fec_scheme fec0         = _opts.fec0;
    fec_scheme fec1         = _opts.fec1;
    unsigned int num_frames = _opts.num_frames;
    unsigned int payload_len= _opts.payload_len;
    float noise_floor       = -60.0f;
    float SNRdB             = _SNRdB;
    int verbose             = _opts.verbose;

    // bookkeeping variables
    unsigned int i, j;
    gmskframe_fer_simdata simdata;

    // create gmskframegen object
    gmskframegen fg = gmskframegen_create(k, m, BT);

    // frame synchronizer
    gmskframesync fs = gmskframesync_create(k,m,BT,gmskframe_fer_callback,(void*)&simdata);

    // allocate buffers
    unsigned char header[8];            // header data
    unsigned char payload[payload_len]; // payload data
    float complex buffer[k];            // symbol buffer

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
        gmskframegen_reset(fg);
        gmskframesync_reset(fs);

        _results->num_frames++;

        simdata.frame_detected  = 0;
        simdata.header_decoded  = 0;
        simdata.payload_decoded = 0;

        // generate random packet data, encode
        for (i=0; i<8; i++)
            header[i] = rand() & 0xff;
        for (i=0; i<payload_len; i++)
            payload[i] = rand() & 0xff;

        // assemble frame
        gmskframegen_assemble(fg,header,payload,payload_len,check,fec0,fec1);

        // initialize frame synchronizer with noise
        for (i=0; i<(rand()%100)+400; i++) {
            float complex noise = nstd * (randnf() + _Complex_I*randnf()) * M_SQRT1_2;
            gmskframesync_execute(fs, &noise, 1);
        }

        // generate frame
        int frame_complete = 0;
        while (!frame_complete) {
            // generate symbol
            frame_complete = gmskframegen_write_samples(fg, buffer);

            // apply channel
            for (i=0; i<k; i++) {
                buffer[i] *= gamma;
                buffer[i] += nstd * (randnf() + _Complex_I*randnf()) * M_SQRT1_2;
            }

            // push samples through synchronizer
            gmskframesync_execute(fs, buffer, k);
        }

        // flush frame synchronizer
        for (i=0; i<2*k*m; i++) {
            float complex noise = nstd * (randnf() + _Complex_I*randnf()) * M_SQRT1_2;
            gmskframesync_execute(fs, &noise, 1);
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

            printf(" SNR: %5.2f frames:%5u/%5u(%5.1f%%) headers:%5u/%5u(%5.1f%%) packets:%5u/%5u(%5.1f%%)\r",
                    SNRdB,
                    _results->num_missed_frames, _results->num_frames, FER*100,
                    _results->num_header_errors, _results->num_frames, HER*100,
                    _results->num_packet_errors, _results->num_frames, PER*100);
            fflush(stdout);
        }

    } // num_frames
        
    // clean up objects
    gmskframegen_destroy(fg);
    gmskframesync_destroy(fs);
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
static int gmskframe_fer_callback(unsigned char *  _rx_header,
                                  int              _rx_header_valid,
                                  unsigned char *  _rx_payload,
                                  unsigned int     _rx_payload_len,
                                  int              _rx_payload_valid,
                                  framesyncstats_s _stats,
                                  void *           _userdata)
{
    gmskframe_fer_simdata * simdata = (gmskframe_fer_simdata*) _userdata;

    // specify that frame was detected
    simdata->frame_detected = 1;

    if (!_rx_header_valid)
        return 0;
    
    // specify that header was detected
    simdata->header_decoded = 1;

    if (_rx_payload_valid)
        simdata->payload_decoded = 1;

    return 0;
}

