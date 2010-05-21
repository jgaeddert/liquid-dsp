//
// interleaver_scatterplot_example.c
// 
// generate interleaver scatterplot figure
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <assert.h>

#include "liquid.h"

#define OUTPUT_FILENAME "interleaver_scatterplot_example.m"

// print usage/help message
void usage()
{
    printf("interleaver_scatterplot_example [options]\n");
    printf("  u/h   : print usage\n");
    printf("  n     : number of bytes, default: 8\n");
    printf("  i     : number of iterations, default: 0\n");
    printf("  t     : interleaver type, 'sequence' or 'block'\n");
}

// find most significant bit in array (starting from left)
unsigned int interleaver_find_bit(unsigned char * _x,
                                  unsigned int _n);

int main(int argc, char*argv[]) {
    // options
    unsigned int n=8; // message length
    unsigned int num_iterations = 0;
    interleaver_type type = LIQUID_INTERLEAVER_SEQUENCE; // interleaver type

    int dopt;
    while ((dopt = getopt(argc,argv,"uhn:i:t:")) != EOF) {
        switch (dopt) {
        case 'u':
        case 'h': usage();                          return 0;
        case 'n': n = atoi(optarg);                 break;
        case 'i': num_iterations = atoi(optarg);    break;
        case 't':
            if ( strcmp(optarg,"sequence")==0 ) {
                type = LIQUID_INTERLEAVER_SEQUENCE;
                break;
            } else if ( strcmp(optarg,"block")==0 ) {
                type = LIQUID_INTERLEAVER_BLOCK;
                break;
            } else {
                fprintf(stderr,"error: %s, unsupported type '%s'\n", argv[0], optarg);
                usage();
                return 1;
            }
        default:
            fprintf(stderr,"error: %s, unknown/unsupported option '%c'\n", argv[0], dopt);
            usage();
            return 1;
        }
    }

    // create the interleaver
    interleaver q = interleaver_create(n, type);
    interleaver_set_num_iterations(q, num_iterations);

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
    FILE * fid = fopen(OUTPUT_FILENAME,"w");
    if (fid == NULL) {
        fprintf(stderr,"error: %s, could not open file '%s' for writing.\n", argv[0],OUTPUT_FILENAME);
        exit(1);
    }
    // print header
    fprintf(fid,"%% %s : auto-generated file (do not edit)\n", OUTPUT_FILENAME);
    fprintf(fid,"%% invoked as :");
    for (i=0; i<argc; i++)
        fprintf(fid," %s",argv[i]);
    fprintf(fid,"\n");
    fprintf(fid,"clear all\n");
    fprintf(fid,"close all\n");
    fprintf(fid,"n = %u;\n", 8*n);

    // export data to file
    for (i=0; i<8*n; i++)
        fprintf(fid,"index(%6u) = %6u;\n", i+1, index[i]);
    fprintf(fid,"figure;\n");
    fprintf(fid,"i = [0:(n-1)];\n");
    fprintf(fid,"plot(i/n,index/n,'x');\n");
    fprintf(fid,"axis square\n");
    fprintf(fid,"axis([0 1 0 1]);\n");

    // close it up
    fclose(fid);

    printf("results written to '%s'\n", OUTPUT_FILENAME);

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

