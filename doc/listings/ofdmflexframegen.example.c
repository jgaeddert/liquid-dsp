// file: doc/listings/ofdmflexframegen.example.c
#include <liquid/liquid.h>

int main() {
    // options
    unsigned int M = 64;                // number of subcarriers
    unsigned int cp_len = 16;           // cyclic prefix length
    unsigned int payload_len = 120;     // length of payload (bytes)

    // buffers
    float complex buffer[M + cp_len];   // time-domain buffer
    unsigned char header[8];            // header data
    unsigned char payload[payload_len]; // payload data
    unsigned char p[M];                 // subcarrier allocation (null/pilot/data)

    // initialize frame generator properties
    ofdmflexframegenprops_s fgprops;
    ofdmflexframegenprops_init_default(&fgprops);
    fgprops.num_symbols_S0  = 3;
    fgprops.check           = LIQUID_CRC_32;
    fgprops.fec0            = LIQUID_FEC_NONE;
    fgprops.fec1            = LIQUID_FEC_HAMMING128;
    fgprops.mod_scheme      = LIQUID_MODEM_QAM;
    fgprops.mod_bps         = 4;

    // initialize subcarrier allocation to default
    ofdmframe_init_default_sctype(M, p);

    // create frame generator
    ofdmflexframegen fg = ofdmflexframegen_create(M, cp_len, p, &fgprops);

    // ... initialize header/payload ...

    // assemble frame
    ofdmflexframegen_assemble(fg, header, payload, payload_len);

    // generate frame
    int last_symbol=0;
    unsigned int num_written;
    while (!last_symbol) {
        // generate each OFDM symbol
        last_symbol = ofdmflexframegen_writesymbol(fg, buffer, &num_written);
    }

    // destroy the frame generator object
    ofdmflexframegen_destroy(fg);
}
