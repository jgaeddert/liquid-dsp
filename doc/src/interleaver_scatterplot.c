//
// interleaver_scatterplot.c
// 
// generate interleaver scatterplot figure
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <assert.h>

#include "liquid.h"
#include "liquid.doc.h"

// print usage/help message
void usage()
{
    printf("interleaver_scatterplot [options]\n");
    printf("  u/h   : print usage\n");
    printf("  g     : specify gnuplot version\n");
    printf("  f     : output gnuplot filename\n");
    printf("  n     : number of bytes, default: 8\n");
    printf("  d     : number of iterations, default: 0\n");
}

// find most significant bit in array (starting from left)
unsigned int interleaver_find_bit(unsigned char * _x,
                                  unsigned int _n);

int main(int argc, char*argv[]) {
    // options
    unsigned int n=8; // message length
    unsigned int depth = 0;
    char filename[256] = "";
    float gnuplot_version=0.0;

    int dopt;
    while ((dopt = getopt(argc,argv,"uhg:f:n:d:")) != EOF) {
        switch (dopt) {
        case 'u':
        case 'h': usage();                          return 0;
        case 'g': gnuplot_version = atof(optarg);   break;
        case 'f': strncpy(filename,optarg,256);     break;
        case 'n': n = atoi(optarg);                 break;
        case 'd': depth = atoi(optarg);    break;
        default:
            exit(1);
        }
    }

    // create the interleaver
    interleaver q = interleaver_create(n);
    interleaver_set_depth(q, depth);

    // create arrays
    unsigned char x[n]; // original message data
    unsigned char y[n]; // interleaved data

    unsigned int index[8*n];

    unsigned int i;
    unsigned int j;
    unsigned int k=0;

    // initialize data
    memset(x, 0, n*sizeof(unsigned char));
    for (i=0; i<n; i++) {
        for (j=0; j<8; j++) {
            x[i] = 1 << (8-j-1);

            // run interleaver and find most significant
            // bit in resulting array
            interleaver_encode(q,x,y);
            index[k] = interleaver_find_bit(y,n);

            // increment index counter
            k++;
        }
        // reset input message sequence
        x[i] = 0;
    }

    assert(k==8*n);

    // destroy the interleaver object
    interleaver_destroy(q);

    // write output file
    FILE * fid = fopen(filename,"w");
    if (fid == NULL) {
        fprintf(stderr,"error: %s, could not open file '%s' for writing.\n", argv[0],filename);
        exit(1);
    }
    // print header
    fprintf(fid,"# %s : auto-generated file (do not edit)\n", filename);
    fprintf(fid,"# invoked as :");
    for (i=0; i<argc; i++)
        fprintf(fid," %s",argv[i]);
    fprintf(fid,"\n");
    fprintf(fid,"reset\n");
    fprintf(fid,"set terminal postscript eps enhanced color solid rounded\n");
    fprintf(fid,"set size square\n");
    //fprintf(fid,"set title \"%s\"\n", figure_title);
    fprintf(fid,"set xrange [0:%u]\n", 8*n);
    fprintf(fid,"set yrange [0:%u]\n", 8*n);
    fprintf(fid,"set xtics 0,%u,%u axis\n",2*n,8*n);
    fprintf(fid,"set ytics 0,%u,%u axis\n",2*n,8*n);
    fprintf(fid,"set xlabel 'Input bit index'\n");
    fprintf(fid,"set ylabel 'Output bit index'\n");
    fprintf(fid,"set nokey # disable legned\n");
    //fprintf(fid,"set grid xtics ytics\n");
    fprintf(fid,"set grid linetype 1 linecolor rgb '%s' linewidth 1\n",LIQUID_DOC_COLOR_GRID);
    fprintf(fid,"set pointsize 1.0\n");
    fprintf(fid,"plot '-' using 1:2 with points pointtype 7 linecolor rgb '%s'\n", LIQUID_DOC_COLOR_PURPLE);

    // export data to file
    for (i=0; i<8*n; i++)
        fprintf(fid,"  %8u    %8u\n", i, index[i]);
    fprintf(fid,"e\n");

    // close it up
    fclose(fid);


    printf("done.\n");
    return 0;
}

// find most significant bit in array (starting from left)
unsigned int interleaver_find_bit(unsigned char * _x,
                                  unsigned int _n)
{
    unsigned int i;
    for (i=0; i<_n; i++) {
        if (_x[i] != 0)
            break;
    }

    unsigned char byte = _x[i];
    // find most significant bit in byte
    switch (byte) {
    case (1<<7):    return 8*i + 0;     // 1000 0000
    case (1<<6):    return 8*i + 1;     // 0100 0000
    case (1<<5):    return 8*i + 2;     // 0010 0000
    case (1<<4):    return 8*i + 3;     // 0001 0000
    case (1<<3):    return 8*i + 4;     // 0000 1000
    case (1<<2):    return 8*i + 5;     // 0000 0100
    case (1<<1):    return 8*i + 6;     // 0000 0010
    case (1<<0):    return 8*i + 7;     // 0000 0001
    default:
        fprintf(stderr,"error: interleaver_find_bid(), invalid array\n");
        exit(1);
    }
}

