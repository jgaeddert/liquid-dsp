const char __docstr__[] =
"This example demonstrates the binary pre-demodulator synchronizer. A random"
" binary sequence is generated, modulated with BPSK, and then interpolated."
" The resulting sequence is used to generate a bpresync object which in turn"
" is used to detect a signal in the presence of carrier frequency and timing"
" offsets and additive white Gauss noise.";

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char*argv[])
{
    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*,    filename, "bpresync_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(unsigned, k,                2,     'k', "filter samples/symbol", NULL);
    liquid_argparse_add(unsigned, m,                5,     'm', "filter delay (symbols)", NULL);
    liquid_argparse_add(float,    beta,             0.3f,  'b', "bandwidth-time product", NULL);
    liquid_argparse_add(float,    dt,               0.0f,  't', "fractional sample timing offset", NULL);
    liquid_argparse_add(unsigned, num_sync_symbols, 64,    'n', "number of synchronization symbols", NULL);
    liquid_argparse_add(float,    SNRdB,            20.0f, 'S', "signal-to-noise ratio [dB]", NULL);
    liquid_argparse_add(float,    dphi,             0.02f, 'F', "carrier frequency offset", NULL);
    liquid_argparse_add(float,    phi,              0.234, 'P', "carrier phase offset", NULL);
    liquid_argparse_parse(argc,argv);

    unsigned int i;

    // validate input
    if (beta <= 0.0f || beta >= 1.0f)
        return liquid_error(LIQUID_EICONFIG,"bandwidth-time product must be in (0,1)");
    if (dt < -0.5f || dt > 0.5f)
        return liquid_error(LIQUID_EICONFIG,"fractional sample offset must be in [-0.5,0.5]");

    // derived values
    unsigned int num_symbols = num_sync_symbols + 2*m + 10;
    unsigned int num_samples = k*num_symbols;
    float nstd = powf(10.0f, -SNRdB/20.0f);

    // arrays
    LIQUID_VLA(liquid_float_complex, seq, num_sync_symbols);    // synchronization pattern (symbols)
    LIQUID_VLA(liquid_float_complex, s0, k*num_sync_symbols);   // synchronization pattern (samples)
    LIQUID_VLA(liquid_float_complex, x, num_samples);           // transmitted signal
    LIQUID_VLA(liquid_float_complex, y, num_samples);           // received signal
    LIQUID_VLA(liquid_float_complex, rxy, num_samples);         // pre-demod correlation output
    LIQUID_VLA(float, dphi_hat, num_samples);            // carrier offset estimate

    // create transmit/receive interpolator/decimator
    firinterp_crcf interp = firinterp_crcf_create_prototype(LIQUID_FIRFILT_RRC,k,m,beta,dt);

    // generate synchronization pattern (BPSK) and interpolate
    for (i=0; i<num_sync_symbols + 2*m; i++) {
        liquid_float_complex sym = 0.0f;
    
        if (i < num_sync_symbols) {
            sym = rand() % 2 ? -1.0f : 1.0f;
            seq[i] = sym;
        }

        if (i < 2*m) firinterp_crcf_execute(interp, sym, s0);
        else         firinterp_crcf_execute(interp, sym, &s0[k*(i-2*m)]);
    }

    // reset interpolator
    firinterp_crcf_reset(interp);

    // interpolate input
    for (i=0; i<num_symbols; i++) {
        liquid_float_complex sym = i < num_sync_symbols ? seq[i] : 0.0f;

        firinterp_crcf_execute(interp, sym, &x[k*i]);
    }

    // push through channel
    for (i=0; i<num_samples; i++)
        y[i] = x[i]*cexpf(_Complex_I*(dphi*i + phi)) + nstd*(randnf() + _Complex_I*randnf())*M_SQRT1_2;

    // create cross-correlator
    bpresync_cccf sync = bpresync_cccf_create(s0, k*num_sync_symbols, 0.05f, 11);
    bpresync_cccf_print(sync);

    // push signal through cross-correlator
    float rxy_max  = 0.0f;  // maximum cross-correlation
    float dphi_est = 0.0f;  // carrier frequency offset estimate
    int delay_est  = 0;     // delay estimate
    for (i=0; i<num_samples; i++) {
        
        // correlate
        bpresync_cccf_push(sync, y[i]);
        bpresync_cccf_execute(sync, &rxy[i], &dphi_hat[i]);

        // detect...
        if (cabsf(rxy[i]) > 0.6f) {
            printf("****** preamble found, rxy = %12.8f (dphi-hat: %12.8f), i=%3u ******\n",
                    cabsf(rxy[i]), dphi_hat[i], i);
        }
        
        // retain maximum
        if (cabsf(rxy[i]) > rxy_max) {
            rxy_max   = cabsf(rxy[i]);
            dphi_est  = dphi_hat[i];
            delay_est = (int)i - (int)2*k*m + 1;
        }
    }

    // destroy objects
    firinterp_crcf_destroy(interp);
    bpresync_cccf_destroy(sync);
    
    // print results
    printf("\n");
    printf("rxy (max) : %12.8f\n", rxy_max);
    printf("dphi est. : %12.8f ,error=%12.8f\n",      dphi_est, dphi-dphi_est);
    printf("delay est.: %12d ,error=%3d sample(s)\n", delay_est, k*num_sync_symbols - delay_est);
    printf("\n");

    // 
    // export results
    //
    FILE * fid = fopen(filename,"w");
    fprintf(fid,"%% %s : auto-generated file\n", filename);
    fprintf(fid,"clear all\n");
    fprintf(fid,"close all\n");
    fprintf(fid,"num_samples = %u;\n", num_samples);
    fprintf(fid,"num_symbols = %u;\n", num_symbols);
    fprintf(fid,"k           = %u;\n", k);

    fprintf(fid,"x   = zeros(1,num_samples);\n");
    fprintf(fid,"y   = zeros(1,num_samples);\n");
    fprintf(fid,"rxy = zeros(1,num_samples);\n");
    for (i=0; i<num_samples; i++) {
        fprintf(fid,"x(%4u)     = %12.8f + j*%12.8f;\n", i+1, crealf(x[i]),   cimagf(x[i]));
        fprintf(fid,"y(%4u)     = %12.8f + j*%12.8f;\n", i+1, crealf(y[i]),   cimagf(y[i]));
        fprintf(fid,"rxy(%4u)   = %12.8f + j*%12.8f;\n", i+1, crealf(rxy[i]), cimagf(rxy[i]));
    }

    fprintf(fid,"t=[0:(num_samples-1)]/k;\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(2,1,1);\n");
    fprintf(fid,"  plot(t,real(y), t,imag(y));\n");
    fprintf(fid,"  axis([0 num_symbols -2 2]);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  xlabel('time');\n");
    fprintf(fid,"  ylabel('received signal');\n");
    fprintf(fid,"subplot(2,1,2);\n");
    fprintf(fid,"  plot(t,abs(rxy));\n");
    fprintf(fid,"  axis([0 num_symbols 0 1.5]);\n");
    fprintf(fid,"  xlabel('time');\n");
    fprintf(fid,"  ylabel('correlator output');\n");
    fprintf(fid,"  grid on;\n");

    fclose(fid);
    printf("results written to '%s'\n", filename);

    return 0;
}
