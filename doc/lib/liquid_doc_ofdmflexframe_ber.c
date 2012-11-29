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
    unsigned int num_trials;
    unsigned int num_packets_found;
    unsigned int num_headers_decoded;
    unsigned int num_payloads_decoded;
    unsigned int payload_len;
    unsigned char * rx_packet;
} ofdmflexframesync_per_simdata;

void ofdmflexframesync_per(ofdmflexframegenprops_s * _fgprops,
                           unsigned int _M,
                           unsigned int _cp_len,
                           unsigned char * _p,
                           void * _fsprops,
                           unsigned int _payload_len,
                           float _noise_floor_dB,
                           float _SNRdB,
                           float _dphi,
                           unsigned int _num_trials,
                           unsigned int * _num_packets_found,
                           unsigned int * _num_headers_decoded,
                           unsigned int * _num_payloads_decoded)
{
    // define parameters
#if 0
    float dphi = _dphi; // carrier frequency offset
    float phi = 0.0f;   // carrier phase offset
#endif
    unsigned int num_trials = _num_trials;
    float noise_floor = _noise_floor_dB;

    // validate options

    // bookkeeping variables
    unsigned int i, j;
    float SNRdB = _SNRdB;
    ofdmflexframesync_per_simdata simdata;

    unsigned char rx_packet[_payload_len];
    simdata.payload_len = _payload_len;
    simdata.rx_packet = rx_packet;

    unsigned int taper_len = 0;

    // create ofdmflexframegen object
    ofdmflexframegen fg = ofdmflexframegen_create(_M, _cp_len, taper_len, _p, _fgprops);
    //ofdmflexframegen_print(fg);

    // frame synchronizer
    ofdmflexframesync fs = ofdmflexframesync_create(_M,_cp_len,taper_len,_p,ofdmflexframesync_per_callback,(void*)&simdata);
    //ofdmflexframesync_print(fs);

    unsigned char header[8];
    unsigned char payload[_payload_len];
    unsigned int symbol_len = _M + _cp_len;
    float complex buffer[symbol_len];

    // channel objects
    nco_crcf nco_channel = nco_crcf_create(LIQUID_VCO);
    float nstd  = powf(10.0f, noise_floor/20.0f);         // noise std. dev.
    float gamma = powf(10.0f, (SNRdB+noise_floor)/20.0f); // channel gain

    //
    // ---------- BEGIN TRIALS ----------
    //

    // start trials for a particular SNR
    simdata.num_trials = 0;
    simdata.num_packets_found = 0;
    simdata.num_headers_decoded = 0;
    simdata.num_payloads_decoded = 0;

    for (j=0; j<num_trials; j++) {
        // reset synchronizer
        //printf("resetting gen/sync\n");
        ofdmflexframegen_reset(fg);
        ofdmflexframesync_reset(fs);

        simdata.num_trials++;

        // generate random packet data, encode
        //printf("generating random packet\n");
        for (i=0; i<8; i++)
            header[i] = rand() & 0xff;
        for (i=0; i<_payload_len; i++)
            payload[i] = rand() & 0xff;

        // encode
        //printf("encoding frame\n");
        ofdmflexframegen_assemble(fg,header,payload,_payload_len);

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


#if 0
        // add channel impairments
        //printf("channel impairments\n");
        nco_crcf_set_frequency(nco_channel,dphi);
        nco_crcf_set_phase(nco_channel,phi);
        for (i=0; i<2*frame_len; i++) {
            frame_interp[i] *= gamma;
            cawgn(&frame_interp[i],nstd);
            nco_crcf_mix_up(nco_channel, frame_interp[i], &frame_interp[i]);
            nco_crcf_step(nco_channel);
        }

        // run synchronizer
        //printf("running sync\n");
        ofdmflexframesync_execute(fs, frame_interp, 2*frame_len);

        // push noise
        for (i=0; i<frame_len; i++)
            frame[i] = (randnf() + _Complex_I*randnf()) * nstd;
        ofdmflexframesync_execute(fs,frame,frame_len);
#endif

    } // num_trials

#if 0
    printf("SNR %12.8f [dB] : %6u %6u %6u %6u\n",
        SNRdB,
        simdata.num_trials,
        simdata.num_packets_found,
        simdata.num_headers_decoded,
        simdata.num_payloads_decoded);
#endif

    *_num_packets_found     = simdata.num_packets_found;
    *_num_headers_decoded   = simdata.num_headers_decoded;
    *_num_payloads_decoded  = simdata.num_payloads_decoded;

    // clean up objects
    ofdmflexframegen_destroy(fg);
    ofdmflexframesync_destroy(fs);
    nco_crcf_destroy(nco_channel);

    return;
}

static int ofdmflexframesync_per_callback(unsigned char *  _rx_header,
                                          int              _rx_header_valid,
                                          unsigned char *  _rx_payload,
                                          unsigned int     _rx_payload_len,
                                          int              _rx_payload_valid,
                                          framesyncstats_s _stats,
                                          void *           _userdata)
{
    ofdmflexframesync_per_simdata * simdata = (ofdmflexframesync_per_simdata*) _userdata;

    simdata->num_packets_found++;
    simdata->num_headers_decoded  += (_rx_header_valid) ? 1 : 0;

    if (!_rx_header_valid)
        return 0;

    if (_rx_payload_valid)
        simdata->num_payloads_decoded++;

#if 0
    printf("callback invoked : %6u %6u %6u %6u\n",
        simdata->num_trials,
        simdata->num_packets_found,
        simdata->num_headers_decoded,
        simdata->num_payloads_decoded);
#endif
    return 0;
}

