//
// symsynclp_crcf_example.c
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <getopt.h>
#include <time.h>
#include <assert.h>

#include "liquid.experimental.h"

#define OUTPUT_FILENAME "symsynclp_crcf_example.m"

// print usage/help message
void usage()
{
    printf("symsynclp_crcf_example [options]\n");
    printf("  u/h   : print usage\n");
    printf("  k     : filter samples/symbol, default: 2\n");
    printf("  m     : filter delay (symbols), default: 3\n");
    printf("  b     : filter excess bandwidth, default: 0.3\n");
    printf("  o     : filter polynomial order (1,2,3), default: 2\n");
    printf("  s     : signal-to-noise ratio, default: 30dB\n");
    printf("  w     : timing pll bandwidth, default: 0.02\n");
    printf("  n     : number of symbols, default: 1024\n");
    printf("  t     : timing phase offset [%% symbol], -0.5 < t <= 0.5, default: 0\n");
    printf("  r     : timing freq. offset [%% symbol], default: 1.000\n");
}


int main(int argc, char*argv[]) {
    srand(time(NULL));

    // options
    unsigned int k=2;   // samples/symbol
    unsigned int m=3;   // filter delay (symbols)
    float beta=0.9f;    // filter excess bandwidth factor
    unsigned int order=2;
    unsigned int num_symbols=1024;
    float SNRdB = 30.0f;

    float bt=0.02f;     // loop filter bandwidth
    float tau=0.2f;     // fractional symbol offset
    float r = 1.00f;    // resampled rate
    
    // use random data or 101010 phasing pattern
    int random_data=1;

    int dopt;
    while ((dopt = getopt(argc,argv,"uhk:m:b:o:s:w:n:t:r:")) != EOF) {
        switch (dopt) {
        case 'u':
        case 'h':   usage();    return 0;
        case 'k':   k = atoi(optarg);               break;
        case 'm':   m = atoi(optarg);               break;
        case 'b':   beta = atof(optarg);            break;
        case 'o':   order = atoi(optarg);           break;
        case 's':   SNRdB = atof(optarg);           break;
        case 'w':   bt = atof(optarg);              break;
        case 'n':   num_symbols = atoi(optarg);     break;
        case 't':   tau = atof(optarg);             break;
        case 'r':   r = atof(optarg);               break;
        default:
            exit(1);
        }
    }

    // validate input
    if (k < 2) {
        fprintf(stderr,"error: k (samples/symbol) must be at least 2\n");
        return 1;
    } else if (m < 1) {
        fprintf(stderr,"error: m (filter delay) must be greater than 0\n");
        return 1;
    } else if (beta <= 0.0f || beta > 1.0f) {
        fprintf(stderr,"error: beta (excess bandwidth factor) must be in (0,1]\n");
        return 1;
    } else if (order == 0) {
        fprintf(stderr,"error: number of polyphase filters must be greater than 0\n");
        return 1;
    } else if (bt <= 0.0f) {
        fprintf(stderr,"error: timing PLL bandwidth must be greater than 0\n");
        return 1;
    } else if (num_symbols == 0) {
        fprintf(stderr,"error: number of symbols must be greater than 0\n");
        return 1;
    } else if (tau < -1.0f || tau > 1.0f) {
        fprintf(stderr,"error: timing phase offset must be in [-1,1]\n");
        return 1;
    } else if (r < 0.5f || r > 2.0f) {
        fprintf(stderr,"error: timing frequency offset must be in [0.5,2]\n");
        return 1;
    }

    // compute delay
    while (tau < 0) tau += 1.0f;    // ensure positive tau
    float g = k*tau;                // number of samples offset
    int ds=floorf(g);               // additional symbol delay
    float dt = (g - (float)ds);     // fractional sample offset

    unsigned int i, n=0;

    unsigned int num_samples = k*num_symbols;
    unsigned int num_samples_resamp = (unsigned int) ceilf(num_samples*r*1.1f) + 4;
    float complex s[num_symbols];           // data symbols
    float complex x[num_samples];           // interpolated samples
    float complex y[num_samples_resamp];    // resampled data (resamp_crcf)
    float complex z[num_symbols + 64];      // synchronized symbols

    for (i=0; i<num_symbols; i++) {
        if (random_data) {
            // random signal (QPSK)
            s[i]  = cexpf(_Complex_I*0.5f*M_PI*((rand() % 4) + 0.5f));
        } else {
            s[i] = (i%2) ? 1.0f : -1.0f;  // 101010 phasing pattern
        }
    }

    // 
    // create and run interpolator
    //

    // design interpolating filter
    unsigned int h_len = 2*k*m + 1;
    float h[h_len];
    liquid_firdes_rcos(k,m,beta,dt,h);
    interp_crcf q = interp_crcf_create(k,h,h_len);
    for (i=0; i<num_symbols; i++) {
        interp_crcf_execute(q, s[i], &x[n]);
        n+=k;
    }
    assert(n == num_samples);
    interp_crcf_destroy(q);

    // 
    // run resampler
    //
    unsigned int resamp_len = 10*k; // resampling filter semi-length (filter delay)
    float resamp_bw = 0.45f;        // resampling filter bandwidth
    float resamp_As = 60.0f;        // resampling filter stop-band attenuation
    unsigned int resamp_npfb = 64;  // number of filters in bank
    resamp_crcf f = resamp_crcf_create(r, resamp_len, resamp_bw, resamp_As, resamp_npfb);
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
    float nstd = powf(10.0f, -SNRdB/20.0f) / sqrtf(2.0f);
    for (i=0; i<num_samples_resampled; i++)
        y[i] += nstd*(randnf() + _Complex_I*randnf());


    // 
    // create and run symbol synchronizer
    //

    // create symbol synchronizer
    symsynclp_crcf d = symsynclp_crcf_create(k, order);
    symsynclp_crcf_set_lf_bw(d,bt);

    unsigned int num_symbols_sync=0;
    unsigned int nn;
    float tau_hat[num_samples];
    for (i=ds; i<num_samples_resampled; i++) {
        tau_hat[num_symbols_sync] = symsynclp_crcf_get_tau(d);
        symsynclp_crcf_execute(d, &y[i], 1, &z[num_symbols_sync], &nn);
        num_symbols_sync += nn;
    }
    symsynclp_crcf_destroy(d);


    // print last several symbols to screen
    printf("z(t) :\n");
    for (i=num_symbols_sync-10; i<num_symbols_sync; i++)
        printf("  z(%2u) = %8.4f + j*%8.4f;\n", i+1, crealf(z[i]), cimagf(z[i]));

    //
    // export output file
    //

    FILE* fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s, auto-generated file\n\n", OUTPUT_FILENAME);
    fprintf(fid,"close all;\nclear all;\n\n");

    fprintf(fid,"k=%u;\n",k);
    fprintf(fid,"m=%u;\n",m);
    fprintf(fid,"beta=%12.8f;\n",beta);
    fprintf(fid,"order=%u;\n",order);
    fprintf(fid,"num_symbols=%u;\n",num_symbols);

    for (i=0; i<h_len; i++)
        fprintf(fid,"h(%3u) = %12.5f;\n", i+1, h[i]);

    for (i=0; i<num_symbols; i++)
        fprintf(fid,"s(%3u) = %12.8f + j*%12.8f;\n", i+1, crealf(s[i]), cimagf(s[i]));

    for (i=0; i<num_samples; i++)
        fprintf(fid,"x(%3u) = %12.8f + j*%12.8f;\n", i+1, crealf(x[i]), cimagf(x[i]));
        
    for (i=0; i<num_samples_resampled; i++)
        fprintf(fid,"y(%3u) = %12.8f + j*%12.8f;\n", i+1, crealf(y[i]), cimagf(y[i]));
        
    for (i=0; i<num_symbols_sync; i++)
        fprintf(fid,"z(%3u) = %12.8f + j*%12.8f;\n", i+1, crealf(z[i]), cimagf(z[i]));
        
    for (i=0; i<num_symbols_sync; i++)
        fprintf(fid,"tau_hat(%3u) = %12.8f;\n", i+1, tau_hat[i]);


    fprintf(fid,"\n\n");
    fprintf(fid,"ms = 8; %% marker size\n");
    fprintf(fid,"zp = filter(h,1,y);\n");
    fprintf(fid,"figure;\nhold on;\n");
    fprintf(fid,"plot([0:length(s)-1],          real(s),    'ob', 'MarkerSize',ms);\n");
    fprintf(fid,"plot([0:length(y)-1]/k  -m,    real(y),    '-',  'MarkerSize',ms, 'Color',[0.8 0.8 0.8]);\n");
    fprintf(fid,"plot([0:length(zp)-1]/k -k*m,  real(zp/k), '-b', 'MarkerSize',ms);\n");
    fprintf(fid,"plot([0:length(z)-1]    -k*m+1,real(z),    'xr', 'MarkerSize',ms);\n");
    fprintf(fid,"hold off;\n");
    fprintf(fid,"xlabel('Symbol Index');\n");
    fprintf(fid,"ylabel('Output Signal');\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"legend('sym in','interp','mf','sym out',0);\n");

    fprintf(fid,"t0=1:floor(0.25*length(z));\n");
    fprintf(fid,"t1=ceil(0.25*length(z)):length(z);\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"hold on;\n");
    fprintf(fid,"plot(real(z(t0)),imag(z(t0)),'x','MarkerSize',ms,'Color',[0.6 0.6 0.6]);\n");
    fprintf(fid,"plot(real(z(t1)),imag(z(t1)),'x','MarkerSize',ms,'Color',[0 0.25 0.5]);\n");
    fprintf(fid,"hold off;\n");
    fprintf(fid,"axis square; grid on;\n");
    fprintf(fid,"axis([-1 1 -1 1]*1.2);\n");
    fprintf(fid,"xlabel('In-phase');\n");
    fprintf(fid,"ylabel('Quadrature');\n");
    fprintf(fid,"legend(['first 25%%'],['last 75%%'],1);\n");

    fprintf(fid,"figure;\n");
    fprintf(fid,"tt = 0:(length(tau_hat)-1);\n");
    fprintf(fid,"plot(tt,tau_hat,'-k','Color',[0 0 0]);\n");
    fprintf(fid,"xlabel('time');\n");
    fprintf(fid,"ylabel('tau-hat');\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"axis([0 length(tau_hat) 0 1]);\n");
    fclose(fid);

    printf("results written to %s.\n", OUTPUT_FILENAME);

    // clean it up
    printf("done.\n");
    return 0;
}
