// file: doc/tutorials/ofdmflexframe_tutorial.c
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
    modulation_scheme ms = LIQUID_MODEM_QPSK;   // payload modulation scheme
    unsigned int bps = 2;                       // payload modulation depth
    fec_scheme fec0  = LIQUID_FEC_NONE;         // inner FEC scheme
    fec_scheme fec1  = LIQUID_FEC_HAMMING128;   // outer FEC scheme
    crc_scheme check = LIQUID_CRC_32;           // data validity check

    // allocate memory for header, payload, sample buffer
    float complex buffer[M + cp_len];           // time-domain buffer
    unsigned char header[8];                    // header
    unsigned char payload[payload_len];         // payload

    // create frame generator
    ofdmflexframegenprops_s fgprops;
    ofdmflexframegenprops_init_default(&fgprops);
    fgprops.payload_len     = payload_len;
    fgprops.check           = check;
    fgprops.fec0            = fec0;
    fgprops.fec1            = fec1;
    fgprops.mod_scheme      = ms;
    fgprops.mod_bps         = bps;
    ofdmflexframegen fg = ofdmflexframegen_create(M, cp_len, NULL, &fgprops);
    ofdmflexframegen_print(fg);

    // create frame synchronizer
    ofdmflexframesync fs = ofdmflexframesync_create(M, cp_len, NULL, mycallback, NULL);

    unsigned int i;

    // initialize header/payload and assemble frame
    for (i=0; i<8; i++)           header[i]  = i      & 0xff;
    for (i=0; i<payload_len; i++) payload[i] = rand() & 0xff;
    ofdmflexframegen_assemble(fg, header, payload);

    // generate frame and synchronize
    int last_symbol=0;
    unsigned int num_written;
    while (!last_symbol) {
        // generate symbol (write samples to buffer)
        last_symbol = ofdmflexframegen_writesymbol(fg, buffer, &num_written);

        // receive symbol (read samples from buffer)
        ofdmflexframesync_execute(fs, buffer, num_written);
    }

    // destroy objects and return
    ofdmflexframegen_destroy(fg);
    ofdmflexframesync_destroy(fs);
    printf("done.\n");
    return 0;
}
