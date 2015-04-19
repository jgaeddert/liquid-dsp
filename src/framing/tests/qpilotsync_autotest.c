//
// qpilotsync_example.c
//
// This example demonstrates...
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "autotest/autotest.h"
#include "liquid.h"

#define DEBUG_QPILOTSYNC_AUTOTEST 1

// 
// AUTOTEST : test simple recovery of frame in noise
//
//  _ms             :   modulation scheme (e.g. LIQUID_MODEM_QPSK)
//  _payload_len    :   payload length [symbols]
//  _pilot_spacing  :   spacing between pilot symbols
//  _dphi           :   carrier frequency offset
//  _phi            :   carrier phase offset
//  _gamma          :   channel gain
//  _SNRdB          :   signal-to-noise ratio [dB]
void qpilotsync_test(modulation_scheme _ms,
                     unsigned int      _payload_len,
                     unsigned int      _pilot_spacing,
                     float             _dphi,
                     float             _phi,
                     float             _gamma,
                     float             _SNRdB)
{
    unsigned int i;
    // derived values
    float nstd = powf(10.0f, -_SNRdB/20.0f);

    // create pilot generator and synchronizer objects
    qpilotgen  pg = qpilotgen_create( _payload_len, _pilot_spacing);
    qpilotsync ps = qpilotsync_create(_payload_len, _pilot_spacing);
    qpilotgen_print(pg);

    // get frame length
    unsigned int frame_len = qpilotgen_get_frame_len(pg);

    // allocate arrays
    unsigned char payload_sym_tx[_payload_len]; // transmitted payload symbols
    float complex payload_tx    [_payload_len]; // transmitted payload samples
    float complex frame_tx      [frame_len];    // transmitted frame samples
    float complex frame_rx      [frame_len];    // received frame samples
    float complex payload_rx    [_payload_len]; // received payload samples
    unsigned char payload_sym_rx[_payload_len]; // received payload symbols

    // create modem objects for payload
    modem mod = modem_create(_ms);
    modem dem = modem_create(_ms);

    // assemble payload symbols
    for (i=0; i<_payload_len; i++) {
        // generate random symbol
        payload_sym_tx[i] = modem_gen_rand_sym(mod);

        // modulate
        modem_modulate(mod, payload_sym_tx[i], &payload_tx[i]);
    }

    // assemble frame
    qpilotgen_execute(pg, payload_tx, frame_tx);

    // add channel impairments
    for (i=0; i<frame_len; i++) {
        // add carrier offset
        frame_rx[i]  = frame_tx[i] * cexpf(_Complex_I*_dphi*i + _Complex_I*_phi);

        // add noise
        frame_rx[i] += nstd*(randnf() + _Complex_I*randnf())*M_SQRT1_2;

        // apply channel gain
        frame_rx[i] *= _gamma;
    }

    // recieve frame
    qpilotsync_execute(ps, frame_rx, payload_rx);

    // demodulate
    for (i=0; i<_payload_len; i++) {
        unsigned int sym_demod;
        modem_demodulate(dem, payload_rx[i], &sym_demod);
        payload_sym_rx[i] = (unsigned char)sym_demod;
    }

    // count errors
    unsigned int bit_errors = 0;
    for (i=0; i<_payload_len; i++)
        bit_errors += count_bit_errors(payload_sym_rx[i], payload_sym_tx[i]);
    printf("received bit errors : %u / %u\n", bit_errors, _payload_len * modem_get_bps(mod));

    // get estimates
    float dphi_hat  = qpilotsync_get_dphi(ps);
    float phi_hat   = qpilotsync_get_phi (ps);
    float gamma_hat = qpilotsync_get_gain(ps);

    CONTEND_DELTA(  dphi_hat,  _dphi, 0.01f );
    CONTEND_DELTA(   phi_hat,   _phi, 0.10f );
    CONTEND_DELTA( gamma_hat, _gamma, 0.05f );

    // check to see that frame was recovered
    CONTEND_EQUALITY( bit_errors, 0 );
    //CONTEND_SAME_DATA( payload_tx, payload_rx, _payload_len );
    
    // destroy allocated objects
    qpilotgen_destroy(pg);
    qpilotsync_destroy(ps);
    modem_destroy(mod);
    modem_destroy(dem);

#if DEBUG_QPILOTSYNC_AUTOTEST
    // write symbols to output file for plotting
    char filename[256];
    sprintf(filename,"qpilotsync_autotest_debug.m");
    FILE * fid = fopen(filename,"w");
    if (!fid) {
        fprintf(stderr,"error: could not open '%s' for writing\n", filename);
        return;
    }
    fprintf(fid,"%% %s : auto-generated file\n", filename);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"payload_len = %u;\n", _payload_len);
    fprintf(fid,"frame_len   = %u;\n", frame_len);
    fprintf(fid,"frame_tx   = zeros(1,frame_len);\n");
    fprintf(fid,"payload_rx = zeros(1,payload_len);\n");
    for (i=0; i<frame_len; i++)
        fprintf(fid,"frame_rx(%6u) = %12.4e + 1i*%12.4e;\n", i+1, crealf(frame_rx[i]), cimagf(frame_rx[i]));
    for (i=0; i<_payload_len; i++)
        fprintf(fid,"payload_rx(%6u) = %12.4e + 1i*%12.4e;\n", i+1, crealf(payload_rx[i]), cimagf(payload_rx[i]));
    fprintf(fid,"figure('Color','white','position',[100 100 950 400]);\n");
    fprintf(fid,"subplot(1,2,1);\n");
    fprintf(fid,"  plot(real(frame_rx),  imag(frame_rx),  'x','MarkerSize',3);\n");
    fprintf(fid,"  axis([-1 1 -1 1]*1.5);\n");
    fprintf(fid,"  axis square;\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  xlabel('real');\n");
    fprintf(fid,"  ylabel('imag');\n");
    fprintf(fid,"  title('received');\n");
    fprintf(fid,"subplot(1,2,2);\n");
    fprintf(fid,"  plot(real(payload_rx),imag(payload_rx),'x','MarkerSize',3);\n");
    fprintf(fid,"  axis([-1 1 -1 1]*1.5);\n");
    fprintf(fid,"  axis square;\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  xlabel('real');\n");
    fprintf(fid,"  ylabel('imag');\n");
    fprintf(fid,"  title('recovered');\n");

    fclose(fid);
    printf("results written to '%s'\n", filename);
#endif
}

void autotest_qpilotsync_qpsk() { qpilotsync_test(LIQUID_MODEM_QPSK,100,10,0.0f,0.0f,1.0f,40.0f); }

