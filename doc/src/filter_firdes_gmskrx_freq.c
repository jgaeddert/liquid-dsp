//
// filter_firdes_gmskrx_freq.c : demonstrate fir filter design for GMSK receiver
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <getopt.h>

#include "liquid.h"
#include "liquid.doc.h"

#define OUTPUT_FILENAME_FREQ "figures.gen/filter_firdes_gmskrx_freq.gnu"


// print usage/help message
void usage()
{
    printf("Usage: sandbox/firdes_gmskrx_test [OPTION]\n");
    printf("Run example GMSK receive filter design\n");
    printf("\n");
    printf("  u/h   : print usage/help\n");
    printf("  k     : filter samples/symbol, default: 4\n");
    printf("  m     : filter delay (symbols), m >= 1, default: 3\n");
    printf("  b     : filter excess bandwidth factor, default: 0.4\n");
    printf("  n     : fft size, default: 1024\n");
}

int main(int argc, char*argv[]) {
    // options
    unsigned int k=4;       // samples/symbol
    unsigned int m=4;       // filter delay [symbols]
    float BT = 0.3f;        // bandwidth-time product
    unsigned int nfft=1024; // fft size

    // read properties from command line
    int dopt;
    while ((dopt = getopt(argc,argv,"uhk:m:b:n:")) != EOF) {
        switch (dopt) {
        case 'u':
        case 'h':   usage();                return 0;
        case 'k':   k    = atoi(optarg);    break;
        case 'm':   m    = atoi(optarg);    break;
        case 'b':   BT   = atof(optarg);    break;
        case 'n':   nfft = atof(optarg);    break;
        default:
            exit(1);
        }
    }

    // validate input
    if (k < 2) {
        fprintf(stderr,"error: %s, k must be at least 2\n", argv[0]);
        exit(1);
    } else if (m < 1) {
        fprintf(stderr,"error: %s, m must be at least 1\n", argv[0]);
        exit(1);
    } else if (BT <= 0.0f || BT >= 1.0f) {
        fprintf(stderr,"error: %s, BT must be in (0,1)\n", argv[0]);
        exit(1);
    }

    unsigned int i;

    // derived values
    unsigned int h_len = 2*k*m+1;   // transmit/receive filter length

    // TODO : ensure FFT size is greater than h_len

    // arrays
    float ht[h_len];    // transmit filter coefficients
    float hr[h_len];    // recieve filter coefficients

    // design transmit filter
    liquid_firdes_gmsktx(k,m,BT,0.0f,ht);

    // design receive filter
    liquid_firdes_gmskrx(k,m,BT,0.0f,hr);

    // compute composite filter response
    float hc[4*k*m+1];
    for (i=0; i<4*k*m+1; i++) {
        int lag = (int)i - (int)(2*k*m);
        hc[i] = liquid_filter_crosscorr(ht,h_len, hr,h_len, lag) / k;
    }

    // compute isi
    float rxy0 = liquid_filter_crosscorr(ht,h_len, hr,h_len, 0);
    float isi_rms = 0.0f;
    for (i=1; i<2*m; i++) {
        float e = liquid_filter_crosscorr(ht,h_len, hr,h_len, i*k) / rxy0;
        isi_rms += e*e;
    }
    isi_rms = sqrtf(isi_rms / (float)(2*m-1));
    printf("ISI (RMS) = %12.8f dB\n", 20*log10f(isi_rms));

    // scale impulse responses by samples/symbol
    for (i=0; i<h_len; i++)   ht[i] /= (float)k;    // transmit
    for (i=0; i<h_len; i++)   hr[i] /= (float)k;    // receive
    for (i=0; i<4*k*m+1; i++) hc[i] /= (float)k;    // composite

    // compute filter power spectral density
    float complex Ht[nfft];
    float complex Hr[nfft];
    float complex Hc[nfft];
    liquid_doc_compute_psdf(ht, h_len,   Ht, nfft, LIQUID_DOC_PSDWINDOW_NONE, 0);
    liquid_doc_compute_psdf(hr, h_len,   Hr, nfft, LIQUID_DOC_PSDWINDOW_NONE, 0);
    liquid_doc_compute_psdf(hc, 4*k*m+1, Hc, nfft, LIQUID_DOC_PSDWINDOW_NONE, 0);

    
    // 
    // generate freq-domain plot
    //
    FILE * fid = NULL;

    fid = fopen(OUTPUT_FILENAME_FREQ,"w");
    fprintf(fid,"# %s: auto-generated file\n\n", OUTPUT_FILENAME_FREQ);
    fprintf(fid,"reset\n");
    fprintf(fid,"set terminal postscript eps enhanced color solid rounded\n");
    fprintf(fid,"set xrange [-0.5:0.5];\n");
    fprintf(fid,"set yrange [-100:20]\n");
    fprintf(fid,"set size ratio 0.6\n");
    //fprintf(fid,"set title 'filter design'\n");
    fprintf(fid,"set xlabel 'Normalized Frequency'\n");
    fprintf(fid,"set ylabel 'Power Spectral Density [dB]'\n");
    fprintf(fid,"set key top right nobox\n");
    fprintf(fid,"set grid xtics ytics\n");
    fprintf(fid,"set grid linetype 1 linecolor rgb '%s' lw 1\n", LIQUID_DOC_COLOR_GRID);
    fprintf(fid,"plot '-' using 1:2 with lines linetype 1 linewidth 2 linecolor rgb '%s' title 'Transmit',\\\n", LIQUID_DOC_COLOR_BLUE);
    fprintf(fid,"     '-' using 1:2 with lines linetype 1 linewidth 2 linecolor rgb '%s' title 'Receive', \\\n", LIQUID_DOC_COLOR_GREEN);
    fprintf(fid,"     '-' using 1:2 with lines linetype 1 linewidth 4 linecolor rgb '%s' title 'Composite',\\\n",    LIQUID_DOC_COLOR_RED);
    fprintf(fid,"     '-' using 1:2 with points pointtype 7 pointsize 0.7 linecolor rgb '%s' notitle\n",    LIQUID_DOC_COLOR_RED);

    float eps = 1e-12f;
    float f[nfft];
    for (i=0; i<nfft; i++)
        f[i] = (float)(i) / (float)nfft - 0.5f;

    // transmit filter
    fprintf(fid,"# transmit filter\n");
    for (i=0; i<nfft; i++)
        fprintf(fid,"  %12.8f %12.4e\n", f[i], 20.0f*log10f(eps+cabsf(Ht[(i+nfft/2)%nfft])));
    fprintf(fid,"e\n");

    // receive filter
    fprintf(fid,"# receive filter\n");
    for (i=0; i<nfft; i++)
        fprintf(fid,"  %12.8f %12.4e\n", f[i], 20.0f*log10f(eps+cabsf(Hr[(i+nfft/2)%nfft])));
    fprintf(fid,"e\n");

    // composite filter
    fprintf(fid,"# composite filter\n");
    for (i=0; i<nfft; i++)
        fprintf(fid,"  %12.8f %12.4e\n", f[i], 20.0f*log10f(eps+cabsf(Hc[(i+nfft/2)%nfft])));
    fprintf(fid,"e\n");

    // alias points
    fprintf(fid,"# alias points\n");
    fprintf(fid,"  %12.8f %12.4e\n", -0.5f/(float)k, -20*log10(2.0f));
    fprintf(fid,"  %12.8f %12.4e\n",  0.5f/(float)k, -20*log10(2.0f));
    fprintf(fid,"e\n");

    fclose(fid);
    return 0;
}

