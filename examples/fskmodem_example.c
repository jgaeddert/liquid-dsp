const char __docstr__[] =
"This example demonstrates the M-ary frequency-shift keying"
" (MFSK) modem in liquid. A message signal is modulated and the"
" resulting signal is recovered using a demodulator object.";

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
    liquid_argparse_add(char*, filename, "fskmodem_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(unsigned, m,                3, 'm', "number of bits/symbol", NULL);
    liquid_argparse_add(unsigned, k,                0, 'k', "filter samples/symbol ('0' converts to 2**m)", NULL);
    liquid_argparse_add(unsigned, num_symbols,   8000, 'n', "number of symbols", NULL);
    liquid_argparse_add(float,    SNRdB,        20.0f, 'S', "signal-to-noise ratio", NULL);
    liquid_argparse_add(float,    bandwidth,     0.2f, 'w', "relative bandwidth", NULL);
    liquid_argparse_add(unsigned, nfft,          1200, 'N', "FFT size", NULL);
    liquid_argparse_parse(argc,argv);

    // derived values
    if (k == 0)
        k = 2 << m; // set samples per symbol if not otherwise specified
    unsigned int M    = 1 << m;
    float        nstd = powf(10.0f, -SNRdB/20.0f);

    // validate input
    if (k < M)
        return liquid_error(LIQUID_EICONFIG,"samples/symbol must be at least modulation size (M=%u)", M);
    if (k > 2048)
        return liquid_error(LIQUID_EICONFIG,"samples/symbol exceeds maximum (2048)");
    if (M > 1024)
        return liquid_error(LIQUID_EICONFIG,"modulation size exceeds maximum (1024)");
    if (bandwidth <= 0.0f || bandwidth >= 0.5f)
        return liquid_error(LIQUID_EICONFIG,"bandwidth must be in (0,0.5)");

    // create modulator/demodulator pair
    fskmod mod = fskmod_create(m,k,bandwidth);
    fskdem dem = fskdem_create(m,k,bandwidth);
    fskdem_print(dem);

    // 
    LIQUID_VLA(liquid_float_complex, buf_tx, k);    // transmit buffer
    LIQUID_VLA(liquid_float_complex, buf_rx, k);    // transmit buffer
    
    // spectral periodogram
    spgramcf periodogram = spgramcf_create_default(nfft);

    // modulate, demodulate, count errors
    unsigned int num_symbol_errors = 0;
    unsigned int i, j;
    for (i=0; i<num_symbols; i++) {
        // generate random symbol
        unsigned int sym_in = rand() % M;

        // modulate
        fskmod_modulate(mod, sym_in, buf_tx);

        // add noise
        for (j=0; j<k; j++)
            buf_rx[j] = buf_tx[j] + nstd*(randnf() + _Complex_I*randnf())*M_SQRT1_2;

        // demodulate
        unsigned int sym_out = fskdem_demodulate(dem, buf_rx);

        // count errors
        num_symbol_errors += (sym_in == sym_out) ? 0 : 1;

        // estimate power spectral density
        spgramcf_write(periodogram, buf_rx, k);
    }

    // destroy modulator/demodulator pair
    fskmod_destroy(mod);
    fskdem_destroy(dem);

    printf("symbol errors: %u / %u\n", num_symbol_errors, num_symbols);

    // compute power spectral density of received signal
    LIQUID_VLA(float, psd, nfft);
    spgramcf_get_psd(periodogram, psd);
    spgramcf_destroy(periodogram);

    // 
    // export results
    //
    
    FILE * fid = fopen(filename,"w");
    fprintf(fid,"%% %s : auto-generated file\n", filename);
    fprintf(fid,"clear all\n");
    fprintf(fid,"close all\n");
    fprintf(fid,"k = %u;\n", k);
    fprintf(fid,"M = %u;\n", M);
    fprintf(fid,"num_symbols = %u;\n", num_symbols);
    fprintf(fid,"nfft        = %u;\n", nfft);

    // save power spectral density
    fprintf(fid,"psd = zeros(1,nfft);\n");
    for (i=0; i<nfft; i++)
        fprintf(fid,"psd(%4u) = %12.8f;\n", i+1, psd[i]);

    // plot PSD
    fprintf(fid,"figure('Color','white');\n");
    fprintf(fid,"f = [0:(nfft-1)]/nfft - 0.5;\n");
    fprintf(fid,"plot(f,psd,'LineWidth',1.5,'Color',[0.5 0 0]);\n");
    fprintf(fid,"axis([-0.5 0.5 -40 20]);\n");
    fprintf(fid,"xlabel('Normalized Frequency [f/F_s]');\n");
    fprintf(fid,"ylabel('PSD [dB]');\n");
    fprintf(fid,"grid on;\n");

    fclose(fid);
    printf("results written to '%s'\n", filename);

    return 0;
}
