// file: doc/tutorials/ofdmflexframe_advanced_tutorial.c
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#include <liquid/liquid.h>

// callback function
int mycallback(unsigned char *  _header,
               int              _header_valid,
               unsigned char *  _payload,
               unsigned int     _payload_len,
               int              _payload_valid,
               framesyncstats_s _stats,
               void *           _userdata)
{
    printf("***** callback invoked!\n");
    printf("  header (%s)\n",  _header_valid  ? "valid" : "INVALID");
    printf("  payload (%s)\n", _payload_valid ? "valid" : "INVALID");
    return 0;
}

int main() {
    // options
    unsigned int M = 64;                        // number of subcarriers
    unsigned int cp_len = 16;                   // cyclic prefix length
    unsigned int payload_len = 120;             // length of payload (bytes)
    modulation_scheme ms = LIQUID_MODEM_PSK8;   // payload modulation scheme
    fec_scheme fec0  = LIQUID_FEC_NONE;         // inner FEC scheme
    fec_scheme fec1  = LIQUID_FEC_HAMMING128;   // outer FEC scheme
    crc_scheme check = LIQUID_CRC_32;           // data validity check
    float dphi  = 0.001f;                       // carrier frequency offset
    float SNRdB = 20.0f;                        // signal-to-noise ratio [dB]

    // allocate memory for header, payload, sample buffer
    float complex buffer[M + cp_len];           // time-domain buffer
    unsigned char header[8];                    // header
    unsigned char payload[payload_len];         // payload

    // create frame generator with default properties
    ofdmflexframegen fg = ofdmflexframegen_create(M, cp_len, NULL, NULL);

    // create frame synchronizer
    ofdmflexframesync fs = ofdmflexframesync_create(M, cp_len, NULL, mycallback, NULL);

    unsigned int i;
    
    // re-configure frame generator with different properties
    ofdmflexframegenprops_s fgprops;
    ofdmflexframegen_getprops(fg,&fgprops); // query the current properties
    fgprops.check           = check;        // set the error-detection scheme
    fgprops.fec0            = fec0;         // set the inner FEC scheme
    fgprops.fec1            = fec1;         // set the outer FEC scheme
    fgprops.mod_scheme      = ms;           // set the modulation scheme
    ofdmflexframegen_setprops(fg,&fgprops); // reconfigure the frame generator

    // initialize header/payload and assemble frame
    for (i=0; i<8; i++)           header[i]  = i      & 0xff;
    for (i=0; i<payload_len; i++) payload[i] = rand() & 0xff;
    ofdmflexframegen_assemble(fg, header, payload, payload_len);
    ofdmflexframegen_print(fg);

    // channel parameters
    float nstd = powf(10.0f, -SNRdB/20.0f); // noise standard deviation
    float phi = 0.0f;                       // channel phase

    // generate frame and synchronize
    int last_symbol=0;
    unsigned int num_written;
    while (!last_symbol) {
        // generate symbol (write samples to buffer)
        last_symbol = ofdmflexframegen_writesymbol(fg, buffer, &num_written);

        // channel impairments
        for (i=0; i<num_written; i++) {
            buffer[i] *= cexpf(_Complex_I*phi); // apply carrier offset
            phi += dphi;                        // update carrier phase
            cawgn(&buffer[i], nstd);            // add noise
        }

        // receive symbol (read samples from buffer)
        ofdmflexframesync_execute(fs, buffer, num_written);
    }

    // destroy objects and return
    ofdmflexframegen_destroy(fg);
    ofdmflexframesync_destroy(fs);
    printf("done.\n");
    return 0;
}
