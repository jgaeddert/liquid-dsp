//
// math_window.c : demonstrate windowing functions
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <getopt.h>

#include "liquid.h"
#include "liquid.doc.h"

// print usage/help message
void usage()
{
    printf("math_window [options]\n");
    printf("  u/h   : print usage\n");
    printf("  n     : window size, default: 51\n");
    printf("  N     : fft size, default: 1024\n");
    printf("  b     : Kaiser-Bessel beta factor\n");
    printf("  w     : window type {kaiser, hamming, hann, blackmanharris, kbd}\n");
    //printf("  o     : output filename\n");
}

int main(int argc, char*argv[])
{
    // options
    unsigned int n=51;      // window size
    unsigned int nfft=1024; // number of fft points
    float beta = 3.0f;      //
    enum {
        KAISER=0,
        HAMMING,
        HANN,
        BLACKMANHARRIS,
        KBD
    } wtype = KAISER;

    int dopt;
    while((dopt = getopt(argc,argv,"uhn:N:b:w:")) != EOF){
        switch (dopt) {
        case 'h':
        case 'u': usage();              return 0;
        case 'n': n = atoi(optarg);     break;
        case 'N': nfft = atoi(optarg);  break;
        case 'b': beta = atof(optarg);  break;
        case 'w': 
            if      (strcmp(optarg,"kaiser")==0)         wtype = KAISER;
            else if (strcmp(optarg,"hamming")==0)        wtype = HAMMING;
            else if (strcmp(optarg,"hann")==0)           wtype = HANN;
            else if (strcmp(optarg,"blackmanharris")==0) wtype = BLACKMANHARRIS;
            else if (strcmp(optarg,"kbd")==0)            wtype = KBD;
            else {
                fprintf(stderr,"error: %s, unknown/unsupported window '%s'\n", argv[0], optarg);
                exit(1);
            }
            break;
        default:
            fprintf(stderr,"error: %s, invalid option\n", argv[0]);
            exit(1);
        }
    }

    // validate input
    if (beta < 0.0f) {
        fprintf(stderr,"error: %s, beta must be positive\n", argv[0]);
        exit(1);
    }

    // allocate memory for window
    float w[n];
    float complex W[nfft];

    // generate window
    unsigned int i;
    for (i=0; i<n; i++) {
        switch (wtype) {
        case KAISER:         w[i] = kaiser(i,n,beta,0);   break;
        case HAMMING:        w[i] = hamming(i,n);         break;
        case HANN:           w[i] = hann(i,n);            break;
        case BLACKMANHARRIS: w[i] = blackmanharris(i,n);  break;
        case KBD:            w[i] = liquid_kbd(i,n,beta); break;
        default:
            fprintf(stderr,"error: %s, invalid window\n", argv[0]);
            exit(1);
        }
    }
    
    // compute power spectral density
    liquid_doc_compute_psdf(w, n, W, nfft, LIQUID_DOC_PSDWINDOW_NONE, 1);

    // generate filenames
    char wname[256] = "";
    switch (wtype) {
    case KAISER:         sprintf(wname,"kaiser");          break;
    case HAMMING:        sprintf(wname,"hamming");         break;
    case HANN:           sprintf(wname,"hann");            break;
    case BLACKMANHARRIS: sprintf(wname,"blackmanharris");  break;
    case KBD:            sprintf(wname,"kbd");             break;
    default:
        fprintf(stderr,"error: %s, invalid window\n", argv[0]);
        exit(1);
    }
    char filename_time[256] = "";
    sprintf(filename_time, "figures.gen/math_window_%s_time.gnu", wname);
    char filename_freq[256] = "";
    sprintf(filename_freq, "figures.gen/math_window_%s_freq.gnu", wname);


    // print results to file(s)
    FILE * fid = NULL;

    // 
    // export time data
    //
    fid = fopen(filename_time,"w");
    if (!fid) {
        fprintf(stderr,"error: %s, could not open '%s' for writing\n", argv[0], filename_time);
        exit(1);
    }
    fprintf(fid,"# %s: auto-generated file\n\n", filename_time);
    fprintf(fid,"reset\n");
    fprintf(fid,"set terminal postscript eps enhanced color solid rounded\n");
    fprintf(fid,"set xrange [%12.8f:%12.8f];\n", -0.5f*(float)(n-1), 0.5f*(float)(n-1));
    fprintf(fid,"set yrange [0:1]\n");
    fprintf(fid,"set size ratio 1.0\n");
    fprintf(fid,"set xlabel 'Sample Index'\n");
    fprintf(fid,"set ylabel 'temporal window'\n");
    fprintf(fid,"set nokey\n");
    //fprintf(fid,"set ytics -5,1,5\n");
    fprintf(fid,"set grid xtics ytics\n");
    fprintf(fid,"set pointsize 0.6\n");
    fprintf(fid,"set grid linetype 1 linecolor rgb '%s' lw 1\n", LIQUID_DOC_COLOR_GRID);
    fprintf(fid,"set border 0\n");
    //fprintf(fid,"set zeroaxis linewidth 4 linetype 1 linecolor rgb '#000000'\n");
    fprintf(fid,"plot '-' using 1:2 with impulses linetype 1 linewidth 4 linecolor rgb '#000000' notitle\n");

    // print window
    for (i=0; i<n; i++)
        fprintf(fid,"%12.8f %12.4e\n", (float)i - 0.5f*(float)(n-1), w[i]);
    fprintf(fid,"e\n");

    fclose(fid);
    printf("results written to '%s'\n", filename_time);

    // 
    // export spectrum data
    //
    fid = fopen(filename_freq,"w");
    if (!fid) {
        fprintf(stderr,"error: %s, could not open '%s' for writing\n", argv[0], filename_freq);
        exit(1);
    }
    fprintf(fid,"# %s: auto-generated file\n\n", filename_time);
    fprintf(fid,"reset\n");
    fprintf(fid,"set terminal postscript eps enhanced color solid rounded\n");
    //fprintf(fid,"set xrange [0:%u];\n",n);
    fprintf(fid,"set yrange [-0.1:1.1]\n");
    fprintf(fid,"set size ratio 1.0\n");
    fprintf(fid,"set xlabel 'Sample Index'\n");
    fprintf(fid,"set key top right nobox\n");
    //fprintf(fid,"set ytics -5,1,5\n");
    fprintf(fid,"set grid xtics ytics\n");
    fprintf(fid,"set pointsize 0.6\n");
    fprintf(fid,"set grid linetype 1 linecolor rgb '%s' lw 1\n", LIQUID_DOC_COLOR_GRID);
    fprintf(fid,"set xlabel 'Normalized Frequency'\n");
    fprintf(fid,"set ylabel 'Power Spectral Density [dB]'\n");
    fprintf(fid,"set nokey # disable legned\n");
    fprintf(fid,"set grid xtics ytics\n");
    fprintf(fid,"set grid linetype 1 linecolor rgb '%s' lw 1\n", LIQUID_DOC_COLOR_GRID);
    fprintf(fid,"set xrange [-0.5:0.5]\n");
    fprintf(fid,"set yrange [-80:0]\n");
    fprintf(fid,"set border 0\n");
    //fprintf(fid,"set zeroaxis linewidth 4 linetype 1 linecolor rgb '#000000'\n");
    fprintf(fid,"plot '-' using 1:2 with lines linetype 1 linewidth 4 linecolor rgb '#000000' notitle\n");

    // save array to output
    unsigned int k;
    float f[nfft];
    for (i=0; i<nfft; i++)
        f[i] = (float)(i) / (float)nfft - 0.5f;

    fprintf(fid,"# frequency psd\n");

    // spectrum data
    for (i=0; i<nfft; i++) {
        k = (i + nfft/2) % nfft;
        fprintf(fid,"%12.8f %12.4e\n", f[i], 20.0f*log10f(cabsf(W[k])));
    }
    fprintf(fid,"e\n");

    fclose(fid);
    printf("results written to '%s'\n", filename_freq);

    return 0;
}

