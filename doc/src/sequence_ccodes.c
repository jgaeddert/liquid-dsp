//
// sequence_ccodes.c
// 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "liquid.h"
#include "liquid.doc.h"

// print usage/help message
void usage()
{
    printf("complementary_codes_example [options]\n");
    printf("  u/h   : print usage\n");
    printf("  n     : sequence length, 8,16,32,64,... (default: 32)\n");
    printf("  f     : output filename\n");
}


int main(int argc, char*argv[])
{
    // options
    unsigned int n = 32;
    char filename[64] = ""; // output filename

    unsigned int i;
    int dopt;
    while ((dopt = getopt(argc,argv,"uhn:f:")) != EOF) {
        switch (dopt) {
        case 'u':
        case 'h':   usage();                return 0;
        case 'n':   n = atoi(optarg);       break;
        case 'f':
            // copy output filename string
            strncpy(filename,optarg,64);
            filename[63] = '\0';
            break;
        default:
            exit(1);
        }
    }

    // validate input
    if ( (1<<liquid_nextpow2(n)) != n ) {
        fprintf(stderr,"error: %s, sequence length must be a power of 2\n", argv[0]);
        exit(1);
    }

    // ensure output filename is set
    if (strcmp(filename,"")==0) {
        fprintf(stderr,"error: %s, filename not set\n", argv[0]);
        exit(1);
    }

    // create and initialize codes
    bsequence a = bsequence_create(n);
    bsequence b = bsequence_create(n);
    bsequence_create_ccodes(a, b);

    // print
    bsequence_print(a);
    bsequence_print(b);

    // generate test sequences
    bsequence ax = bsequence_create(n);
    bsequence bx = bsequence_create(n);
    bsequence_create_ccodes(ax, bx);

    // compute auto-correlations
    int raa[n];
    int rbb[n];
    for (i=0; i<n; i++) {
        raa[i] = 2*bsequence_correlate(a,ax) - n;
        rbb[i] = 2*bsequence_correlate(b,bx) - n;

        bsequence_circshift(ax);
        bsequence_circshift(bx);
    }

    // print results to file
    FILE * fid = fopen(filename,"w");
    if (!fid) {
        fprintf(stderr,"error: %s, cannot open output file '%s' for writing\n", argv[0], filename);
        exit(1);
    }

    // print header
    fprintf(fid,"# %s : auto-generated file (do not edit)\n", filename);
    fprintf(fid,"# invoked as :");
    for (i=0; i<argc; i++)
        fprintf(fid," %s",argv[i]);
    fprintf(fid,"reset\n");
    fprintf(fid,"set terminal postscript eps enhanced color solid rounded\n");
    fprintf(fid,"set xrange [-1:%u];\n", n+1);
    fprintf(fid,"set size ratio 0.17\n");
    fprintf(fid,"set xlabel 'delay (number of samples)'\n");
    //fprintf(fid,"set grid xtics ytics\n");
    //fprintf(fid,"set grid linetype 1 linecolor rgb '%s' lw 1\n", LIQUID_DOC_COLOR_GRID);
    fprintf(fid,"set multiplot layout 3,1 scale 1.0,1.0\n");

    // a
    fprintf(fid,"# sequence a\n");
    fprintf(fid,"set ylabel 'a'\n");
    fprintf(fid,"set yrange [-0.1:1.1]\n");
    fprintf(fid,"set nokey # disable legned\n");
    fprintf(fid,"plot '-' using 1:2 with steps linetype 1 linewidth 4 linecolor rgb '%s'\n",LIQUID_DOC_COLOR_BLUE);
    for (i=0; i<n; i++) {
        fprintf(fid,"  %6u %6u\n", i, bsequence_index(a,n-i-1));
    }
    fprintf(fid,"e\n");

    // b
    fprintf(fid,"# sequence b\n");
    fprintf(fid,"set ylabel 'b'\n");
    fprintf(fid,"set yrange [-0.1:1.1]\n");
    fprintf(fid,"set nokey # disable legned\n");
    fprintf(fid,"plot '-' using 1:2 with steps linetype 1 linewidth 4 linecolor rgb '%s'\n",LIQUID_DOC_COLOR_PURPLE);
    for (i=0; i<n; i++) {
        fprintf(fid,"  %6u %6u\n", i, bsequence_index(b,n-i-1));
    }
    fprintf(fid,"e\n");

    fprintf(fid,"# auto-correlation\n");
    fprintf(fid,"set ylabel 'auto-correlation'\n");
    fprintf(fid,"set yrange [-0.5:1.1]\n");
    fprintf(fid,"set key\n");
    fprintf(fid,"plot '-' using 1:2 with lines linetype 1 linewidth 2 linecolor rgb '%s' title 'raa',\\\n",LIQUID_DOC_COLOR_BLUE);
    fprintf(fid,"     '-' using 1:2 with lines linetype 1 linewidth 2 linecolor rgb '%s' title 'rbb',\\\n",LIQUID_DOC_COLOR_PURPLE);
    fprintf(fid,"     '-' using 1:2 with lines linetype 1 linewidth 4 linecolor rgb '%s' title '(raa+rbb)/2'\n",LIQUID_DOC_COLOR_GREEN);
    // raa
    for (i=0; i<n; i++) {
        fprintf(fid,"  %6u %12.4e\n", i, (float)raa[i] / (float)n);
    }
    fprintf(fid,"e\n");

    // rbb
    for (i=0; i<n; i++) {
        fprintf(fid,"  %6u %12.4e\n", i, (float)rbb[i] / (float)n);
    }
    fprintf(fid,"e\n");

    // [raa + rbb]/2
    for (i=0; i<n; i++) {
        fprintf(fid,"  %6u %12.4e\n", i, (float)(raa[i]+rbb[i]) / (float)(2*n));
    }
    fprintf(fid,"e\n");

    fprintf(fid,"unset multiplot\n");

    fclose(fid);
    printf("results written to %s.\n", filename);

    // clean up memory
    bsequence_destroy(a);
    bsequence_destroy(b);
    bsequence_destroy(ax);
    bsequence_destroy(bx);

    return 0;
}

