//
//
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "liquid.h"

#define FRAMESYNC64_SYMSYNC_BW_0    (0.01f)
#define FRAMESYNC64_SYMSYNC_BW_1    (0.001f)

#define FRAMESYNC64_AGC_BW_0        (1e-3f)
#define FRAMESYNC64_AGC_BW_1        (1e-6f)

#define FRAMESYNC64_PLL_BW_0        (0.0f)//(1e-2f)
#define FRAMESYNC64_PLL_BW_1        (0.0f)//(1e-4f)

//#define FRAME64_RAMP_UP_LEN 64
//#define FRAME64_PHASING_LEN 64
#define FRAME64_PN_LEN      64
//#define FRAME64_RAMP_DN_LEN 64

#define DEBUG
#define DEBUG_FILENAME      "framesync64_internal_debug.m"
#define DEBUG_BUFFER_LEN    2048

// Internal
void framesync64_open_bandwidth(framesync64 _fs);
void framesync64_close_bandwidth(framesync64 _fs);
void framesync64_decode_header(framesync64 _fs);
void framesync64_decode_payload(framesync64 _fs);

void framesync64_syms_to_byte(unsigned char * _syms, unsigned char * _byte);

struct framesync64_s {
    modem demod;
    fec dec;

    // synchronizer objects
    agc agc_rx;
    symsync_crcf mfdecim;
    pll pll_rx;
    nco nco_rx;
    pnsync_crcf fsync;

    // status variables
    enum {
        FRAMESYNC64_STATE_SEEKPN=0,
        FRAMESYNC64_STATE_RXHEADER,
        FRAMESYNC64_STATE_RXPAYLOAD,
        FRAMESYNC64_STATE_RESET
    } state;
    unsigned int num_symbols_collected;
    unsigned int header_key;
    unsigned int payload_key;

    framesync64_callback *callback;

    // header
    unsigned char header_sym[256];
    unsigned char header_enc[64];
    unsigned char header[32];

    // payload
    unsigned char payload_sym[512];
    unsigned char payload_enc[128];
    unsigned char payload[64];

#ifdef DEBUG
    FILE*fid;
    fwindow debug_agc_rssi;
    cfwindow debug_rxy;
    cfwindow debug_nco_rx_out;
#endif
};

framesync64 framesync64_create(
//    unsigned int _k,
    unsigned int _m,
    float _beta,
    framesync64_callback _callback)
{
    framesync64 fs = (framesync64) malloc(sizeof(struct framesync64_s));
    //fs->callback = _callback;

    //
    fs->agc_rx = agc_create(1.0f, FRAMESYNC64_AGC_BW_0);
    fs->pll_rx = pll_create();
    fs->nco_rx = nco_create();
    pll_set_bandwidth(fs->pll_rx, FRAMESYNC64_PLL_BW_0);

    // pnsync
    unsigned int i;
    msequence ms = msequence_create(6);
    float pn_sequence[FRAME64_PN_LEN];
    for (i=0; i<FRAME64_PN_LEN; i++)
        pn_sequence[i] = (msequence_advance(ms)) ? 1.0f : -1.0f;
    fs->fsync = pnsync_crcf_create(FRAME64_PN_LEN, pn_sequence);
    msequence_destroy(ms);

    // design symsync (k=2)
    unsigned int npfb = 16;
    unsigned int H_len = 2*2*npfb*_m + 1;
    float H[H_len];
    design_rrc_filter(2*npfb,_m,_beta,0,H);
    fs->mfdecim =  symsync_crcf_create(2, npfb, H, H_len-1);
    symsync_crcf_set_lf_bw(fs->mfdecim, FRAMESYNC64_SYMSYNC_BW_0);

    // create decoder
    fs->dec = fec_create(FEC_HAMMING74, NULL);

    // create demod
    fs->demod = modem_create(MOD_QPSK, 2);

    // set status flags
    fs->state = FRAMESYNC64_STATE_SEEKPN;
    fs->num_symbols_collected = 0;

#ifdef DEBUG
    fs->debug_agc_rssi  =  fwindow_create(DEBUG_BUFFER_LEN);
    fs->debug_rxy       = cfwindow_create(DEBUG_BUFFER_LEN);
    fs->debug_nco_rx_out= cfwindow_create(DEBUG_BUFFER_LEN);
#endif

    return fs;
}

void framesync64_destroy(framesync64 _fs)
{
    symsync_crcf_destroy(_fs->mfdecim);
    fec_destroy(_fs->dec);
    agc_destroy(_fs->agc_rx);
    pll_destroy(_fs->pll_rx);
    nco_destroy(_fs->nco_rx);
    pnsync_crcf_destroy(_fs->fsync);
    free(_fs->demod);
#ifdef DEBUG
    unsigned int i;
    float * r;
    float complex * rc;
    FILE* fid = fopen(DEBUG_FILENAME,"w");
    fprintf(fid,"%% %s: auto-generated file", DEBUG_FILENAME);
    fprintf(fid,"\n\n");
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n\n");

    // write agc_rssi
    fprintf(fid,"agc_rssi = zeros(1,%u);\n", DEBUG_BUFFER_LEN);
    fwindow_read(_fs->debug_agc_rssi, &r);
    for (i=0; i<DEBUG_BUFFER_LEN; i++)
        fprintf(fid,"agc_rssi(%4u) = %12.4e;\n", i+1, r[i]);
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(20*log10(agc_rssi))\n");
    fprintf(fid,"ylabel('RSSI [dB]');\n");

    // write rxy
    fprintf(fid,"rxy = zeros(1,%u);\n", DEBUG_BUFFER_LEN);
    cfwindow_read(_fs->debug_rxy, &rc);
    for (i=0; i<DEBUG_BUFFER_LEN; i++)
        fprintf(fid,"rxy(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(abs(rxy))\n");
    fprintf(fid,"ylabel('|r_{xy}|');\n");

    // write nco_rx_out
    fprintf(fid,"nco_rx_out = zeros(1,%u);\n", DEBUG_BUFFER_LEN);
    cfwindow_read(_fs->debug_nco_rx_out, &rc);
    for (i=0; i<DEBUG_BUFFER_LEN; i++)
        fprintf(fid,"nco_rx_out(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(nco_rx_out,'x')\n");
    fprintf(fid,"ylabel('I');\n");
    fprintf(fid,"ylabel('Q');\n");
    fprintf(fid,"axis square;\n");

    fprintf(fid,"\n\n");
    fclose(fid);

    printf("framesync64/debug: results written to %s\n", DEBUG_FILENAME);

    // clean up debug windows
    fwindow_destroy(_fs->debug_agc_rssi);
    cfwindow_destroy(_fs->debug_rxy);
    cfwindow_destroy(_fs->debug_nco_rx_out);
#endif
    free(_fs);
}

void framesync64_print(framesync64 _fs)
{
    printf("framesync64:\n");
}

void framesync64_execute(framesync64 _fs, float complex *_x, unsigned int _n)
{
    unsigned int i, j, nw;
    float complex agc_rx_out;
    float complex mfdecim_out[4];
    float complex nco_rx_out;
    float phase_error;
    float complex rxy;
    unsigned int demod_sym;
    unsigned int n = _fs->num_symbols_collected;

    for (i=0; i<_n; i++) {
        // agc
        agc_execute(_fs->agc_rx, _x[i], &agc_rx_out);
#ifdef DEBUG
        fwindow_push(_fs->debug_agc_rssi, agc_get_signal_level(_fs->agc_rx));
#endif
        //continue;

        // override agc
        agc_rx_out = _x[i];

        // symbol synchronizer
        symsync_crcf_execute(_fs->mfdecim, &agc_rx_out, 1, mfdecim_out, &nw);

        for (j=0; j<nw; j++) {
            // mix down, demodulate, run PLL
            nco_mix_down(_fs->nco_rx, mfdecim_out[j], &nco_rx_out);
            demodulate(_fs->demod, nco_rx_out, &demod_sym);
            get_demodulator_phase_error(_fs->demod, &phase_error);
            pll_step(_fs->pll_rx, _fs->nco_rx, phase_error);
            nco_step(_fs->nco_rx);
#ifdef DEBUG
            cfwindow_push(_fs->debug_nco_rx_out, nco_rx_out);
#endif

            //
            switch (_fs->state) {
            case FRAMESYNC64_STATE_SEEKPN:
                //
                rxy = pnsync_crcf_correlate(_fs->fsync, nco_rx_out);
                rxy *= cexpf(-M_PI/4*_Complex_I);
#ifdef DEBUG
                cfwindow_push(_fs->debug_rxy, rxy);
#endif
                if (cabsf(rxy) > 0.6f) {
                    printf("|rxy| = %8.4f, angle: %8.4f\n",cabsf(rxy),cargf(rxy));
                    // close bandwidth
                    framesync64_close_bandwidth(_fs);
                    //nco_adjust_phase(_fs->nco_rx, -cargf(rxy));
                    _fs->state = FRAMESYNC64_STATE_RXHEADER;
                }
                break;
            case FRAMESYNC64_STATE_RXHEADER:
                _fs->header_sym[n] = (unsigned char) demod_sym;
                n++;
                if (n==256) {
                    n = 0;
                    _fs->state = FRAMESYNC64_STATE_RXPAYLOAD;
                    framesync64_decode_header(_fs);
                }
                break;
            case FRAMESYNC64_STATE_RXPAYLOAD:
                _fs->payload_sym[n] = (unsigned char) demod_sym;
                n++;
                if (n==512) {
                    n = 0;
                    _fs->state = FRAMESYNC64_STATE_RESET;
                    framesync64_decode_payload(_fs);
                }
                break;
            case FRAMESYNC64_STATE_RESET:
                // open bandwidth
                framesync64_open_bandwidth(_fs);
                _fs->state = FRAMESYNC64_STATE_SEEKPN;
                _fs->num_symbols_collected = 0;
                break;
            default:;
            }
        }
    }
}

// 
// internal
//

void framesync64_open_bandwidth(framesync64 _fs)
{
    agc_set_bandwidth(_fs->agc_rx, FRAMESYNC64_AGC_BW_1);
    symsync_crcf_set_lf_bw(_fs->mfdecim, FRAMESYNC64_SYMSYNC_BW_1);
    pll_set_bandwidth(_fs->pll_rx, FRAMESYNC64_PLL_BW_1);
}

void framesync64_close_bandwidth(framesync64 _fs)
{
    agc_set_bandwidth(_fs->agc_rx, FRAMESYNC64_AGC_BW_0);
    symsync_crcf_set_lf_bw(_fs->mfdecim, FRAMESYNC64_SYMSYNC_BW_0);
    pll_set_bandwidth(_fs->pll_rx, FRAMESYNC64_PLL_BW_0);
}

void framesync64_decode_header(framesync64 _fs)
{
    unsigned int i;
    for (i=0; i<64; i++)
        framesync64_syms_to_byte(_fs->header_sym+(4*i), _fs->header_enc+i);

#ifdef DEBUG
    printf("header ENCODED (rx):\n");
    for (i=0; i<64; i++) {
        printf("%2x ", _fs->header_enc[i]);
        if (!((i+1)%16)) printf("\n");
    }
    printf("\n");
#endif

    // decode header
    fec_decode(_fs->dec, 32, _fs->header_enc, _fs->header);

    // unscramble header data
    unscramble_data(_fs->header, 32);

#ifdef DEBUG
    printf("header (rx):\n");
    for (i=0; i<32; i++) {
        printf("%2x ", _fs->header[i]);
        if (!((i+1)%8)) printf("\n");
    }
    printf("\n");
#endif

    // strip off crc32
    unsigned int header_key=0;
    header_key |= ( _fs->header[28] << 24 );
    header_key |= ( _fs->header[29] << 16 );
    header_key |= ( _fs->header[30] <<  8 );
    header_key |= ( _fs->header[31]       );
    _fs->header_key = header_key;
    printf("rx: header_key:  0x%8x\n", header_key);

    // strip off crc32
    unsigned int payload_key=0;
    payload_key |= ( _fs->header[0] << 24 );
    payload_key |= ( _fs->header[1] << 16 );
    payload_key |= ( _fs->header[2] <<  8 );
    payload_key |= ( _fs->header[3]       );
    _fs->payload_key = payload_key;
    printf("rx: payload_key: 0x%8x\n", payload_key);

    // validate crc
    if (crc32_validate_message(_fs->header,28,_fs->header_key))
        printf("header crc:  valid\n");
    else
        printf("header crc:  INVALID\n");
}

void framesync64_decode_payload(framesync64 _fs)
{
    unsigned int i;
    for (i=0; i<128; i++)
        framesync64_syms_to_byte(&(_fs->payload_sym[4*i]), &(_fs->payload_enc[i]));

    // decode payload
    fec_decode(_fs->dec, 64, _fs->payload_enc, _fs->payload);

    // unscramble payload data
    unscramble_data(_fs->payload, 64);

    // validate crc
    if (crc32_validate_message(_fs->payload,64,_fs->payload_key))
        printf("payload crc: valid\n");
    else
        printf("payload crc: INVALID\n");

#ifdef DEBUG
    printf("payload (rx):\n");
    for (i=0; i<64; i++) {
        printf("%2x ", _fs->payload[i]);
        if (!((i+1)%8)) printf("\n");
    }
    printf("\n");
#endif

    // invoke callback method
}

void framesync64_syms_to_byte(unsigned char * _syms, unsigned char * _byte)
{
    unsigned char b=0;
    b |= (_syms[0] << 6) & 0xc0;
    b |= (_syms[1] << 4) & 0x30;
    b |= (_syms[2] << 2) & 0x0c;
    b |= (_syms[3]     ) & 0x03;
    *_byte = b;
}

