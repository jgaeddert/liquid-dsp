//
// agc_qpsk_example.c
//
// Automatic gain control test for data signals with fluctuating signal
// levels.  QPSK modulation introduces periodic random zero-crossings
// which gives instantaneous amplitude levels near zero.  This example
// tests the response of the AGC to these types of signals.
//

#include <stdio.h>
#include "liquid.h"

#define OUTPUT_FILENAME "agc_qpsk_example.m"

int main() {
    // options
    float etarget=1.0f;     // target energy
    float gamma=0.01f;      // channel gain
    float bt=1e-2f;         // loop bandwidth
    unsigned int num_symbols=100;     // number of iterations
    unsigned int d=5;       // print every d iterations

    unsigned int k=2;
    unsigned int m=3;
    float beta=0.3f;

    // create objects
    modem mod = modem_create(LIQUID_MODEM_QPSK,2);
    unsigned int h_len = 2*k*m+1;
    float h[h_len];
    design_rrc_filter(k,m,beta,0,h);
    interp_crcf interp = interp_crcf_create(k,h,h_len);
    agc_crcf p = agc_crcf_create();
    agc_crcf_set_target(p, etarget);
    agc_crcf_set_bandwidth(p, bt);

    unsigned int i;
    for (i=0; i<h_len; i++)
        printf("h(%4u) = %8.4f;\n", i+1, h[i]);

    // print info
    printf("automatic gain control // target: %8.4f, loop bandwidth: %4.2e\n",etarget,bt);

    FILE* fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n",OUTPUT_FILENAME);
    fprintf(fid,"clear all;\nclose all;\n\n");

    unsigned int sym, n=0, num_samples=num_symbols*k;
    float complex s, x[num_samples];
    for (i=0; i<num_symbols; i++) {
        // generate random symbol
        sym = modem_gen_rand_sym(mod);
        modem_modulate(mod, sym, &s);
        s *= gamma;

        interp_crcf_execute(interp, s, &x[n]);
        
        n+=k;
    }

    // run agc
    float complex y[num_samples];
    for (i=0; i<num_samples; i++) {
        agc_crcf_execute(p, x[i], &y[i]);
        if ( ((i+1)%d) == 0 )
            printf("%4u: %8.3f\n", i+1, agc_crcf_get_signal_level(p));

        fprintf(fid,"   x(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(x[i]), cimagf(x[i]));
        fprintf(fid,"   y(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(y[i]), cimagf(y[i]));
        fprintf(fid,"rssi(%4u) = %12.4e;\n", i+1, agc_crcf_get_signal_level(p));
    }


    fprintf(fid,"\n\n");
    fprintf(fid,"n = length(x);\n");
    fprintf(fid,"t = 0:(n-1);\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(t,10*log10(rssi),'-k','LineWidth',2);\n");
    fprintf(fid,"xlabel('sample index');\n");
    fprintf(fid,"ylabel('rssi [dB]');\n");
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(t,real(y),t,imag(y));\n");
    fprintf(fid,"xlabel('sample index');\n");
    fprintf(fid,"ylabel('agc output');\n");
    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);

    modem_destroy(mod);
    agc_crcf_destroy(p);
    interp_crcf_destroy(interp);

    printf("done.\n");
    return 0;
}

