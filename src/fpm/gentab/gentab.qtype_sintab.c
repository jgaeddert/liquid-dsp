/*
 * Copyright (c) 2008, 2009, 2010, 2011, 2012 Joseph Gaeddert
 * Copyright (c) 2008, 2009, 2010, 2011, 2012 Virginia Polytechnic
 *                                Institute & State University
 *
 * This file is part of liquid.
 *
 * liquid is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * liquid is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with liquid.  If not, see <http://www.gnu.org/licenses/>.
 */

//
// gentab.qtype_sintab.c
//
// Table/constant look-up table for sine wave
//

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "liquidfpm.internal.h"

void usage(void)
{
    printf("%s usage:\n", __FILE__);
    printf("  u/h   :   print this help file\n");
    printf("    n   :   length (e.g. 32)\n");
}

int main(int argc, char * argv[])
{
    // initialize variables, set defaults
    unsigned int tabsize = 256;
    unsigned int n = 32;

    // read options
    int dopt;
    while ((dopt = getopt(argc,argv,"uhn:")) != EOF) {
        switch (dopt) {
        case 'u':
        case 'h':   usage();            return 0;
        case 'n':   n = atoi(optarg);   break;
        default:
            fprintf(stderr,"error: %s, unknown option\n", argv[0]);
            return 1;
        }
    }

    char qtype[64];
    
    switch (n) {
    case 16:    strcpy(qtype,"q16");    break;
    case 32:    strcpy(qtype,"q32");    break;
    default:
        fprintf(stderr,"error: %s, invalid size (%u), must be 16,32\n", argv[0], n);
        exit(1);
    }

    unsigned int i;

    // generate floating-point table
    double sintab[tabsize];
    for (i=0; i<tabsize; i++)
        sintab[i] = sin( M_PI_2 * ((double)i)/((double)tabsize) );

    // generate header
    unsigned int values_per_line = 128 / n;

    printf("// auto-generated file (do not edit)\n");
    printf("// invoked as  :   ");
    for (i=0; i<argc; i++)
        printf("%s ", argv[i]);
    printf("\n\n");

    printf("#include \"liquidfpm.internal.h\"\n\n");

    printf("// sine look-up table\n");
    printf("const %s_t %s_sintab[%u] = {\n    ", qtype,qtype,tabsize);
    for (i=0; i<tabsize; i++) {
        switch (n) {
        case 16:  printf("0x%.4x", q16_float_to_fixed(sintab[i]));  break;
        case 32:  printf("0x%.8x", q32_float_to_fixed(sintab[i]));  break;
        default:;
        }

        if ( i == (tabsize-1) )
            printf("\n};\n");
        else if ( ((i+1)%values_per_line) == 0 )
            printf(",\n    ");
        else
            printf(", ");
    }

    printf("\n\n");

    return 0;
}


