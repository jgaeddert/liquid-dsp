const char __docstr__[] =
"This example demonstrates the M-ary frequency-shift keying"
" (MFSK) modem in liquid by showing the resulting spectral"
" waterfall.";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char*argv[])
{
    // define variables and parse command-line arguments
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*, filename, "fskmodem_waterfall_example", 'o', "output base filename", NULL);
    liquid_argparse_add(unsigned, m,               2, 'm', "number of bits/symbol", NULL);
    liquid_argparse_add(unsigned, num_symbols,   400, 'n', "number of symbols", NULL);
    liquid_argparse_add(float,    SNRdB,       30.0f, 'S', "signal-to-noise ratio", NULL);
    liquid_argparse_add(float,    bandwidth,    0.1f, 'w', "relative bandwidth", NULL);
    liquid_argparse_parse(argc,argv);

    // derived values
    unsigned int M    = 1 << m;     // constellation size
    unsigned int k    = 500 * M;    // samples per symbol (highly over-sampled)
    float        nstd = powf(10.0f, -SNRdB/20.0f);  // noise std. dev.

    // validate input
    if (k < M)
        return liquid_error(LIQUID_EICONFIG,"samples/symbol must be at least modulation size");
    if (k > 2048)
        return liquid_error(LIQUID_EICONFIG,"samples/symbol exceeds maximum (2048)");
    if (M > 1024)
        return liquid_error(LIQUID_EICONFIG,"modulation size (M=%u) exceeds maximum (1024)", M);
    if (bandwidth <= 0.0f || bandwidth >= 0.5f)
        return liquid_error(LIQUID_EICONFIG,"bandwidth must be in (0,0.5)");

    // create spectral waterfall object
    unsigned int nfft  = 1 << liquid_nextpow2(k);
    int          wtype = LIQUID_WINDOW_HAMMING;
    unsigned int wlen  = nfft/2;
    unsigned int delay = nfft/2;
    unsigned int time  =    512;
    spwaterfallcf periodogram = spwaterfallcf_create(nfft,wtype,wlen,delay,time);
    spwaterfallcf_print(periodogram);

    // create modulator/demodulator pair
    fskmod mod = fskmod_create(m,k,bandwidth);

    LIQUID_VLA(liquid_float_complex, buf_tx, k);    // transmit buffer
    LIQUID_VLA(liquid_float_complex, buf_rx, k);    // transmit buffer

    // modulate, demodulate, count errors
    unsigned int i, j;
    for (i=0; i<num_symbols; i++) {
        // generate random symbol
        unsigned int sym_in = rand() % M;

        // modulate
        fskmod_modulate(mod, sym_in, buf_tx);

        // add noise
        for (j=0; j<k; j++)
            buf_rx[j] = buf_tx[j] + nstd*(randnf() + _Complex_I*randnf())*M_SQRT1_2;

        // estimate power spectral density
        spwaterfallcf_write(periodogram, buf_rx, k);
    }

    // export output files
    spwaterfallcf_export(periodogram,filename);

    // destroy objects
    spwaterfallcf_print(periodogram);
    fskmod_destroy(mod);

    return 0;
}
