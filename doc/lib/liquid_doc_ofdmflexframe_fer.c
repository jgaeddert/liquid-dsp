//
// ofdmflexframesync_sim.c
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>

#include "liquid.doc.h"

static int ofdmflexframesync_per_callback(unsigned char *  _rx_header,
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
} ofdmflexframesync_per_simdata;

// simulate frame detection and error rates for ofdmflexframe
void ofdmflexframesync_fer(ofdmflexframesync_fer_opts      _opts,
                           float                           _SNRdB,
                           ofdmflexframesync_fer_results * _results)
{
    // TODO: validate options
    // get options
    unsigned int M              = _opts.M;
    unsigned int cp_len         = _opts.cp_len;
    unsigned int taper_len      = 0;
    unsigned char * p           = _opts.p;
    unsigned int num_frames     = _opts.num_frames;
    unsigned int payload_len    = _opts.payload_len;
    float noise_floor           = -60.0f;
    float SNRdB                 = _SNRdB;

    // frame generater properties
    ofdmflexframegenprops_s fgprops;
    ofdmflexframegenprops_init_default(&fgprops);
    fgprops.check           = _opts.check;
    fgprops.fec0            = _opts.fec0;
    fgprops.fec1            = _opts.fec1;
    fgprops.mod_scheme      = _opts.ms;

    // bookkeeping variables
    unsigned int i, j;
    ofdmflexframesync_per_simdata simdata;

    // create ofdmflexframegen object
    ofdmflexframegen fg = ofdmflexframegen_create(M, cp_len, taper_len, p, &fgprops);
    //ofdmflexframegen_print(fg);

    // frame synchronizer
    ofdmflexframesync fs = ofdmflexframesync_create(M,cp_len,taper_len,p,ofdmflexframesync_per_callback,(void*)&simdata);
    //ofdmflexframesync_print(fs);

    unsigned char header[8];
    unsigned char payload[payload_len];
    unsigned int symbol_len = M + cp_len;
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
        ofdmflexframegen_reset(fg);
        ofdmflexframesync_reset(fs);

        _results->num_frames++;

        simdata.frame_detected  = 0;
        simdata.header_decoded  = 0;
        simdata.payload_decoded = 0;

        // generate random packet data, encode
        for (i=0; i<8; i++)
            header[i] = rand() & 0xff;
        for (i=0; i<payload_len; i++)
            payload[i] = rand() & 0xff;

        // encode
        ofdmflexframegen_assemble(fg,header,payload,payload_len);

        // initialize frame synchronizer with noise
        for (i=0; i<(rand()%100)+400; i++) {
            float complex noise = nstd * (randnf() + _Complex_I*randnf()) * M_SQRT1_2;
            ofdmflexframesync_execute(fs, &noise, 1);
        }

        // generate frame
        int last_symbol=0;
        while (!last_symbol) {
            // generate symbol
            last_symbol = ofdmflexframegen_writesymbol(fg, buffer);

            // apply channel
            for (i=0; i<symbol_len; i++) {
                buffer[i] *= gamma;
                buffer[i] += nstd * (randnf() + _Complex_I*randnf()) * M_SQRT1_2;
            }

            // receive symbol
            ofdmflexframesync_execute(fs, buffer, symbol_len);
        }

        // accumulate results
        _results->num_missed_frames += simdata.frame_detected  ? 0 : 1;
        _results->num_header_errors += simdata.header_decoded  ? 0 : 1;
        _results->num_packet_errors += simdata.payload_decoded ? 0 : 1;

        // peridically print results
        if ( (((j+1) % 50)==0 || (j==num_frames-1)) && _opts.verbose) {
            float FER = (float) _results->num_missed_frames / (float) _results->num_frames;
            float HER = (float) _results->num_header_errors / (float) _results->num_frames;
            float PER = (float) _results->num_packet_errors / (float) _results->num_frames;

            printf(" SNR: %5.2f frames:%6u/%6u (%5.1f%%) headers:%6u/%6u (%5.1f%%) packets:%6u/%6u (%5.1f%%)\r",
                    SNRdB,
                    _results->num_missed_frames, _results->num_frames, FER*100,
                    _results->num_header_errors, _results->num_frames, HER*100,
                    _results->num_packet_errors, _results->num_frames, PER*100);
            fflush(stdout);
        }

    } // num_frames
        
    // clean up objects
    ofdmflexframegen_destroy(fg);
    ofdmflexframesync_destroy(fs);
    nco_crcf_destroy(nco_channel);

    // update statistics
    _results->FER = (float) _results->num_missed_frames / (float) _results->num_frames;
    _results->HER = (float) _results->num_header_errors / (float) _results->num_frames;
    _results->PER = (float) _results->num_packet_errors / (float) _results->num_frames;

    if ( _opts.verbose) {
        // print new line (refesh buffer)
        printf("\n");
    }
    return;
}

// static callback function
static int ofdmflexframesync_per_callback(unsigned char *  _rx_header,
                                          int              _rx_header_valid,
                                          unsigned char *  _rx_payload,
                                          unsigned int     _rx_payload_len,
                                          int              _rx_payload_valid,
                                          framesyncstats_s _stats,
                                          void *           _userdata)
{
    ofdmflexframesync_per_simdata * simdata = (ofdmflexframesync_per_simdata*) _userdata;

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

