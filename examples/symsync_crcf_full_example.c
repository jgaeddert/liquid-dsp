const char __docstr__[] =
"This example extends that of `symsync_crcf_example.c` by including options"
" for simulating a timing rate offset in addition to just a timing phase"
" error. The resulting output file shows not just the constellation but the"
" time domain sequence as well as the timing phase estimate over time.";

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
    liquid_argparse_add(char*, filename, "symsync_crcf_full_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(char*,    ftype_str,"arkaiser",'f',"filter type", liquid_argparse_firfilt);
    liquid_argparse_add(unsigned, k,           2,  'k', "filter samples per symbol (input)", NULL);
    liquid_argparse_add(unsigned, k_out,       2,  'K', "filter samples per symbol (output)", NULL);
    liquid_argparse_add(unsigned, m,           9,  'm', "filter semi-length", NULL);
    liquid_argparse_add(float,    beta,      0.3,  'b', "filter excess bandwidth factor", NULL);
    liquid_argparse_add(char*,    mod_str,  "qpsk",'M', "modulation scheme", liquid_argparse_modem);
    liquid_argparse_add(unsigned, num_filters,  32,'n', "FFT size", NULL);
    liquid_argparse_add(unsigned, num_symbols, 800,'N', "number of samples to simulate", NULL);
    liquid_argparse_add(float,    bandwidth, 0.02, 'w', "loop filter bandwidth", NULL);
    liquid_argparse_add(float,    SNRdB,       30, 's', "signal-to-noise ratio [dB]", NULL);
    liquid_argparse_add(float,    r,            1, 'r', "resampled rate", NULL);
    liquid_argparse_add(float,    tau,       -0.2, 'T', "fractional symbol offset", NULL);
    liquid_argparse_add(bool,     sequence, false, 'R', "use random data or 101010 phasing pattern", NULL);
    liquid_argparse_parse(argc,argv);

    // set filter types
    liquid_firfilt_type ftype_tx = (liquid_firfilt_type)liquid_getopt_str2firfilt(ftype_str);
    liquid_firfilt_type ftype_rx = (liquid_firfilt_type)liquid_getopt_str2firfilt(ftype_str);
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
    if (num_filters == 0)
        return liquid_error(LIQUID_EICONFIG,"number of polyphase filters must be greater than 0");
    if (bandwidth <= 0.0f)
        return liquid_error(LIQUID_EICONFIG,"timing PLL bandwidth must be greater than 0");
    if (num_symbols == 0)
        return liquid_error(LIQUID_EICONFIG,"number of symbols must be greater than 0");
    if (tau < -1.0f || tau > 1.0f)
        return liquid_error(LIQUID_EICONFIG,"timing phase offset must be in [-1,1]");
    if (r < 0.5f || r > 2.0f)
        return liquid_error(LIQUID_EICONFIG,"timing frequency offset must be in [0.5,2]");

    // compute delay
    while (tau < 0) tau += 1.0f;    // ensure positive tau
    float g = k*tau;                // number of samples offset
    int ds=floorf(g);               // additional symbol delay
    float dt = (g - (float)ds);     // fractional sample offset
    if (dt > 0.5f) {                // force dt to be in [0.5,0.5]
        dt -= 1.0f;
        ds++;
    }

    unsigned int i, n=0;

    unsigned int num_samples = k*num_symbols;
    unsigned int num_samples_resamp = (unsigned int) ceilf(num_samples*r*1.1f) + 4;
    LIQUID_VLA(liquid_float_complex, s, num_symbols);           // data symbols
    LIQUID_VLA(liquid_float_complex, x, num_samples);           // interpolated samples
    LIQUID_VLA(liquid_float_complex, y, num_samples_resamp);    // resampled data (resamp_crcf)
    LIQUID_VLA(liquid_float_complex, z, k_out*num_symbols + 64);// synchronized samples
    LIQUID_VLA(liquid_float_complex, sym_out, num_symbols + 64);// synchronized symbols

    // generate symbols
    modemcf mod = modemcf_create((modulation_scheme)liquid_getopt_str2mod(mod_str));
    for (i=0; i<num_symbols; i++) {
        if (sequence) {
            s[i] = (i%2) ? 1.0f : -1.0f;  // 101010 phasing pattern
        } else {
            // random modulated symbol
            unsigned int sym = modemcf_gen_rand_sym(mod);
            modemcf_modulate(mod, sym, s+i);
        }
    }
    modemcf_destroy(mod);

    // 
    // create and run interpolator
    //

    // design interpolating filter
    unsigned int h_len = 2*k*m+1;
    LIQUID_VLA(float, h, h_len);
    liquid_firdes_prototype(ftype_tx,k,m,beta,dt,h);
    firinterp_crcf q = firinterp_crcf_create(k,h,h_len);
    for (i=0; i<num_symbols; i++) {
        firinterp_crcf_execute(q, s[i], &x[n]);
        n+=k;
    }
    assert(n == num_samples);
    firinterp_crcf_destroy(q);

    // 
    // run resampler
    //
    unsigned int resamp_len = 10*k; // resampling filter semi-length (filter delay)
    float resamp_bw = 0.45f;        // resampling filter bandwidth
    float resamp_as = 60.0f;        // resampling filter stop-band attenuation
    unsigned int resamp_npfb = 64;  // number of filters in bank
    resamp_crcf f = resamp_crcf_create(r, resamp_len, resamp_bw, resamp_as, resamp_npfb);
    unsigned int num_samples_resampled = 0;
    unsigned int num_written;
    for (i=0; i<num_samples; i++) {
#if 0
        // bypass arbitrary resampler
        y[i] = x[i];
        num_samples_resampled = num_samples;
#else
        // TODO : compensate for resampler filter delay
        resamp_crcf_execute(f, x[i], &y[num_samples_resampled], &num_written);
        num_samples_resampled += num_written;
#endif
    }
    resamp_crcf_destroy(f);

    // 
    // add noise
    //
    float nstd = powf(10.0f, -SNRdB/20.0f);
    for (i=0; i<num_samples_resampled; i++)
        y[i] += nstd*(randnf() + _Complex_I*randnf());


    // 
    // create and run symbol synchronizer
    //

    symsync_crcf d = symsync_crcf_create_rnyquist(ftype_rx, k, m, beta, num_filters);
    symsync_crcf_set_lf_bw(d,bandwidth);
    symsync_crcf_set_output_rate(d,k_out);

    unsigned int num_samples_sync=0;
    unsigned int nn;
    unsigned int num_symbols_sync = 0;
    LIQUID_VLA(float, tau_hat, num_samples);
    for (i=ds; i<num_samples_resampled; i++) {
        tau_hat[num_samples_sync] = symsync_crcf_get_tau(d);
        symsync_crcf_execute(d, &y[i], 1, &z[num_samples_sync], &nn);

        // decimate
        unsigned int j;
        for (j=0; j<nn; j++) {
            if ( (num_samples_sync%k_out)==0 )
                sym_out[num_symbols_sync++] = z[num_samples_sync];
            num_samples_sync++;
        }
    }
    symsync_crcf_destroy(d);

    // print last several symbols to screen
    printf("output symbols:\n");
    printf("  ...\n");
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
    fprintf(fid,"k_out=%u;\n",k_out);
    fprintf(fid,"num_filters=%u;\n",num_filters);
    fprintf(fid,"num_symbols=%u;\n",num_symbols);

    for (i=0; i<h_len; i++)
        fprintf(fid,"h(%3u) = %12.5f;\n", i+1, h[i]);

    for (i=0; i<num_symbols; i++)
        fprintf(fid,"s(%3u) = %12.8f + j*%12.8f;\n", i+1, crealf(s[i]), cimagf(s[i]));

    for (i=0; i<num_samples; i++)
        fprintf(fid,"x(%3u) = %12.8f + j*%12.8f;\n", i+1, crealf(x[i]), cimagf(x[i]));
        
    for (i=0; i<num_samples_resampled; i++)
        fprintf(fid,"y(%3u) = %12.8f + j*%12.8f;\n", i+1, crealf(y[i]), cimagf(y[i]));
        
    for (i=0; i<num_samples_sync; i++)
        fprintf(fid,"z(%3u) = %12.8f + j*%12.8f;\n", i+1, crealf(z[i]), cimagf(z[i]));
        
    for (i=0; i<num_symbols_sync; i++)
        fprintf(fid,"sym_out(%3u) = %12.8f + j*%12.8f;\n", i+1, crealf(sym_out[i]), cimagf(sym_out[i]));
        
    for (i=0; i<num_samples_sync; i++)
        fprintf(fid,"tau_hat(%3u) = %12.8f;\n", i+1, tau_hat[i]);


    fprintf(fid,"\n\n");
    fprintf(fid,"%% scale QPSK in-phase by sqrt(2)\n");
    fprintf(fid,"z = z*sqrt(2);\n");
    fprintf(fid,"\n\n");
    fprintf(fid,"tz = [0:length(z)-1]/k_out;\n");
    fprintf(fid,"iz = 1:k_out:length(z);\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(tz,     real(z),    '-',...\n");
    fprintf(fid,"     tz(iz), real(z(iz)),'or');\n");
    fprintf(fid,"xlabel('Time');\n");
    fprintf(fid,"ylabel('Output Signal (real)');\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"legend('output time series','optimum timing','location','northeast');\n");

    fprintf(fid,"iz0 = iz( 1:round(length(iz)*0.5) );\n");
    fprintf(fid,"iz1 = iz( round(length(iz)*0.5):length(iz) );\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"hold on;\n");
    fprintf(fid,"plot(real(z(iz0)),imag(z(iz0)),'x','MarkerSize',4,'Color',[0.6 0.6 0.6]);\n");
    fprintf(fid,"plot(real(z(iz1)),imag(z(iz1)),'o','MarkerSize',4,'Color',[0 0.25 0.5]);\n");
    fprintf(fid,"hold off;\n");
    fprintf(fid,"axis square;\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"axis([-1 1 -1 1]*2.0);\n");
    fprintf(fid,"xlabel('In-phase');\n");
    fprintf(fid,"ylabel('Quadrature');\n");
    fprintf(fid,"legend(['first 50%%'],['last 50%%'],'location','northeast');\n");

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
