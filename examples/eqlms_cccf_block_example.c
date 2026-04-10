const char __docstr__[] =
"This example tests the least mean-squares (LMS) equalizer (EQ) on a"
" signal with an unknown modulation and carrier frequency offset."
" Equalization is performed blind on a block of samples and the resulting"
" constellation is output to a file for plotting.";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#ifndef _MSC_VER
#include <complex.h>
#endif
#include <time.h>

#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char*argv[])
{
    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*, filename, "eqlms_cccf_block_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(unsigned, num_samples, 2400,   'n', "number of symbols to observe", NULL);
    liquid_argparse_add(float,    SNRdB,       30.0f,  's', "signal-to-noise ratio [dB]", NULL);
    liquid_argparse_add(float,    fc,          0.002f, 'f', "carrier offset", NULL);
    liquid_argparse_add(unsigned, hc_len,      5,      'c', "channel filter length", NULL);
    liquid_argparse_add(unsigned, k,           2,      'k', "matched filter samples/symbol", NULL);
    liquid_argparse_add(unsigned, m,           3,      'm', "matched filter delay (symbols)", NULL);
    liquid_argparse_add(float,    beta,        0.3f,   'b', "matched filter excess bandwidth factor", NULL);
    liquid_argparse_add(unsigned, p,           3,      'p', "equalizer length (symbols, hp_len = 2*k*p+1)", NULL);
    liquid_argparse_add(float,    mu,          0.08f,  'u', "equalizer learning rate", NULL);
    liquid_argparse_add(char *,   mod_scheme,  "qpsk", 'M', "modulation scheme", NULL);
    liquid_argparse_parse(argc,argv);

    // modulation type/depth
    modulation_scheme ms = (modulation_scheme)liquid_getopt_str2mod(mod_scheme);

    // validate input
    if (num_samples == 0)
        return liquid_error(LIQUID_EICONFIG,"number of samples must be greater than zero");
    if (hc_len == 0)
        return liquid_error(LIQUID_EICONFIG,"channel must have at least 1 tap");
    if (k < 2)
        return liquid_error(LIQUID_EICONFIG,"samples/symbol must be at least 2");
    if (m == 0)
        return liquid_error(LIQUID_EICONFIG,"filter semi-length must be at least 1 symbol");
    if (beta < 0.0f || beta > 1.0f)
        return liquid_error(LIQUID_EICONFIG,"filter excess bandwidth must be in [0,1]");
    if (p == 0)
        return liquid_error(LIQUID_EICONFIG,"equalizer semi-length must be at least 1 symbol");
    if (mu < 0.0f || mu > 1.0f)
        return liquid_error(LIQUID_EICONFIG,"equalizer learning rate must be in [0,1]");
    if (ms == LIQUID_MODEM_UNKNOWN)
        return liquid_error(LIQUID_EICONFIG,"unknown modulation scheme '%s'", mod_scheme);

    // derived/fixed values
    unsigned int    i;
    unsigned int    buf_len = 37;
    LIQUID_VLA(liquid_float_complex, buf_input, buf_len);
    LIQUID_VLA(liquid_float_complex, buf_channel, buf_len);
    LIQUID_VLA(liquid_float_complex, buf_output, buf_len);

    // generate input sequence using symbol stream generator
    symstreamcf gen = symstreamcf_create_linear(LIQUID_FIRFILT_ARKAISER,k,m,beta,ms);

    // create multi-path channel filter
    LIQUID_VLA(liquid_float_complex, hc, hc_len);
    for (i=0; i<hc_len; i++)
        hc[i] = (i==0) ? 0.5f : (randnf() + _Complex_I*randnf())*0.2f;
    firfilt_cccf channel = firfilt_cccf_create(hc, hc_len);

    // create equalizer
    eqlms_cccf eq = eqlms_cccf_create_rnyquist(LIQUID_FIRFILT_RRC, k, p, beta, 0.0f);
    eqlms_cccf_set_bw(eq, mu);

    FILE * fid = fopen(filename,"w");
    fprintf(fid,"%% %s : auto-generated file\n\n", filename);
    fprintf(fid,"clear all\n");
    fprintf(fid,"close all\n");
    fprintf(fid,"x = [];\n");
    fprintf(fid,"y = [];\n");
    fprintf(fid,"z = [];\n");

    unsigned int num_samples_total = 0;
    while (num_samples_total < num_samples)
    {
        // generate block of input samples
        symstreamcf_write_samples(gen, buf_input, buf_len);

        // apply channel to input signal
        firfilt_cccf_execute_block(channel, buf_input, buf_len, buf_channel);

        // run equalizer
        eqlms_cccf_execute_block(eq, k, buf_channel, buf_len, buf_output);
        
        // save results to output file
        for (i=0; i<buf_len; i++) {
            fprintf(fid,"x(end+1) = %12.4e + %12.4ei;\n", crealf(buf_input  [i]), cimagf(buf_input  [i]));
            fprintf(fid,"y(end+1) = %12.4e + %12.4ei;\n", crealf(buf_channel[i]), cimagf(buf_channel[i]));
            fprintf(fid,"z(end+1) = %12.4e + %12.4ei;\n", crealf(buf_output [i]), cimagf(buf_output [i]));
        }

        // increment number of samples
        num_samples_total += buf_len;
    }

    // destroy objects
    symstreamcf_destroy(gen);
    firfilt_cccf_destroy(channel);
    eqlms_cccf_destroy(eq);

    fprintf(fid,"k = %u;\n", k);
    fprintf(fid,"m = %u;\n", m);
    fprintf(fid,"n = length(x);\n");

    fprintf(fid,"figure('Color','white','position',[500 500 1200 500]);\n");
    // plot constellation
    fprintf(fid,"subplot(2,8,[5 6 7 8 13 14 15 16]),\n");
    fprintf(fid,"  s = 1:k:n;\n");
    fprintf(fid,"  s0 = round(length(s)/2);\n");
    fprintf(fid,"  syms_rx_0 = z(s(1:s0));\n");
    fprintf(fid,"  syms_rx_1 = z(s(s0:end));\n");
    fprintf(fid,"  plot(real(syms_rx_0),imag(syms_rx_0),'x','Color',[1 1 1]*0.7,...\n");
    fprintf(fid,"       real(syms_rx_1),imag(syms_rx_1),'x','Color',[1 1 1]*0.0);\n");
    fprintf(fid,"  xlabel('In-Phase');\n");
    fprintf(fid,"  ylabel('Quadrature');\n");
    fprintf(fid,"  axis([-1 1 -1 1]*1.5);\n");
    fprintf(fid,"  axis square;\n");
    fprintf(fid,"  grid on;\n");
    // plot time response
    fprintf(fid,"t = 0:(n-1);\n");
    fprintf(fid,"subplot(2,8,1:4),\n");
    fprintf(fid,"  plot(t,   real(z),   '-','Color',[1 1 1]*0.7,...\n");
    fprintf(fid,"       t(s),real(z(s)),'s','Color',[0 0.2 0.5],'MarkerSize',3);\n");
    fprintf(fid,"  axis([0 n -1.5 1.5]);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  ylabel('real');\n");
    fprintf(fid,"subplot(2,8,9:12),\n");
    fprintf(fid,"  plot(t,   imag(z),   '-','Color',[1 1 1]*0.7,...\n");
    fprintf(fid,"       t(s),imag(z(s)),'s','Color',[0 0.5 0.2],'MarkerSize',3);\n");
    fprintf(fid,"  axis([0 n -1.5 1.5]);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  ylabel('imag');\n");

    fclose(fid);
    printf("results written to '%s'\n", filename);

    return 0;
}
