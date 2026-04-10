const char __docstr__[] = "Demonstrate GMSK modem interface.";

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char*argv[])
{
    // define variables and parse command-line arguments
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*, filename, "gmskmodem_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(unsigned, k,        4,     'k', "filter samples/symbol", NULL);
    liquid_argparse_add(unsigned, m,        3,     'm', "filter delay (symbols)", NULL);
    liquid_argparse_add(float,    BT,       0.3f,  'b', "bandwidth-time product", NULL);
    liquid_argparse_add(unsigned, n,        200,   'n', "number of symbols", NULL);
    liquid_argparse_add(float,    SNRdB,    30.0f, 's', "signal-to-noise ratio [dB]", NULL);
    liquid_argparse_add(float,    dphi,     0.0f,  'F', "carrier frequency offset", NULL);
    liquid_argparse_add(float,    phi,      0.0f,  'P', "carrier phase offset", NULL);
    liquid_argparse_parse(argc,argv);

    // validate input
    if (BT <= 0.0f || BT >= 1.0f)
        return liquid_error(LIQUID_EICONFIG,"bandwidth-time product must be in (0,1)");

    // derived values
    unsigned int num_symbols = n + 2*m;
    unsigned int num_samples = k*num_symbols;
    float nstd = powf(10.0f,-SNRdB/20.0f);  // noise standard deviation

    // create modulator
    gmskmod mod   = gmskmod_create(k, m, BT);
    gmskmod_print(mod);

    // create demodulator
    gmskdem demod = gmskdem_create(k, m, BT);
    gmskdem_set_eq_bw(demod, 0.01f);
    gmskdem_print(demod);

    unsigned int i;
    LIQUID_VLA(unsigned int, s, num_symbols);
    LIQUID_VLA(liquid_float_complex, x, num_samples);
    LIQUID_VLA(liquid_float_complex, y, num_samples);
    LIQUID_VLA(unsigned int, sym_out, num_symbols);

    // generate random data sequence
    for (i=0; i<num_symbols; i++)
        s[i] = rand() % 2;

    // modulate signal
    for (i=0; i<num_symbols; i++)
        gmskmod_modulate(mod, s[i], &x[k*i]);

    // add channel impairments
    for (i=0; i<num_samples; i++) {
        y[i]  = x[i]*cexpf(_Complex_I*(phi + i*dphi));
        y[i] += nstd*(randnf() + _Complex_I*randnf())*M_SQRT1_2;
    }

    // demodulate signal
    for (i=0; i<num_symbols; i++)
        gmskdem_demodulate(demod, &y[k*i], &sym_out[i]);

    // destroy modem objects
    gmskmod_destroy(mod);
    gmskdem_destroy(demod);

    // print results to screen
    unsigned int delay = 2*m;
    unsigned int num_errors=0;
    for (i=delay; i<num_symbols; i++) {
        //printf("  %4u : %2u (%2u)\n", i, s[i-delay], sym_out[i]);
        num_errors += (s[i-delay] == sym_out[i]) ? 0 : 1;
    }
    printf("symbol errors : %4u / %4u\n", num_errors, n);

    // write results to output file
    FILE * fid = fopen(filename,"w");
    fprintf(fid,"%% %s : auto-generated file\n", filename);
    fprintf(fid,"clear all\n");
    fprintf(fid,"close all\n");
    fprintf(fid,"k = %u;\n", k);
    fprintf(fid,"m = %u;\n", m);
    fprintf(fid,"BT = %f;\n", BT);
    fprintf(fid,"num_symbols = %u;\n", num_symbols);
    fprintf(fid,"num_samples = %u;\n", num_samples);

    fprintf(fid,"x = zeros(1,num_samples);\n");
    fprintf(fid,"y = zeros(1,num_samples);\n");
    for (i=0; i<num_samples; i++) {
        fprintf(fid,"x(%4u) = %12.8f + j*%12.8f;\n", i+1, crealf(x[i]), cimagf(x[i]));
        fprintf(fid,"y(%4u) = %12.8f + j*%12.8f;\n", i+1, crealf(y[i]), cimagf(y[i]));
    }
    fprintf(fid,"t=[0:(num_samples-1)]/k;\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(t,real(y),t,imag(y));\n");

    // artificially demodulate (generate receive filter, etc.)
    LIQUID_VLA(float, hr, 2*k*m+1);
    liquid_firdes_gmskrx(k,m,BT,0,hr);
    for (i=0; i<2*k*m+1; i++)
        fprintf(fid,"hr(%3u) = %12.8f;\n", i+1, hr[i]);
    fprintf(fid,"z = filter(hr,1,arg( ([y(2:end) 0]).*conj(y) ))/k;\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(t,z,t(k:k:end),z(k:k:end),'or');\n");
    fprintf(fid,"grid on;\n");

    fclose(fid);
    printf("results written to '%s'\n", filename);

    return 0;
}
