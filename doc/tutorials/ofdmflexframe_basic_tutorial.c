// file: doc/tutorials/ofdmflexframe_basic_tutorial.c
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#include <liquid/liquid.h>

int main() {
    // options
    unsigned int M           = 64;          // number of subcarriers
    unsigned int cp_len      = 16;          // cyclic prefix length
    unsigned int taper_len   = 4;           // taper length
    unsigned int payload_len = 120;         // length of payload (bytes)

    // allocate memory for header, payload, sample buffer
    unsigned int symbol_len = M + cp_len;   // samples per OFDM symbol
    float complex buffer[symbol_len];       // time-domain buffer
    unsigned char header[8];                // header
    unsigned char payload[payload_len];     // payload

    // create frame generator object with default properties
    ofdmflexframegen fg = ofdmflexframegen_create(M, cp_len, taper_len, NULL, NULL);

    unsigned int i;

    // initialize header/payload and assemble frame
    for (i=0; i<8; i++)           header[i]  = i      & 0xff;
    for (i=0; i<payload_len; i++) payload[i] = rand() & 0xff;
    ofdmflexframegen_assemble(fg, header, payload, payload_len);
    ofdmflexframegen_print(fg);

    // generate frame one OFDM symbol at a time
    int last_symbol=0;
    while (!last_symbol) {
        // generate symbol (write samples to buffer)
        last_symbol = ofdmflexframegen_writesymbol(fg, buffer);

        // print status
        printf("ofdmflexframegen wrote %3u samples %s\n",
            symbol_len,
            last_symbol ? "(last symbol)" : "");
    }

    // destroy objects and return
    ofdmflexframegen_destroy(fg);
    printf("done.\n");
    return 0;
}
