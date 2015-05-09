/*
 * Copyright (c) 2007 - 2015 Joseph Gaeddert
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
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

    // validate length
    char qtype[10];
    unsigned int intbits;
    unsigned int fracbits;
    switch (n) {
    case 16:
        strcpy(qtype,"q16");
        intbits = q16_intbits;
        fracbits = q16_fracbits;
        break;
    case 32:
        strcpy(qtype,"q32");
        intbits = q32_intbits;
        fracbits = q32_fracbits;
        break;
    default:
        fprintf(stderr,"error: %s, invalid size (%u), must be 16,32\n", argv[0], n);
        exit(1);
    }


    unsigned int i;

    // generate floating-point table
    double sintab[tabsize];
    double atan2tab[tabsize];
    double log2r_max = log2((double)tabsize);
    for (i=0; i<tabsize; i++) {
        sintab[i] = sin( M_PI_2 * ((double)i)/((double)tabsize) );
        
        double log2r = log2r_max * ((double)i) / ((double)tabsize);
        double r     = exp2(log2r);
        atan2tab[i]  = atan(r);
    }

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
        // print tab line
        printf("0x%.*x", n/4, qtype_float_to_fixed(sintab[i],intbits,fracbits));

        if ( i == (tabsize-1) )
            printf("\n};\n");
        else if ( ((i+1)%values_per_line) == 0 )
            printf(",\n    ");
        else
            printf(", ");
    }

    printf("\n\n");


    printf("// atan2 look-up table\n");
    printf("const %s_t %s_atan2tab[%u] = {\n    ", qtype,qtype,tabsize);
    for (i=0; i<tabsize; i++) {
        // print tab line
        printf("0x%.*x", n/4, qtype_angle_float_to_fixed(atan2tab[i],intbits,fracbits));

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


