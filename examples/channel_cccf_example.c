const char __docstr__[] =
"This example demonstrates how the channel_cccf object can be used to"
" emulate a multi-path fading, log-normal shadowing, and AWGN channel."
" A stream of modulated and interpolated symbols are generated using the"
" symstream object. The resulting samples are passed through a channel"
" to add various impairments. The symtrack object recovers timing,"
" carrier, and other information imparted by the channel and returns"
" data symbols ready for demodulation.";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <assert.h>

#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char*argv[])
{
    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*, filename, "channel_cccf_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(unsigned, k,           2,      'k', "samples per symbol", NULL);
    liquid_argparse_add(unsigned, m,           7,      'm', "filter delay (symbols)", NULL);
    liquid_argparse_add(float,    beta,        0.25f,  'b', "filter excess bandwidth factor", NULL);
    liquid_argparse_add(unsigned, num_symbols, 4000,   'n', "number of data symbols", NULL);
    liquid_argparse_add(unsigned, hc_len,      5,      'H', "channel filter length", NULL);
    liquid_argparse_add(float,    noise_floor, -60.0f, '0', "noise floor [dB]", NULL);
    liquid_argparse_add(float,    SNRdB,       30.0f,  'S', "signal-to-noise ratio [dB]", NULL);
    liquid_argparse_add(float,    bandwidth,    0.25f, 'w', "loop filter bandwidth", NULL);
    liquid_argparse_add(float,    dphi,         0.00f, 'F', "carrier frequency offset [radians/sample]", NULL);
    liquid_argparse_add(float,    phi,          2.1f,  'P', "carrier phase offset [radians]", NULL);
    liquid_argparse_add(char*,    mod_scheme, "qpsk",  'M', "modulation scheme", liquid_argparse_modem);
    liquid_argparse_parse(argc,argv);

    // validate input
    modulation_scheme ms = (modulation_scheme)liquid_getopt_str2mod(mod_scheme);
    if (k < 2)
        return liquid_error(LIQUID_EICONFIG,"k (samples/symbol) must be greater than 1");
    if (m < 1)
        return liquid_error(LIQUID_EICONFIG,"m (filter delay) must be greater than 0");
    if (beta <= 0.0f || beta > 1.0f)
        return liquid_error(LIQUID_EICONFIG,"beta (excess bandwidth factor) must be in (0,1]");
    if (num_symbols == 0)
        return liquid_error(LIQUID_EICONFIG,"number of symbols must be greater than 0");
    if (bandwidth <= 0.0f)
        return liquid_error(LIQUID_EICONFIG,"timing PLL bandwidth must be greater than 0");

    unsigned int i;

    // derived/fixed values
    unsigned int num_samples = num_symbols*k;

    LIQUID_VLA(liquid_float_complex, x, num_samples);    // input (interpolated) samples
    LIQUID_VLA(liquid_float_complex, y, num_samples);    // channel output samples
    LIQUID_VLA(liquid_float_complex, sym_out, num_symbols + 64);// synchronized symbols

    // 
    // generate input sequence using symbol stream generator
    //
    symstreamcf gen = symstreamcf_create_linear(LIQUID_FIRFILT_ARKAISER,k,m,beta,ms);
    symstreamcf_write_samples(gen, x, num_samples);
    symstreamcf_destroy(gen);

    // create channel
    channel_cccf channel = channel_cccf_create();

    // add channel impairments
    channel_cccf_add_awgn          (channel, noise_floor, SNRdB);
    channel_cccf_add_carrier_offset(channel, dphi, phi);
    channel_cccf_add_multipath     (channel, NULL, hc_len);
    channel_cccf_add_shadowing     (channel, 1.0f, 0.1f);

    // print channel internals
    channel_cccf_print(channel);

    // apply channel to input signal
    channel_cccf_execute_block(channel, x, num_samples, y);

    // destroy channel
    channel_cccf_destroy(channel);

    // 
    // create and run symbol synchronizer
    //
    symtrack_cccf symtrack = symtrack_cccf_create(LIQUID_FIRFILT_RRC,k,m,beta,ms);
    
    // set tracking bandwidth
    symtrack_cccf_set_bandwidth(symtrack,bandwidth);

    unsigned int num_symbols_sync = 0;
    symtrack_cccf_execute_block(symtrack, y, num_samples, sym_out, &num_symbols_sync);
    symtrack_cccf_destroy(symtrack);

    // print results
    printf("symbols in  : %u\n", num_symbols);
    printf("symbols out : %u\n", num_symbols_sync);

    // estimate spectrum
    unsigned int nfft = 1200;
    LIQUID_VLA(float, psd, nfft);
    spgramcf_estimate_psd(nfft, y, num_samples, psd);

    //
    // export output file
    //
    FILE * fid = fopen(filename,"w");
    fprintf(fid,"%% %s, auto-generated file\n\n", filename);
    fprintf(fid,"close all;\nclear all;\n\n");
    fprintf(fid,"num_symbols=%u;\n",num_symbols_sync);

    for (i=0; i<num_symbols_sync; i++)
        fprintf(fid,"z(%3u) = %12.8f + j*%12.8f;\n", i+1, crealf(sym_out[i]), cimagf(sym_out[i]));

    // power spectral density estimate
    fprintf(fid,"nfft = %u;\n", nfft);
    fprintf(fid,"f=[0:(nfft-1)]/nfft - 0.5;\n");
    fprintf(fid,"psd = zeros(1,nfft);\n");
    for (i=0; i<nfft; i++)
        fprintf(fid,"psd(%3u) = %12.8f;\n", i+1, psd[i]);

    fprintf(fid,"iz0 = 1:round(length(z)*0.5);\n");
    fprintf(fid,"iz1 = round(length(z)*0.5):length(z);\n");
    fprintf(fid,"figure('Color','white','position',[500 500 800 800]);\n");
    fprintf(fid,"subplot(2,2,1);\n");
    fprintf(fid,"plot(real(z(iz0)),imag(z(iz0)),'x','MarkerSize',4);\n");
    fprintf(fid,"  axis square;\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  axis([-1 1 -1 1]*1.8);\n");
    fprintf(fid,"  xlabel('In-phase');\n");
    fprintf(fid,"  ylabel('Quadrature');\n");
    fprintf(fid,"  title('First 50%% of symbols');\n");
    fprintf(fid,"subplot(2,2,2);\n");
    fprintf(fid,"  plot(real(z(iz1)),imag(z(iz1)),'x','MarkerSize',4);\n");
    fprintf(fid,"  axis square;\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  axis([-1 1 -1 1]*1.5);\n");
    fprintf(fid,"  xlabel('In-phase');\n");
    fprintf(fid,"  ylabel('Quadrature');\n");
    fprintf(fid,"  title('Last 50%% of symbols');\n");
    fprintf(fid,"subplot(2,2,3:4);\n");
    fprintf(fid,"  plot(f, psd, 'LineWidth',1.5,'Color',[0 0.5 0.2]);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  pmin = 10*floor(0.1*min(psd - 5));\n");
    fprintf(fid,"  pmax = 10*ceil (0.1*max(psd + 5));\n");
    fprintf(fid,"  axis([-0.5 0.5 pmin pmax]);\n");
    fprintf(fid,"  xlabel('Normalized Frequency [f/F_s]');\n");
    fprintf(fid,"  ylabel('Power Spectral Density [dB]');\n");

    fclose(fid);
    printf("results written to %s.\n", filename);

    // clean it up
    printf("done.\n");
    return 0;
}
