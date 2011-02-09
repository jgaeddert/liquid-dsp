//
// agc_example.c
//
// Automatic gain control example demonstrating the transient
// responses between the three AGC types (LIQUID_AGC_DEFAULT,
// LIQUID_AGC_LOG, LIQUID_AGC_EXP).
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#include <getopt.h>
#include "liquid.h"

#define OUTPUT_FILENAME "agc_example.m"

// print usage/help message
void usage()
{
    printf("agc_example [options]\n");
    printf("  u/h   : print usage\n");
    printf("  n     : number of samples, n>=100, default: 2048\n");
    printf("  N     : noise floor [dB], default: -25.0\n");
    printf("  s     : SNR [dB], default: 25.0\n");
    printf("  b     : AGC bandwidth, b >= 0, default: 0.01\n");
    printf("  D     : AGC internal decimation factor, D>0, default: 4\n");
}


int main(int argc, char*argv[])
{
    // options
    float etarget=1.0f;                 // target level
    float noise_floor = -25.0f;         // noise floor [dB]
    float SNRdB = 25.0f;                // signal-to-noise ratio [dB]
    float bt=0.01f;                     // agc loop bandwidth
    unsigned int D = 4;                 // AGC internal decimation factor
    unsigned int num_samples = 2048;    // number of samples
    unsigned int d=num_samples/32;      // print every d iterations

    int dopt;
    while((dopt = getopt(argc,argv,"uhn:N:s:b:D:")) != EOF){
        switch (dopt) {
        case 'h':
        case 'u': usage(); return 0;
        case 'n': num_samples = atoi(optarg);   break;
        case 'N': noise_floor = atof(optarg);   break;
        case 's': SNRdB = atof(optarg);         break;
        case 'b': bt = atof(optarg);            break;
        case 'D': D = atoi(optarg);             break;
        default:
            fprintf(stderr,"error: %s, unknown option: %c\n", argv[0], dopt);
            exit(1);
        }
    }

    // validate input
    if (bt < 0.0f) {
        fprintf(stderr,"error: %s, bandwidth must be positive\n", argv[0]);
        exit(1);
    } else if (D == 0) {
        fprintf(stderr,"error: %s, decimation factor must be greater than zero\n", argv[0]);
        exit(1);
    } else if (num_samples < 100) {
        fprintf(stderr,"error: %s, must have at least 100 samples\n", argv[0]);
        exit(1);
    }

    // derived values
    float gamma = powf(10.0f, (SNRdB+noise_floor)/10.0f);   // channel gain

    // create objects
    agc_crcf p = agc_crcf_create();
    agc_crcf_set_target(p, etarget);
    agc_crcf_set_decim(p, D);
    agc_crcf_set_bandwidth(p, bt);

    // squelch
    agc_crcf_squelch_deactivate(p);

    unsigned int i;
    float complex x[num_samples];
    float rssi_default[num_samples];
    float rssi_log[num_samples];
    float rssi_exp[num_samples];
    float rssi_true[num_samples];

    // print info
    printf("automatic gain control // target: %8.4f, loop bandwidth: %4.2e\n",etarget,bt);

    // generate signal
    for (i=0; i<num_samples; i++)
        x[i] = gamma * cexpf(_Complex_I*2*M_PI*0.093f*i);

    unsigned int n=0;
    unsigned int n0   = num_samples / 6;
    unsigned int ramp = num_samples / 256;
    unsigned int n1   = num_samples / 2;
    for (i=0; i<n0; i++)    x[n++] *= 0.0f;
    for (i=0; i<ramp; i++)  x[n++] *= 0.5f - 0.5f*cosf(M_PI*i/(float)ramp);
    for (i=0; i<n1; i++)    x[n++] *= 1.0f;
    for (i=0; i<ramp; i++)  x[n++] *= 0.5f + 0.5f*cosf(M_PI*i/(float)ramp);
    while (n < num_samples) x[n++] *= 0.0f;

    // add noise
    float noise_std = powf(10.0f, noise_floor / 10.0f) / sqrtf(2.0f);
    for (i=0; i<num_samples; i++)
        x[i] += noise_std*(randnf() + _Complex_I*randnf());

    // run agc
    float complex y;

    // default
    agc_crcf_reset(p);
    agc_crcf_set_type(p,LIQUID_AGC_DEFAULT);
    for (i=0; i<num_samples; i++) {
        agc_crcf_execute(p, x[i], &y);
        rssi_default[i] = agc_crcf_get_signal_level(p);
    }

    // 
    agc_crcf_reset(p);
    agc_crcf_set_type(p,LIQUID_AGC_LOG);
    for (i=0; i<num_samples; i++) {
        agc_crcf_execute(p, x[i], &y);
        rssi_log[i] = agc_crcf_get_signal_level(p);
    }

    // default
    agc_crcf_reset(p);
    agc_crcf_set_type(p,LIQUID_AGC_EXP);
    for (i=0; i<num_samples; i++) {
        agc_crcf_execute(p, x[i], &y);
        rssi_exp[i] = agc_crcf_get_signal_level(p);
    }

    // true
    agc_crcf_reset(p);
    agc_crcf_set_type(p,LIQUID_AGC_TRUE);
    for (i=0; i<num_samples; i++) {
        agc_crcf_execute(p, x[i], &y);
        rssi_true[i] = agc_crcf_get_signal_level(p);
    }

    agc_crcf_destroy(p);

    // print results to screen
    printf("received signal strength indication (rssi):\n");
    printf("   i    default      log      exp     true\n");
    printf("----   -------- -------- -------- --------\n");
    for (i=0; i<num_samples; i+=d) {
        printf("%4u : %8.2f %8.2f %8.2f %8.2f\n",
            i,
            10*log10f(rssi_default[i]),
            10*log10f(rssi_log[i]),
            10*log10f(rssi_exp[i]),
            10*log10f(rssi_true[i]));
    }

    // open output file
    FILE* fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n",OUTPUT_FILENAME);
    fprintf(fid,"clear all;\nclose all;\n\n");
    fprintf(fid,"n = %u;\n", num_samples);

    for (i=0; i<num_samples; i++) {
        //fprintf(fid,"x(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(x[i]), cimagf(x[i]));
        fprintf(fid,"rssi_default(%4u)  = %12.4e;\n", i+1, rssi_default[i]);
        fprintf(fid,"rssi_log(%4u)      = %12.4e;\n", i+1, rssi_log[i]);
        fprintf(fid,"rssi_exp(%4u)      = %12.4e;\n", i+1, rssi_exp[i]);
        fprintf(fid,"rssi_true(%4u)     = %12.4e;\n", i+1, rssi_true[i]);
    }

    fprintf(fid,"\n\n");
    fprintf(fid,"t = 0:(n-1);\n");
    fprintf(fid,"plot(t,10*log10(rssi_default),'-','LineWidth',1,...\n");
    fprintf(fid,"     t,10*log10(rssi_log),    '-','LineWidth',1,...\n");
    fprintf(fid,"     t,10*log10(rssi_exp),    '-','LineWidth',1,...\n");
    fprintf(fid,"     t,10*log10(rssi_true),   '-','LineWidth',1);");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"xlabel('sample index');\n");
    fprintf(fid,"ylabel('rssi [dB]');\n");
    fprintf(fid,"legend('AGC\\_DEFAULT','AGC\\_LOG','AGC\\_EXP','AGC\\_TRUE',1);\n");
    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);

    printf("done.\n");
    return 0;
}

