const char __docstr__[] =
"Example of the finite impulse response (FIR) polyphase filterbank"
" (PFB) channelizer with an output rate of 2 Fs / M as an (almost)"
" perfect reconstructive system.";

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char*argv[])
{
    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*,    filename, "firpfbch2_crcf_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(unsigned, num_channels,  6, 'M', "number of channels", NULL);
    liquid_argparse_add(unsigned, m,             4, 'm', "filter length [symbols]", NULL);
    liquid_argparse_add(float,    As,           80, 'a', "filter stop-band attenuation", NULL);
    liquid_argparse_add(unsigned, num_symbols,  24, 'n', "number of symbols", NULL);
    liquid_argparse_parse(argc,argv);

    // validate input
    if (num_channels < 2 || num_channels % 2)
        return liquid_error(LIQUID_EICONFIG,"number of channels must be greater than 2 and even");
    if (m == 0)
        return liquid_error(LIQUID_EICONFIG,"filter semi-length must be greater than zero");
    if (num_symbols == 0)
        return liquid_error(LIQUID_EICONFIG,"number of symbols must be greater than zero");

    // derived values
    unsigned int i;
    unsigned int num_samples = num_channels * num_symbols;

    // allocate arrays
    LIQUID_VLA(liquid_float_complex, x, num_samples);
    LIQUID_VLA(liquid_float_complex, y, num_samples);

    // generate input signal
    for (i=0; i<num_samples; i++) {
        //x[i] = (i==0) ? 1.0f : 0.0f;
        x[i] = cexpf( (-0.05f + 0.07f*_Complex_I)*i );  // decaying complex exponential
    }

    // create filterbank objects from prototype
    firpfbch2_crcf qa = firpfbch2_crcf_create_kaiser(LIQUID_ANALYZER,    num_channels, m, As);
    firpfbch2_crcf qs = firpfbch2_crcf_create_kaiser(LIQUID_SYNTHESIZER, num_channels, m, As);
    firpfbch2_crcf_print(qa);
    firpfbch2_crcf_print(qs);

    // run channelizer
    LIQUID_VLA(liquid_float_complex, Y, num_channels);
    for (i=0; i<num_samples; i+=num_channels/2) {
        // run analysis filterbank
        firpfbch2_crcf_execute(qa, &x[i], Y);

        // run synthesis filterbank
        firpfbch2_crcf_execute(qs, Y, &y[i]);
    }

    // destroy filterbank objects
    firpfbch2_crcf_destroy(qa); // analysis filterbank
    firpfbch2_crcf_destroy(qs); // synthesis filterbank

    // print output
    for (i=0; i<num_samples; i++)
        printf("%4u : %12.8f + %12.8fj\n", i, crealf(y[i]), cimagf(y[i]));

    // compute RMSE
    float rmse = 0.0f;
    unsigned int delay = 2*num_channels*m - num_channels/2 + 1;
    for (i=0; i<num_samples; i++) {
        liquid_float_complex err = y[i] - (i < delay ? 0.0f : x[i-delay]);
        rmse += crealf( err*conjf(err) );
    }
    rmse = sqrtf( rmse/(float)num_samples );
    printf("rmse : %12.4e\n", rmse);

    //
    // EXPORT DATA TO FILE
    //
    FILE * fid = fopen(filename,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n", filename);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"num_channels=%u;\n", num_channels);
    fprintf(fid,"m = %u;\n", m);
    fprintf(fid,"num_symbols=%u;\n",  num_symbols);
    fprintf(fid,"num_samples = num_channels*num_symbols;\n");

    fprintf(fid,"x = zeros(1,num_samples);\n");
    fprintf(fid,"y = zeros(1,num_samples);\n");

    // save input and output arrays
    for (i=0; i<num_samples; i++) {
        fprintf(fid,"x(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(x[i]), cimag(x[i]));
        fprintf(fid,"y(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(y[i]), cimag(y[i]));
    }

    // save error vector
    for (i=delay; i<num_samples; i++) {
        liquid_float_complex e = y[i] - x[i-delay];
        fprintf(fid,"e(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(e), cimag(e));
    }

    // plot results
    fprintf(fid,"t = 0:(num_samples-1);\n");
    fprintf(fid,"delay = %u;\n", delay);
    fprintf(fid,"figure;\n");
    fprintf(fid,"title('composite');\n");
    fprintf(fid,"subplot(3,1,1);\n");
    fprintf(fid,"    plot(t,real(x), t-delay,real(y),'s','MarkerSize',1);\n");
    fprintf(fid,"    axis([-2 num_samples -0.3 1.1]);\n");
    fprintf(fid,"    ylabel('real');\n");
    fprintf(fid,"    legend('original','reconstructed','location','northeast');\n");
    fprintf(fid,"    grid on;\n");
    fprintf(fid,"subplot(3,1,2);\n");
    fprintf(fid,"    plot(t,imag(x), t-delay,imag(y),'s','MarkerSize',1);\n");
    fprintf(fid,"    axis([-2 num_samples -0.3 1.1]);\n");
    fprintf(fid,"    ylabel('imag');\n");
    fprintf(fid,"    grid on;\n");
    fprintf(fid,"subplot(3,1,3);\n");
    fprintf(fid,"    plot(t-delay,real(e), t-delay,imag(e));\n");
    fprintf(fid,"    emax = 1.2*max(abs(e));\n");
    fprintf(fid,"    axis([-2 num_samples -emax emax]);\n");
    fprintf(fid,"    legend('real','imag','location','northeast');\n");
    fprintf(fid,"    xlabel('time');\n");
    fprintf(fid,"    ylabel('error');\n");
    fprintf(fid,"    grid on;\n");

    fclose(fid);
    printf("results written to '%s'\n", filename);

    printf("done.\n");
    return 0;
}
