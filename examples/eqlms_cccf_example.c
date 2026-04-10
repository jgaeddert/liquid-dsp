const char __docstr__[] =
"Demonstrates least mean-squares (LMS) equalizer (EQ) on a QPSK signal";

#include <stdio.h>
#include <stdlib.h>
#ifndef _MSC_VER
#include <complex.h>
#endif
#include <math.h>
#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"
#define OUTPUT_FILENAME "eqlms_cccf_example.m"

int main(int argc, char* argv[])
{
    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*, filename, "eqlms_cccf_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(unsigned, num_symbols, 800,    'n', "number of symbols to observe", NULL);
    liquid_argparse_add(unsigned, hc_len,      5,      'c', "channel filter length", NULL);
    liquid_argparse_add(unsigned, w_len,       11,     'w', "equalizer filter length", NULL);
    liquid_argparse_add(float,    mu,          0.08f,  'u', "equalizer learning rate", NULL);
    liquid_argparse_add(char *,   mod_scheme,  "qpsk", 'M', "modulation scheme", liquid_argparse_modem);
    liquid_argparse_parse(argc,argv);

    // modulation type/depth
    modulation_scheme ms = (modulation_scheme)liquid_getopt_str2mod(mod_scheme);

    // modem
    modemcf mod = modemcf_create(ms);

    // create channel filter (random coefficients)
    LIQUID_VLA(liquid_float_complex, h, hc_len);
    h[0] = 1.0f;
    unsigned int i;
    for (i=1; i<hc_len; i++)
        h[i] = (randnf() + randnf()*_Complex_I) * 0.1f;
    firfilt_cccf fchannel = firfilt_cccf_create(h,hc_len);

    // create equalizer (default initial coefficients)
    LIQUID_VLA(liquid_float_complex, w, w_len);
    for (i=0; i<w_len; i++) w[i] = i==0 ? 1 : 0;
    eqlms_cccf eq = eqlms_cccf_create(w,w_len);
    eqlms_cccf_set_bw(eq, mu);

    // run equalization
    liquid_float_complex sym_in, sym_channel, sym_out;  // modulated/recovered symbols
    float rmse = 0.0f;
    for (i=0; i<2*num_symbols; i++) {
        // generate modulated input symbol
        unsigned int sym = modemcf_gen_rand_sym(mod);
        modemcf_modulate(mod, sym, &sym_in);

        // apply channel filter (in place)
        firfilt_cccf_push(fchannel, sym_in);
        firfilt_cccf_execute(fchannel, &sym_channel);

        // skip first w_len symbols
        if (i < w_len) continue;

        // update equalizer weights
        eqlms_cccf_push      (eq, sym_channel); // input symbol
        eqlms_cccf_execute   (eq, &sym_out);    // dot product
        //eqlms_cccf_step(eq, sym_in, sym_out);     // ideal update
        eqlms_cccf_step_blind(eq, sym_out);     // blind update

        // accumulate RMS error
        if (i < num_symbols) continue;
        float e = cabsf(sym_out - sym_in);
        rmse += e*e;
    }
    eqlms_cccf_print(eq);
    rmse = 10*log10f( rmse / (float)num_symbols);
    printf("RMSE: %.3f dB\n", rmse);
    eqlms_cccf_copy_coefficients(eq,w);

    // clean up objects
    firfilt_cccf_destroy(fchannel);
    eqlms_cccf_destroy(eq);
    modemcf_destroy(mod);

    // export data to file
    FILE * fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all; close all;\n");
    fprintf(fid,"hc_len=%u; w_len=%u;\n", hc_len, w_len);
    // save channel coefficients
    for (i=0; i<hc_len; i++)
        fprintf(fid,"h(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(h[i]), cimagf(h[i]));
    // save equalizer coefficients
    for (i=0; i<w_len; i++)
        fprintf(fid,"w(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(w[i]), cimagf(w[i]));
    // plot results
    fprintf(fid,"\n\n");
    fprintf(fid,"nfft=1200;\n");
    fprintf(fid,"f=[0:(nfft-1)]/nfft - 0.5;\n");
    fprintf(fid,"H=20*log10(abs(fftshift(fft(h,nfft))));\n");
    fprintf(fid,"W=20*log10(abs(fftshift(fft(w,nfft))));\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(f,H,'-r',f,W,'-b', f,H+W,'-k','LineWidth',2);\n");
    fprintf(fid,"xlabel('Normalied Frequency');\n");
    fprintf(fid,"ylabel('Power Spectral Density [dB]');\n");
    fprintf(fid,"axis([-0.5 0.5 -10 10]);\n");
    fprintf(fid,"legend('channel','equalizer','composite');\n");
    fclose(fid);
    printf("results written to %s.\n",OUTPUT_FILENAME);
    return 0;
}
