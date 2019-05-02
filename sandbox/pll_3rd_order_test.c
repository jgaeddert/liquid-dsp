// Demonstrates a 3rd-order PLL design to track to drifting carrier offset.
#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <math.h>
#include <time.h>

#include "liquid.h"

int main()
{
    // options
    const char * filename      = "pll_3rd_order_test.m";
    float        phase_in      =  3.0f;    // carrier phase offset
    float        frequency_in  = -0.20;    // carrier frequency offset
    float        alpha         =  0.08f;   // phase adjustment factor
    unsigned int n             = 1200;     // number of samples
    float        df_in         =  0.30 /(float)n;

    // initialize states
    float beta          = 0.5*alpha*alpha;      // frequency adjustment factor
    float gamma         = 2*powf(alpha,4.0f);   // frequency drift adjustment factor
    float phase_out     = 0.0f;                 // output signal phase
    float frequency_out = 0.0f;                 // output signal frequency
    float df_out        = 0.0f;                 // output signal frequency drift

    // run basic simulation
    // open output file
    FILE * fid = fopen(filename,"w");
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"n = %u;\n", n);
    fprintf(fid,"x       = zeros(1,n); y       = zeros(1,n);\n");
    fprintf(fid,"phase_x = zeros(1,n); phase_y = zeros(1,n);\n");
    fprintf(fid,"freq_x  = zeros(1,n); freq_y  = zeros(1,n);\n");
    fprintf(fid,"dfreq_x = zeros(1,n); dfreq_y = zeros(1,n);\n");
    fprintf(fid,"phase_error = zeros(1,n);\n");
    int i;
    for (i=0; i<n; i++) {
        // compute input and output signals
        float complex signal_in  = cexpf(_Complex_I * phase_in);
        float complex signal_out = cexpf(_Complex_I * phase_out);

        // compute phase error estimate
        float phase_error = cargf( signal_in * conjf(signal_out) );

        // print results to file for plotting
        fprintf(fid,"x(%6u) = %12.8f + %12.8fi;  ", i+1, crealf(signal_in),  cimagf(signal_in));
        fprintf(fid,"y(%6u) = %12.8f + %12.8fi;  ", i+1, crealf(signal_out), cimagf(signal_out));
        fprintf(fid,"phase_x    (%6u) = %12.8e;  ", i+1, phase_in);
        fprintf(fid,"phase_y    (%6u) = %12.8e;  ", i+1, phase_out);
        fprintf(fid,"freq_x     (%6u) = %12.8e;  ", i+1, frequency_in);
        fprintf(fid,"freq_y     (%6u) = %12.8e;  ", i+1, frequency_out);
        fprintf(fid,"dfreq_x    (%6u) = %12.8e;  ", i+1, df_in);
        fprintf(fid,"dfreq_y    (%6u) = %12.8e;  ", i+1, df_out);
        fprintf(fid,"phase_error(%6u) = %12.8e;\n", i+1, phase_error);

        // apply loop filter and correct output phase and frequency
        phase_out     += alpha * phase_error;    // adjust phase
        frequency_out +=  beta * phase_error;    // adjust frequency
        df_out        += gamma * phase_error;    // adjust delta frequency

        // increment input and output frequency values
        frequency_in  += df_in;
        frequency_out += df_out;

        // increment input and output phase values
        phase_in  += frequency_in;
        phase_out += frequency_out;
    }
    fprintf(fid,"figure('position',[100 100 600 800]);\n");
    fprintf(fid,"t = 0:(n-1);\n");
    fprintf(fid,"subplot(4,1,1);\n");
    fprintf(fid,"  plot(t,real(x),t,real(y));\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  xlabel('sample index');\n");
    fprintf(fid,"  ylabel('real');\n");
    fprintf(fid,"  legend('input','output');\n");
    fprintf(fid,"subplot(4,1,2);\n");
    fprintf(fid,"  plot(t,phase_error);\n");
    fprintf(fid,"  xlabel('sample index');\n");
    fprintf(fid,"  ylabel('phase error');\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"subplot(4,1,3);\n");
    fprintf(fid,"  plot(t,freq_x-freq_y);\n");
    fprintf(fid,"  xlabel('sample index');\n");
    fprintf(fid,"  ylabel('frequency error');\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"subplot(4,1,4);\n");
    fprintf(fid,"  plot(t,dfreq_x-dfreq_y);\n");
    fprintf(fid,"  xlabel('sample index');\n");
    fprintf(fid,"  ylabel('delta frequency error');\n");
    fprintf(fid,"  grid on;\n");
    fclose(fid);

    printf("output written to %s.\n", filename);
    return 0;
}
