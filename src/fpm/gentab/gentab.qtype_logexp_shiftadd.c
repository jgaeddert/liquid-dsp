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
// gentab.qtype_log2_shiftadd.c
//
// Table/constant look-up table for log2
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

    // validate input
    char qtype[64];
    
    switch (n) {
    case 16:    strcpy(qtype,"q16");    break;
    case 32:    strcpy(qtype,"q32");    break;
    default:
        fprintf(stderr,"error: %s, invalid size (%u), must be 16,32\n", argv[0], n);
        exit(1);
    }

    unsigned int i;

    float b=2.0f;       // base of logarithm

    // generate table: Ak = log_b( 1 + 2^-k )
    int logtab[n];
    double inv_log_b = 1.0 / log(b);
    double inv_2_n   = 1.0;
    double tabval;
    for (i=0; i<n; i++) {
        tabval = log(1.0 + inv_2_n) * inv_log_b;
        switch (n) {
        case 16:  logtab[i] = q16_float_to_fixed(tabval);  break;
        case 32:  logtab[i] = q32_float_to_fixed(tabval);  break;
        default:;
        }
        inv_2_n *= 0.5;
    }

    printf("// auto-generated file : do not edit\n");
    printf("// invoked as : ");
    for (i=0; i<argc; i++)
        printf("%s ", argv[i]);
    printf("\n\n");
    printf("#include \"liquidfpm.internal.h\"\n");
    printf("\n");

    // Find maximum number of iterations (look for first zero
    // in the log table)
    unsigned int nmax=0;
    for (i=0; i<n; i++)
        if (logtab[i] == 0) break;
    nmax = i-1;

    printf("// Pre-computed look-up table: A[k] = log2( 1 + 2^-k )\n");
    printf("const %s_t %s_log2_shiftadd_Ak_tab[%u] = {\n", qtype,qtype,n);
    for (i=0; i<n; i++) {
        switch (n) {
        case 16:  printf("    0x%.4x,\n",logtab[i]);  break;
        case 32:  printf("    0x%.8x,\n",logtab[i]);  break;
        default:;
        }
    }
    printf("};\n\n");


    printf("// Maximum number of iterations, given the shiftadd_Ak_table\n");
    printf("// above.  The shift|add algorithm will hit an infinite loop\n");
    printf("// condition for values in the table equal to zero, hence this\n");
    printf("// limitation.\n");
    printf("const unsigned int %s_log2_shiftadd_nmax = %u;\n\n", qtype, nmax);

    // compute base conversion constants
    // TODO : remove as these values are redundant with those in gentab.constants.c
    float ln2     = logf(2.0f);
    float log10_2 = log10f(2.0f);
    float log2_e  = log2f(expf(1));
    float log2_10 = log2f(10.0f);
    printf("// constants for logarithm base conversions\n");
    switch (n) {
    case 16:
        printf("const %s_t %s_ln2     = 0x%.4x; // log(2)\n",   qtype,qtype, q16_float_to_fixed(    ln2));
        printf("const %s_t %s_log10_2 = 0x%.4x; // log(10)\n",  qtype,qtype, q16_float_to_fixed(log10_2));
        printf("const %s_t %s_log2_e  = 0x%.4x; // log2(e)\n",  qtype,qtype, q16_float_to_fixed( log2_e));
        printf("const %s_t %s_log2_10 = 0x%.4x; // log2(10)\n", qtype,qtype, q16_float_to_fixed(log2_10));
        break;
    case 32:
        printf("const %s_t %s_ln2     = 0x%.8x; // log(2)\n",   qtype,qtype, q32_float_to_fixed(    ln2));
        printf("const %s_t %s_log10_2 = 0x%.8x; // log(10)\n",  qtype,qtype, q32_float_to_fixed(log10_2));
        printf("const %s_t %s_log2_e  = 0x%.8x; // log2(e)\n",  qtype,qtype, q32_float_to_fixed( log2_e));
        printf("const %s_t %s_log2_10 = 0x%.8x; // log2(10)\n", qtype,qtype, q32_float_to_fixed(log2_10));
        break;
    default:;
    }
    printf("\n");
    return 0;
}

