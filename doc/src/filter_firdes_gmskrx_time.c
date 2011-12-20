//
// filter_firdes_gmskrx_time.c : demonstrate fir filter design for GMSK receiver
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <getopt.h>

#include "liquid.h"
#include "liquid.doc.h"

#define OUTPUT_FILENAME_TIME "figures.gen/filter_firdes_gmskrx_time.gnu"

// print usage/help message
void usage()
{
    printf("Usage: sandbox/firdes_gmskrx_test [OPTION]\n");
    printf("Run example GMSK receive filter design\n");
    printf("\n");
    printf("  u/h   : print usage/help\n");
    printf("  k     : filter samples/symbol (time), default: 16\n");
    printf("  m     : filter delay (symbols), m >= 1, default: 3\n");
    printf("  b     : filter excess bandwidth factor, default: 0.4\n");
}

int main(int argc, char*argv[]) {
    // options
    unsigned int k=16;      // samples/symbol (time)
    unsigned int m=5;       // filter delay [symbols]
    float BT = 0.4f;        // bandwidth-time product

    // read properties from command line
    int dopt;
    while ((dopt = getopt(argc,argv,"uhk:m:b:")) != EOF) {
        switch (dopt) {
        case 'u':
        case 'h':   usage();            return 0;
        case 'k':   k  = atoi(optarg);  break;
        case 'm':   m  = atoi(optarg);  break;
        case 'b':   BT = atof(optarg);  break;
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
    
    // print results to file(s)
    FILE * fid = NULL;

    // 
    // generate time-domain plot
    //

    // open/initialize output file
    fid = fopen(OUTPUT_FILENAME_TIME,"w");
    fprintf(fid,"# %s: auto-generated file\n\n", OUTPUT_FILENAME_TIME);
    fprintf(fid,"reset\n");
    fprintf(fid,"set terminal postscript eps enhanced color solid rounded\n");
    //fprintf(fid,"set xrange [0:%u];\n",n);
    //fprintf(fid,"set yrange [-0.4:1.1]\n");
    fprintf(fid,"set size ratio 0.3\n");
    fprintf(fid,"set xlabel 'Time [t/T]'\n");
    fprintf(fid,"set key top right nobox\n");
    //fprintf(fid,"set ytics -5,1,5\n");
    fprintf(fid,"set grid xtics ytics\n");
    fprintf(fid,"set pointsize 0.6\n");
    fprintf(fid,"set grid linetype 1 linecolor rgb '%s' lw 1\n", LIQUID_DOC_COLOR_GRID);
    fprintf(fid,"set multiplot layout 2,1 scale 1.0,1.0\n");

    fprintf(fid,"# prototype\n");
    fprintf(fid,"set ylabel 'Transmit/Receive'\n");
    fprintf(fid,"set xrange [-%u:%u]\n", m, m);
    fprintf(fid,"plot '-' using 1:2 with lines linetype 1 linewidth 2 linecolor rgb '%s' title 'Transmit',\\\n", LIQUID_DOC_COLOR_BLUE);
    fprintf(fid,"     '-' using 1:2 with lines linetype 1 linewidth 2 linecolor rgb '%s' title 'Receive'\n", LIQUID_DOC_COLOR_GREEN);

    // transmit
    for (i=0; i<h_len; i++)
        fprintf(fid,"%12.8f %12.4e\n", (float)i/(float)k - (float)m, ht[i]);
    fprintf(fid,"e\n");

    // receive
    for (i=0; i<h_len; i++)
        fprintf(fid,"%12.8f %12.4e\n", (float)i/(float)k - (float)m, hr[i]);
    fprintf(fid,"e\n");


    fprintf(fid,"# filter\n");
    fprintf(fid,"set ylabel 'Composite'\n");
    fprintf(fid,"set xrange [-%u:%u]\n", m, m);
    fprintf(fid,"plot '-' using 1:2 with lines linetype 1 linewidth 3 linecolor rgb '%s' title 'Composite',\\\n", LIQUID_DOC_COLOR_RED);
    fprintf(fid,"     '-' using 1:2 with points pointtype 7 pointsize 0.9 linecolor rgb '%s' title ''\n", LIQUID_DOC_COLOR_RED);
    // composite filter
    for (i=0; i<4*k*m+1; i++)
        fprintf(fid,"%12.8f %12.4e\n", (float)i/(float)k - (float)(2*m), hc[i]);
    fprintf(fid,"e\n");
    // composite filter (alias points)
    for (i=0; i<4*k*m+1; i+=k)
        fprintf(fid,"%12.8f %12.4e\n", (float)i/(float)k - (float)(2*m), hc[i]);
    fprintf(fid,"e\n");

    fprintf(fid,"unset multiplot\n");
    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME_TIME);
    return 0;
}

