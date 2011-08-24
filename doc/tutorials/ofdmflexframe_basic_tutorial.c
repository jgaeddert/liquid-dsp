// file: doc/tutorials/ofdmflexframe_basic_tutorial.c
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#include <liquid/liquid.h>

int main() {
    // options
    unsigned int M = 64;                            // number of subcarriers
    unsigned int cp_len = 16;                       // cyclic prefix length
    unsigned int payload_len = 120;                 // length of payload (bytes)

    // allocate memory for header, payload, sample buffer
    float complex buffer[M + cp_len];               // time-domain buffer
    unsigned char header[8];                        // header
    unsigned char payload[payload_len];             // payload

    // create frame generator properties oject and initialize to default
    ofdmflexframegenprops_s fgprops;                // create properties object
    ofdmflexframegenprops_init_default(&fgprops);   // initialize to default
    fgprops.payload_len = payload_len;              // set payload length

    // create frame generator object
    ofdmflexframegen fg = ofdmflexframegen_create(M, cp_len, NULL, &fgprops);
    ofdmflexframegen_print(fg);

    unsigned int i;

    // initialize header/payload and assemble frame
    for (i=0; i<8; i++)           header[i]  = i      & 0xff;
    for (i=0; i<payload_len; i++) payload[i] = rand() & 0xff;
    ofdmflexframegen_assemble(fg, header, payload);

    // generate frame one OFDM symbol at a time
    int last_symbol=0;
    unsigned int num_written;
    while (!last_symbol) {
        // generate symbol (write samples to buffer)
        last_symbol = ofdmflexframegen_writesymbol(fg, buffer, &num_written);
    }

    // destroy objects and return
    ofdmflexframegen_destroy(fg);
    printf("done.\n");
    return 0;
}
