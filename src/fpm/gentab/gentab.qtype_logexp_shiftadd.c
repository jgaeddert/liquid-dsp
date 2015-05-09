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

    // base of logarithm
    float b=2.0f;

    // generate table: Ak = log_b( 1 + 2^-k )
    int logtab[n];
    double inv_log_b = 1.0 / log(b);
    double inv_2_n   = 1.0;
    double tabval;
    for (i=0; i<n; i++) {
        // compute floating-point value
        tabval = log(1.0 + inv_2_n) * inv_log_b;

        // convert to fixed-point
        logtab[i] = qtype_float_to_fixed(tabval,intbits,fracbits);

        // update argument
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
    for (i=0; i<n; i++)
        printf("    0x%.*x,\n", n/4, logtab[i]);
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
    printf("const %s_t %s_ln2     = 0x%.*x; // log(2)\n",   qtype, qtype, n/4, qtype_float_to_fixed(    ln2,intbits,fracbits));
    printf("const %s_t %s_log10_2 = 0x%.*x; // log(10)\n",  qtype, qtype, n/4, qtype_float_to_fixed(log10_2,intbits,fracbits));
    printf("const %s_t %s_log2_e  = 0x%.*x; // log2(e)\n",  qtype, qtype, n/4, qtype_float_to_fixed( log2_e,intbits,fracbits));
    printf("const %s_t %s_log2_10 = 0x%.*x; // log2(10)\n", qtype, qtype, n/4, qtype_float_to_fixed(log2_10,intbits,fracbits));
    printf("\n");

    return 0;
}

