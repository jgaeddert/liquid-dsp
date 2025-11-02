char __docstr__[] =
"This example demonstrates how to recover data symbols using the symtrack"
" object. A stream of modulated and interpolated symbols are generated using"
" the symstream object. The resulting samples are passed through a channel"
" to add various impairments. The symtrack object recovers timing, carrier,"
" and other information imparted by the channel and returns data symbols"
" ready for demodulation.";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <assert.h>

#include "liquid.h"
#include "liquid.argparse.h"

int main(int argc, char* argv[])
{
    // define variables and parse command-line arguments
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*, filename, "symtrack_cccf_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(char*,    ftype_str, "arkaiser",'f',"filter type", liquid_argparse_firfilt);
    liquid_argparse_add(char*,    mod_str,   "qpsk",'M', "modulation scheme", liquid_argparse_modem);
    liquid_argparse_add(unsigned, k,             2, 'k', "filter samples per symbol (input)", NULL);
    liquid_argparse_add(unsigned, m,             9, 'm', "filter semi-length", NULL);
    liquid_argparse_add(float,    beta,        0.3, 'b', "filter excess bandwidth factor", NULL);
    liquid_argparse_add(unsigned, num_symbols,4000, 'N', "number of data symbols to simulate", NULL);
    liquid_argparse_add(unsigned, hc_len,        4, 'H', "channel filter length", NULL);
    liquid_argparse_add(float,    noise_floor, -80, '0', "noise floor [dB]", NULL);
    liquid_argparse_add(float,    SNRdB,        30, 's', "signal-to-noise ratio [dB]", NULL);
    liquid_argparse_add(float,    bandwidth,  0.10, 'w', "loop filter bandwidth", NULL);
    liquid_argparse_add(float,    dphi,      0.010, 'F', "frequency offset [radians/sample]", NULL);
    liquid_argparse_add(float,    phi,       0.800, 'P', "phase offset [radians]", NULL);
    liquid_argparse_add(unsigned, nfft,       2400, 'n', "FFT size", NULL);
    liquid_argparse_add(unsigned, num_samples,80000,'S',"number of samples to simulate", NULL);
    liquid_argparse_parse(argc,argv);

    // set filter types
    int ftype_tx = liquid_getopt_str2firfilt(ftype_str);
    int ftype_rx = liquid_getopt_str2firfilt(ftype_str);
    // ensure appropriate GMSK filter pairs are used if either tx or rx is specified
    if (strcmp(ftype_str,"gmsktx")==0)
        ftype_rx = LIQUID_FIRFILT_GMSKRX;
    if (strcmp(ftype_str,"gmskrx")==0)
        ftype_tx = LIQUID_FIRFILT_GMSKTX;

    // validate input
    if (k < 2)
        return liquid_error(LIQUID_EICONFIG,"k (samples/symbol) must be at least 2");
    if (m < 1)
        return liquid_error(LIQUID_EICONFIG,"m (filter delay) must be greater than 0");
    if (beta <= 0.0f || beta > 1.0f)
        return liquid_error(LIQUID_EICONFIG,"beta (excess bandwidth factor) must be in (0,1]");
    if (bandwidth <= 0.0f)
        return liquid_error(LIQUID_EICONFIG,"timing PLL bandwidth must be greater than 0");
    if (num_symbols == 0)
        return liquid_error(LIQUID_EICONFIG,"number of symbols must be greater than 0");

    // buffers
    unsigned int    buf_len = 800;      // buffer size
    float complex   x   [buf_len];      // original signal
    float complex   y   [buf_len];      // channel output
    float complex   syms[buf_len];      // recovered symbols
    // window for saving last few symbols
    windowcf sym_buf = windowcf_create(buf_len);

    // create stream generator
    int         ftype = liquid_getopt_str2firfilt(ftype_str);
    int         ms    = liquid_getopt_str2mod(mod_str);
    symstreamcf gen   = symstreamcf_create_linear(ftype,k,m,beta,ms);

    // create channel emulator and add impairments
    channel_cccf channel = channel_cccf_create();
    channel_cccf_add_awgn          (channel, noise_floor, SNRdB);
    channel_cccf_add_carrier_offset(channel, dphi, phi);
    channel_cccf_add_multipath     (channel, NULL, hc_len);

    // create symbol tracking synchronizer
    symtrack_cccf symtrack = symtrack_cccf_create(ftype,k,m,beta,ms);
    symtrack_cccf_set_bandwidth(symtrack,bandwidth);
    //symtrack_cccf_set_eq_off(symtrack); // disable equalization
    symtrack_cccf_print(symtrack);

    // create spectral periodogram for estimating spectrum
    spgramcf periodogram = spgramcf_create_default(nfft);

    unsigned int total_samples = 0;
    unsigned int total_symbols = 0;
    while (total_samples < num_samples)
    {
        // write samples to buffer
        symstreamcf_write_samples(gen, x, buf_len);

        // apply channel
        channel_cccf_execute_block(channel, x, buf_len, y);

        // push resulting sample through periodogram
        spgramcf_write(periodogram, y, buf_len);

        // run resulting stream through synchronizer
        unsigned int num_symbols_sync;
        symtrack_cccf_execute_block(symtrack, y, buf_len, syms, &num_symbols_sync);
        total_symbols += num_symbols_sync;

        // write resulting symbols to window buffer for plotting
        windowcf_write(sym_buf, syms, num_symbols_sync);

        // accumulated samples
        total_samples += buf_len;
    }
    printf("total samples: %u\n", total_samples);
    printf("total symbols: %u\n", total_symbols);

    // write accumulated power spectral density estimate
    float psd[nfft];
    spgramcf_get_psd(periodogram, psd);

    //
    // export output file
    //

    FILE * fid = fopen(filename,"w");
    fprintf(fid,"%% %s, auto-generated file\n\n", filename);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");

    // read buffer and write last symbols to file
    float complex * rc;
    windowcf_read(sym_buf, &rc);
    fprintf(fid,"syms = zeros(1,%u);\n", buf_len);
    unsigned int i;
    for (i=0; i<buf_len; i++)
        fprintf(fid,"syms(%3u) = %12.8f + j*%12.8f;\n", i+1, crealf(rc[i]), cimagf(rc[i]));

    // power spectral density estimate
    fprintf(fid,"nfft = %u;\n", nfft);
    fprintf(fid,"f=[0:(nfft-1)]/nfft - 0.5;\n");
    fprintf(fid,"psd = zeros(1,nfft);\n");
    for (i=0; i<nfft; i++)
        fprintf(fid,"psd(%3u) = %12.8f;\n", i+1, psd[i]);

    fprintf(fid,"figure('Color','white','position',[500 500 1400 400]);\n");
    fprintf(fid,"subplot(1,3,1);\n");
    fprintf(fid,"plot(real(syms),imag(syms),'x','MarkerSize',4);\n");
    fprintf(fid,"  axis square;\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  axis([-1 1 -1 1]*1.6);\n");
    fprintf(fid,"  xlabel('In-phase');\n");
    fprintf(fid,"  ylabel('Quadrature');\n");
    fprintf(fid,"  title('Last %u symbols');\n", buf_len);
    fprintf(fid,"subplot(1,3,2:3);\n");
    fprintf(fid,"  plot(f, psd, 'LineWidth',1.5,'Color',[0 0.5 0.2]);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  pmin = 10*floor(0.1*min(psd - 5));\n");
    fprintf(fid,"  pmax = 10*ceil (0.1*max(psd + 5));\n");
    fprintf(fid,"  axis([-0.5 0.5 pmin pmax]);\n");
    fprintf(fid,"  xlabel('Normalized Frequency [f/F_s]');\n");
    fprintf(fid,"  ylabel('Power Spectral Density [dB]');\n");

    fclose(fid);
    printf("results written to %s.\n", filename);

    // destroy objects
    symstreamcf_destroy  (gen);
    spgramcf_destroy     (periodogram);
    channel_cccf_destroy (channel);
    symtrack_cccf_destroy(symtrack);
    windowcf_destroy     (sym_buf);

    // clean it up
    printf("done.\n");
    return 0;
}
