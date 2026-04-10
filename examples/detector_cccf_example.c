const char __docstr__[] =
"This example demonstrates the binary pre-demodulator synchronizer. A random"
" binary sequence is generated, modulated with BPSK, and then interpolated."
" The resulting sequence is used to generate a detector object which in turn"
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
    liquid_argparse_add(char*, filename, "dds_cccf_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(unsigned, n,            128,   'n', "number of sync samples", NULL);
    liquid_argparse_add(float,    dt,           0.0f,  'T', "fractional sample timing offset", NULL);
    liquid_argparse_add(float,    noise_floor, -30.0f, '0', "noise floor [dB]", NULL);
    liquid_argparse_add(float,    SNRdB,        20.0f, 'S', "signal-to-noise ratio [dB]", NULL);
    liquid_argparse_add(float,    dphi,         0.0f,  'F', "carrier frequency offset", NULL);
    liquid_argparse_add(float,    phi,          0.0f,  'P', "carrier phase offset", NULL);
    liquid_argparse_add(float,    threshold,    0.3f,  't', "detection threshold", NULL);
    liquid_argparse_parse(argc,argv);

    // validate input
    if (dt < -0.5f || dt > 0.5f)
        return liquid_error(LIQUID_EICONFIG,"fractional sample offset must be in [-0.5,0.5]");

    // derived values
    unsigned int num_samples = 3*n;
    float nstd = powf(10.0f, noise_floor/20.0f);
    float gamma = powf(10.0f, (SNRdB + noise_floor)/20.0f);

    // arrays
    LIQUID_VLA(liquid_float_complex, s, n);             // synchronization pattern (samples)
    LIQUID_VLA(liquid_float_complex, x, num_samples);   // transmitted signal
    LIQUID_VLA(liquid_float_complex, y, num_samples);   // received signal

    // generate synchronization pattern (OFDM symbol, slightly over-sampled)
    unsigned int i;
    LIQUID_VLA(liquid_float_complex, S, n);
    for (i=0; i<n; i++)
        S[i] = (i < 0.4*n || i > 0.6*n) ? randnf() + _Complex_I*randnf() : 0.0f;
    fft_run(n, S, s, LIQUID_FFT_BACKWARD, 0);
    float s2 = 0.0f;
    for (i=0; i<n; i++)
        s2 += crealf(s[i]*conjf(s[i]));
    for (i=0; i<n; i++)
        s[i] /= sqrtf(s2 / (float)n);

    // generate transmitted signal: 0 0 0 ... 0 s[0] s[1] ... s[n-1] 0 0 0 ... 0
    for (i=0; i<n; i++) {
        x[0*n+i] = 0.0f;
        x[1*n+i] = s[i];
        x[2*n+i] = 0.0f;
    }

    // generate received signal (add channel impairments)
    unsigned int d = 11;    // fractional sample filter delay
    firfilt_crcf finterp = firfilt_crcf_create_kaiser(2*d+1, 0.45f, 40.0f, dt);
    for (i=0; i<num_samples+d; i++) {
        // fractional sample timing offset
        if (i < num_samples) firfilt_crcf_push(finterp, x[i]);
        else                 firfilt_crcf_push(finterp, 0.0f);

        if (i < d) firfilt_crcf_execute(finterp, &y[0]);
        else       firfilt_crcf_execute(finterp, &y[i-d]);
    }
    firfilt_crcf_destroy(finterp);

    for (i=0; i<num_samples; i++) {
        // channel gain
        y[i] *= gamma;

        // carrier offset
        y[i] *= cexpf(_Complex_I*(dphi*i + phi));
        
        // noise
        y[i] += nstd*(randnf() + _Complex_I*randnf())*M_SQRT1_2;
    }

    // create cross-correlator
    detector_cccf sync = detector_cccf_create(s, n, threshold, 0.03f);
    detector_cccf_print(sync);

    // push signal through detector
    float tau_hat   = 0.0f;
    float dphi_hat  = 0.0f;
    float gamma_hat = 1.0f;
    int signal_detected = 0;
    unsigned int index = 0;
    for (i=0; i<num_samples; i++) {
        
        // correlate
        int detected = detector_cccf_correlate(sync, y[i], &tau_hat, &dphi_hat, &gamma_hat);

        if (detected) {
            signal_detected = 1;
            printf("****** preamble found, tau_hat=%8.6f, dphi_hat=%8.6f, gamma_hat=%8.6f\n",
                    tau_hat, dphi_hat, gamma_hat);
            index = i;
        }
    }

    // destroy objects
    detector_cccf_destroy(sync);
    
    // print results
    printf("\n");
    printf("signal detected :   %s\n", signal_detected ? "yes" : "no");
    float delay_est = (float) index + tau_hat;
    float delay     = (float)(2*n) + dt; // actual delay (samples)
    printf("delay estimate  : %8.3f, actual=%8.3f (error=%8.3f) sample(s)\n", delay_est, delay, delay-delay_est);
    printf("dphi estimate   : %8.5f, actual=%8.5f (error=%8.5f) rad/sample\n",dphi_hat,  dphi,  dphi-dphi_hat);
    printf("gamma estimate  : %8.3f, actual=%8.3f (error=%8.3f) dB\n",        20*log10f(gamma_hat), 20*log10f(gamma), 20*log10(gamma/gamma_hat));
    printf("\n");

    // 
    // export results
    //
    FILE * fid = fopen(filename,"w");
    fprintf(fid,"%% %s : auto-generated file\n", filename);
    fprintf(fid,"clear all\n");
    fprintf(fid,"close all\n");
    fprintf(fid,"n           = %u;\n", n);
    fprintf(fid,"num_samples = %u;\n", num_samples);

    fprintf(fid,"s = zeros(1,n);\n");
    for (i=0; i<n; i++)
        fprintf(fid,"s(%4u) = %12.8f + j*%12.8f;\n", i+1, crealf(s[i]), cimagf(s[i]));

    fprintf(fid,"x = zeros(1,num_samples);\n");
    fprintf(fid,"y = zeros(1,num_samples);\n");
    for (i=0; i<num_samples; i++) {
        fprintf(fid,"x(%4u) = %12.8f + j*%12.8f;\n", i+1, crealf(x[i]), cimagf(x[i]));
        fprintf(fid,"y(%4u) = %12.8f + j*%12.8f;\n", i+1, crealf(y[i]), cimagf(y[i]));
    }

    fprintf(fid,"t=[0:(num_samples-1)];\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(2,1,1);\n");
    fprintf(fid,"  plot(t,real(x), t,imag(x));\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  xlabel('time');\n");
    fprintf(fid,"  ylabel('transmitted signal');\n");
    fprintf(fid,"subplot(2,1,2);\n");
    fprintf(fid,"  plot(t,real(y), t,imag(y));\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  xlabel('time');\n");
    fprintf(fid,"  ylabel('received signal');\n");

    fclose(fid);
    printf("results written to '%s'\n", filename);

    return 0;
}
