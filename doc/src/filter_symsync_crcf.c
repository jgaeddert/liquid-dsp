//
// filter_symsync_crcf.c
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <getopt.h>
#include <time.h>
#include <assert.h>

#include "liquid.h"
#include "liquid.doc.h"

// print usage/help message
void usage()
{
    printf("filter_symsync_crcf [options]\n");
    printf("  h     : print usage\n");
    printf("  f     : base filename, default: 'figures.gen/filter_symsync_crcf'\n");
    printf("  k     : filter samples/symbol, default: 2\n");
    printf("  K     : output samples/symbol, default: 2\n");
    printf("  m     : filter delay (symbols), default: 3\n");
    printf("  b     : filter excess bandwidth, default: 0.5\n");
    printf("  B     : filter polyphase banks, default: 64\n");
    printf("  s     : signal-to-noise ratio, default: 30dB\n");
    printf("  w     : timing pll bandwidth, default: 0.01\n");
    printf("  n     : number of symbols, default: 500\n");
    printf("  t     : timing phase offset [%% symbol], -0.5 < t <= 0.5, default: -0.4\n");
    printf("  r     : timing freq. offset [%% symbol], default: 1.000\n");
}


int main(int argc, char*argv[]) {
    srand(time(NULL));

    // options
    unsigned int k=2;               // samples/symbol (input)
    unsigned int k_out=2;           // samples/symbol (output)
    unsigned int m=4;               // filter delay (symbols)
    float beta=0.3f;                // filter excess bandwidth factor
    unsigned int num_filters=64;    // number of filters in the bank
    unsigned int num_symbols=500;   // number of data symbols
    float SNRdB = 30.0f;            // signal-to-noise ratio
    liquid_rnyquist_type ftype_tx = LIQUID_RNYQUIST_RRC;
    liquid_rnyquist_type ftype_rx = LIQUID_RNYQUIST_RRC;

    float bt=0.01f;     // loop filter bandwidth
    float tau=-0.4f;    // fractional symbol offset
    float r = 1.00f;    // resampled rate
    
    char filename_base[256] = "figures.gen/filter_symsync_crcf";
    
    int dopt;
    while ((dopt = getopt(argc,argv,"hf:k:K:m:b:B:s:w:n:t:r:")) != EOF) {
        switch (dopt) {
        case 'h':   usage();                            return 0;
        case 'f':   strncpy(filename_base,optarg,256);  break;
        case 'k':   k           = atoi(optarg);     break;
        case 'K':   k_out       = atoi(optarg);     break;
        case 'm':   m           = atoi(optarg);     break;
        case 'b':   beta        = atof(optarg);     break;
        case 'B':   num_filters = atoi(optarg);     break;
        case 's':   SNRdB       = atof(optarg);     break;
        case 'w':   bt          = atof(optarg);     break;
        case 'n':   num_symbols = atoi(optarg);     break;
        case 't':   tau         = atof(optarg);     break;
        case 'r':   r           = atof(optarg);     break;
        default:
            exit(1);
        }
    }

    // validate input
    if (k < 2) {
        fprintf(stderr,"error: k (samples/symbol) must be at least 2\n");
        exit(1);
    } else if (m < 1) {
        fprintf(stderr,"error: m (filter delay) must be greater than 0\n");
        exit(1);
    } else if (beta <= 0.0f || beta > 1.0f) {
        fprintf(stderr,"error: beta (excess bandwidth factor) must be in (0,1]\n");
        exit(1);
    } else if (num_filters == 0) {
        fprintf(stderr,"error: number of polyphase filters must be greater than 0\n");
        exit(1);
    } else if (bt <= 0.0f) {
        fprintf(stderr,"error: timing PLL bandwidth must be greater than 0\n");
        exit(1);
    } else if (num_symbols == 0) {
        fprintf(stderr,"error: number of symbols must be greater than 0\n");
        exit(1);
    } else if (tau < -1.0f || tau > 1.0f) {
        fprintf(stderr,"error: timing phase offset must be in [-1,1]\n");
        exit(1);
    } else if (r < 0.5f || r > 2.0f) {
        fprintf(stderr,"error: timing frequency offset must be in [0.5,2]\n");
        exit(1);
    }

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

    // derived values
    unsigned int num_samples = k*num_symbols;
    unsigned int num_samples_resamp = (unsigned int) ceilf(num_samples*r*1.1f) + 4;

    // arrays
    float complex s[num_symbols];           // data symbols
    float complex x[num_samples];           // interpolated samples
    float complex y[num_samples_resamp];    // resampled data (resamp_crcf)
    float complex z[k_out*num_symbols + 64];// synchronized samples
    float complex sym_out[num_symbols + 64];// synchronized symbols

    // random signal (QPSK)
    for (i=0; i<num_symbols; i++) {
        s[i] = ( rand() % 2 ? M_SQRT1_2 : -M_SQRT1_2 ) +
               ( rand() % 2 ? M_SQRT1_2 : -M_SQRT1_2 ) * _Complex_I;
    }

    // 
    // create and run interpolator
    //

    // design interpolating filter
    unsigned int h_len = 2*k*m+1;
    float h[h_len];
    liquid_firdes_rnyquist(ftype_tx,k,m,beta,dt,h);
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
    float nstd = powf(10.0f, -SNRdB/20.0f);
    for (i=0; i<num_samples_resampled; i++)
        y[i] += nstd*(randnf() + _Complex_I*randnf());


    // 
    // create and run symbol synchronizer
    //

    symsync_crcf d = symsync_crcf_create_rnyquist(ftype_rx, k, m, beta, num_filters);
    symsync_crcf_set_lf_bw(d,bt);
    symsync_crcf_set_output_rate(d,k_out);

    unsigned int num_samples_sync=0;
    unsigned int nn;
    unsigned int num_symbols_sync = 0;
    float tau_hat[num_samples];
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
    for (i=num_symbols_sync-10; i<num_symbols_sync; i++)
        printf("  sym_out(%2u) = %8.4f + j*%8.4f;\n", i+1, crealf(sym_out[i]), cimagf(sym_out[i]));

    // 
    // export output
    //
    FILE * fid = NULL;
    char filename[300];

    // 
    // const: constellation
    //
    strncpy(filename, filename_base, 256);
    strcat(filename, "_const.gnu");
    fid = fopen(filename,"w");
    if (!fid) {
        fprintf(stderr,"error: %s, could not open file '%s' for writing\n", argv[0], filename);
        return 1;
    }
    fprintf(fid,"# %s: auto-generated file\n\n", filename);
    fprintf(fid,"reset\n");
    fprintf(fid,"set terminal postscript eps enhanced color solid rounded\n");
    fprintf(fid,"set size ratio 1\n");
    fprintf(fid,"set xrange [-1.5:1.5];\n");
    fprintf(fid,"set yrange [-1.5:1.5];\n");
    fprintf(fid,"set xlabel 'In-phase'\n");
    fprintf(fid,"set ylabel 'Quadrature phase'\n");
    fprintf(fid,"set grid xtics ytics\n");
    fprintf(fid,"set grid linetype 1 linecolor rgb '%s' linewidth 1\n",LIQUID_DOC_COLOR_GRID);
    fprintf(fid,"plot '-' using 1:2 with points pointtype 7 pointsize 0.5 linecolor rgb '%s' title 'first %u symbols',\\\n", LIQUID_DOC_COLOR_GRAY, num_symbols/2);
    fprintf(fid,"     '-' using 1:2 with points pointtype 7 pointsize 0.7 linecolor rgb '%s' title 'last %u symbols'\n",     LIQUID_DOC_COLOR_RED,  num_symbols/2);

    // first half of symbols
    for (i=2*m; i<num_symbols_sync/2; i++)
        fprintf(fid,"  %12.4e %12.4e\n", crealf(sym_out[i]), cimagf(sym_out[i]));
    fprintf(fid,"e\n");

    // second half of symbols
    for ( ; i<num_symbols_sync; i++)
        fprintf(fid,"  %12.4e %12.4e\n", crealf(sym_out[i]), cimagf(sym_out[i]));
    fprintf(fid,"e\n");

    fclose(fid);
    printf("results written to '%s'\n", filename);


    //
    // time series
    //
    strncpy(filename, filename_base, 256);
    strcat(filename, "_time.gnu");
    fid = fopen(filename,"w");
    if (!fid) {
        fprintf(stderr,"error: %s, could not open file '%s' for writing\n", argv[0], filename);
        return 1;
    }
    fprintf(fid,"# %s: auto-generated file\n\n", filename);
    fprintf(fid,"reset\n");
    fprintf(fid,"set terminal postscript eps enhanced color solid rounded\n");
    fprintf(fid,"set xrange [0:%u];\n",num_symbols);
    fprintf(fid,"set yrange [-1.5:1.5]\n");
    fprintf(fid,"set size ratio 0.3\n");
    fprintf(fid,"set xlabel 'Symbol Index'\n");
    fprintf(fid,"set key top right nobox\n");
    //fprintf(fid,"set ytics -5,1,5\n");
    fprintf(fid,"set grid xtics ytics\n");
    fprintf(fid,"set pointsize 0.6\n");
    fprintf(fid,"set grid linetype 1 linecolor rgb '%s' lw 1\n", LIQUID_DOC_COLOR_GRID);
    fprintf(fid,"set multiplot layout 2,1 scale 1.0,1.0\n");

    // real
    fprintf(fid,"# real\n");
    fprintf(fid,"set ylabel 'Real'\n");
    fprintf(fid,"plot '-' using 1:2 with lines linetype 1 linewidth 1 linecolor rgb '#999999' notitle,\\\n");
    fprintf(fid,"     '-' using 1:2 with points pointtype 7 linecolor rgb '%s' notitle'\n", LIQUID_DOC_COLOR_BLUE);
    // 
    for (i=0; i<num_samples_sync; i++)
        fprintf(fid,"%12.8f %12.4e\n", (float)i/(float)k_out, crealf(z[i]));
    fprintf(fid,"e\n");
    // 
    for (i=0; i<num_samples_sync; i+=k)
        fprintf(fid,"%12.8f %12.4e\n", (float)i/(float)k_out, crealf(z[i]));
    fprintf(fid,"e\n");

    // imag
    fprintf(fid,"# imag\n");
    fprintf(fid,"set ylabel 'Imag'\n");
    fprintf(fid,"plot '-' using 1:2 with lines linetype 1 linewidth 1 linecolor rgb '#999999' notitle,\\\n");
    fprintf(fid,"     '-' using 1:2 with points pointtype 7 linecolor rgb '%s' notitle'\n", LIQUID_DOC_COLOR_GREEN);
    // 
    for (i=0; i<num_samples_sync; i++)
        fprintf(fid,"%12.8f %12.4e\n", (float)i/(float)k_out, cimagf(z[i]));
    fprintf(fid,"e\n");
    // 
    for (i=0; i<num_samples_sync; i+=k)
        fprintf(fid,"%12.8f %12.4e\n", (float)i/(float)k_out, cimagf(z[i]));
    fprintf(fid,"e\n");

    fprintf(fid,"unset multiplot\n");

    // close output file
    fclose(fid);
    printf("results written to '%s'\n", filename);

    // clean it up
    return 0;
}
