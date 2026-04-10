const char __docstr__[] =
"This is a simplified example of the symync family of objects to show how"
" symbol timing can be recovered after the matched filter output.";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <assert.h>

#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char* argv[])
{
    // define variables and parse command-line arguments
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*, filename, "symsync_crcf_kaiser_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(char*,    ftype_str,"arkaiser",'f',"filter type", liquid_argparse_firfilt);
    liquid_argparse_add(unsigned, k,           2,  'k', "filter samples per symbol", NULL);
    liquid_argparse_add(unsigned, m,           9,  'm', "filter semi-length", NULL);
    liquid_argparse_add(float,    beta,      0.3,  'b', "filter excess bandwidth factor", NULL);
    liquid_argparse_add(char*,    mod_str,  "qpsk",'M', "modulation scheme", liquid_argparse_modem);
    liquid_argparse_add(unsigned, num_filters,  32,'n', "FFT size", NULL);
    liquid_argparse_add(unsigned, num_symbols, 800,'N', "number of samples to simulate", NULL);
    liquid_argparse_add(float,    bandwidth, 0.02, 'w', "loop filter bandwidth", NULL);
    liquid_argparse_add(float,    tau,       0.50, 't', "fractional sample offset", NULL);
    liquid_argparse_add(float,    SNRdB,       30, 's', "signal-to-noise ratio [dB]", NULL);
    liquid_argparse_parse(argc,argv);

    // validate input
    if (k < 2)
        return liquid_error(LIQUID_EICONFIG,"k (samples/symbol) must be at least 2");
    if (m < 1)
        return liquid_error(LIQUID_EICONFIG,"m (filter delay) must be greater than 0");
    if (beta <= 0.0f || beta > 1.0f)
        return liquid_error(LIQUID_EICONFIG,"beta (excess bandwidth factor) must be in (0,1]");
    if (num_filters == 0)
        return liquid_error(LIQUID_EICONFIG,"number of polyphase filters must be greater than 0");
    if (num_symbols == 0)
        return liquid_error(LIQUID_EICONFIG,"number of symbols must be greater than 0");
    if (bandwidth <= 0.0f)
        return liquid_error(LIQUID_EICONFIG,"timing bandwidth must be greater than 0");
    if (tau < -1.0f || tau > 1.0f)
        return liquid_error(LIQUID_EICONFIG,"timing phase offset must be in [-1,1]");

    // derived values
    unsigned int num_samples = k*num_symbols;
    LIQUID_VLA(liquid_float_complex, x, num_samples);     // interpolated samples
    LIQUID_VLA(liquid_float_complex, y, num_samples);     // received signal (with noise)
    LIQUID_VLA(float, tau_hat, num_samples);     // instantaneous timing offset estimate
    LIQUID_VLA(liquid_float_complex, sym_out, num_symbols+64);  // synchronized symbols

    // create sequence of Nyquist-interpolated QPSK symbols
    liquid_firfilt_type ftype = (liquid_firfilt_type)liquid_getopt_str2firfilt(ftype_str);
    firinterp_crcf interp = firinterp_crcf_create_prototype(ftype,k,m,beta,tau);
    unsigned int i;
    for (i=0; i<num_symbols; i++) {
        // generate random QPSK symbol
        liquid_float_complex s = ( rand() % 2 ? M_SQRT1_2 : -M_SQRT1_2 ) +
                          ( rand() % 2 ? M_SQRT1_2 : -M_SQRT1_2 ) * _Complex_I;

        // interpolate symbol
        firinterp_crcf_execute(interp, s, &x[i*k]);
    }
    firinterp_crcf_destroy(interp);

    // add noise
    float nstd = powf(10.0f, -SNRdB/20.0f);
    for (i=0; i<num_samples; i++)
        y[i] = x[i] + nstd*(randnf() + _Complex_I*randnf());


    // create and run symbol synchronizer
    symsync_crcf decim = symsync_crcf_create_kaiser(k, m, beta, num_filters);
    symsync_crcf_set_lf_bw(decim,bandwidth); // set loop filter bandwidth

    // NOTE: we could just synchronize entire block (see following line);
    //       however we would like to save the instantaneous timing offset
    //       estimate for plotting purposes
    //symsync_crcf_execute(d, y, num_samples, sym_out, &num_symbols_sync);

    unsigned int num_symbols_sync = 0;
    unsigned int num_written=0;
    for (i=0; i<num_samples; i++) {
        // save instantaneous timing offset estimate
        tau_hat[i] = symsync_crcf_get_tau(decim);

        // execute one sample at a time
        symsync_crcf_execute(decim, &y[i], 1, &sym_out[num_symbols_sync], &num_written);

        // increment number of symbols synchronized
        num_symbols_sync += num_written;
    }
    symsync_crcf_destroy(decim);

    // print last several symbols to screen
    printf("output symbols:\n");
    for (i=num_symbols_sync-10; i<num_symbols_sync; i++)
        printf("  sym_out(%2u) = %8.4f + j*%8.4f;\n", i+1, crealf(sym_out[i]), cimagf(sym_out[i]));


    //
    // export output file
    //
    FILE* fid = fopen(filename,"w");
    fprintf(fid,"%% %s, auto-generated file\n\n", filename);
    fprintf(fid,"close all;\nclear all;\n\n");

    fprintf(fid,"k=%u;\n",k);
    fprintf(fid,"m=%u;\n",m);
    fprintf(fid,"beta=%12.8f;\n",beta);
    fprintf(fid,"num_filters=%u;\n",num_filters);
    fprintf(fid,"num_symbols=%u;\n",num_symbols);

    for (i=0; i<num_samples; i++)
        fprintf(fid,"x(%3u) = %12.8f + j*%12.8f;\n", i+1, crealf(x[i]), cimagf(x[i]));
        
    for (i=0; i<num_samples; i++)
        fprintf(fid,"y(%3u) = %12.8f + j*%12.8f;\n", i+1, crealf(y[i]), cimagf(y[i]));
        
    for (i=0; i<num_samples; i++)
        fprintf(fid,"tau_hat(%3u) = %12.8f;\n", i+1, tau_hat[i]);
        
    for (i=0; i<num_symbols_sync; i++)
        fprintf(fid,"sym_out(%3u) = %12.8f + j*%12.8f;\n", i+1, crealf(sym_out[i]), cimagf(sym_out[i]));
        
    fprintf(fid,"i0 = 1:round( 0.5*num_symbols );\n");
    fprintf(fid,"i1 = round( 0.5*num_symbols ):num_symbols;\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"hold on;\n");
    fprintf(fid,"plot(real(sym_out(i0)),imag(sym_out(i0)),'x','MarkerSize',4,'Color',[0.6 0.6 0.6]);\n");
    fprintf(fid,"plot(real(sym_out(i1)),imag(sym_out(i1)),'o','MarkerSize',4,'Color',[0 0.25 0.5]);\n");
    fprintf(fid,"hold off;\n");
    fprintf(fid,"axis square;\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"axis([-1 1 -1 1]*1.6);\n");
    fprintf(fid,"xlabel('In-phase');\n");
    fprintf(fid,"ylabel('Quadrature');\n");
    fprintf(fid,"legend(['first 50%%'],['last 50%%']);\n");

    fprintf(fid,"figure;\n");
    fprintf(fid,"tt = 0:(length(tau_hat)-1);\n");
    fprintf(fid,"b = floor(num_filters*tau_hat + 0.5);\n");
    fprintf(fid,"stairs(tt,tau_hat*num_filters);\n");
    fprintf(fid,"hold on;\n");
    fprintf(fid,"plot(tt,b,'-k','Color',[0 0 0]);\n");
    fprintf(fid,"hold off;\n");
    fprintf(fid,"xlabel('time');\n");
    fprintf(fid,"ylabel('filterbank index');\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"axis([0 length(tau_hat) -1 num_filters]);\n");

    fclose(fid);
    printf("results written to %s.\n", filename);

    // clean it up
    printf("done.\n");
    return 0;
}
