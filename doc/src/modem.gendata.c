// modem.gendata.c
//
// Generates a gnuplot data file for exporting a modem figure.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>

#include "liquid.h"

// print usage/help message
void usage()
{
    printf("modem.gendata [options]\n");
    printf("  u/h   : print usage\n");
    printf("  g     : specify gnuplot version\n");
    printf("  f     : specify output filename\n");
    printf("  m     : modulation scheme (psk default)\n");
    liquid_print_modulation_schemes();
}

int main(int argc, char*argv[]) {
    // create mod/demod objects
    modulation_scheme ms = LIQUID_MODEM_QPSK;
    unsigned int gnuplot_version=0;
    char filename[256];
    strcpy(filename,"");

    int dopt;
    while ((dopt = getopt(argc,argv,"uhg:f:m:")) != EOF) {
        switch (dopt) {
        case 'u':
        case 'h':
            usage();
            return 0;
        case 'g':
            gnuplot_version = atoi(optarg);
            break;
        case 'f':
            strncpy(filename,optarg,256);
            break;
        case 'm':
            ms = liquid_getopt_str2mod(optarg);
            if (ms == LIQUID_MODEM_UNKNOWN) {
                fprintf(stderr,"error: %s, unknown/unsupported modulation scheme \"%s\"\n", argv[0], optarg);
                return 1;
            }
            break;
        default:
            exit(1);
        }
    }

    // create the modem and generate constellation
    modem mod = modem_create(ms);
    unsigned int bps = modem_get_bps(mod);

    unsigned int i; // modulated symbol
    unsigned int num_symbols = 1<<bps;
    float complex x;
    unsigned int s;
    char symbol_str[bps+1];

    FILE * fid = fopen(filename,"w");
    if (fid == NULL) {
        fprintf(stderr,"error: %s, could not open file \"%s\" for writing.\n", argv[0], filename);
        exit(1);
    }
    // print header
    fprintf(fid,"# %s : auto-generated file (do not edit)\n", filename);
    fprintf(fid,"# invoked as :");
    for (i=0; i<argc; i++)
        fprintf(fid," %s",argv[i]);
    fprintf(fid,"\n");

    for (i=0; i<num_symbols; i++) {
        modem_modulate(mod, i, &x);

        // generate binary symbol string
        unsigned int j;
        s = i;
        for (j=0; j<bps; j++) {
            symbol_str[bps-j-1] = (s & 1) ? '1' : '0';
            s >>= 1;
        }
        symbol_str[j] = '\0';   // terminate with null character

        fprintf(fid,"%12.4e %12.4e \"%s\"  \"%u\"\n", crealf(x), cimagf(x), symbol_str, i);
    }

    // close output file
    fclose(fid);

    // clean up modem object
    modem_destroy(mod);


    return 0;
}
