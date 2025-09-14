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
// Table generation for CORDIC (sin, cos, atan)
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <math.h>

#include "liquidfpm.internal.h"

void usage(void)
{
    printf("%s usage:\n", __FILE__);
    printf("  u/h   :   print this help file\n");
    printf("    n   :   length (e.g. 32)\n");
}

int main(int argc, char*argv[])
{
    // options
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
            exit(1);
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


    printf("// auto-generated file : do not edit\n");
    printf("// invoked as : ");
    unsigned int i;
    for (i=0; i<argc; i++)
        printf("%s ", argv[i]);
    printf("\n\n");

    printf("#include \"liquidfpm.internal.h\"\n");
    printf("\n");

    // 1/K scaling factor
    float k_inv = 0.60725293500888125616;

    printf("// scaling factor 1/K:  K = prod(k=0,infty){ sqrt(1 + 2^(-2k)) }\n");
    printf("const %s_t %s_cordic_k_inv = 0x%.*x;\n\n",
            qtype,
            qtype,
            n/4,
            qtype_float_to_fixed(k_inv,intbits,fracbits));

    // generate table
    double inv_2_n   = 1.0;
    double Ak;
    float qtype_angle_scalarf; // pi / 2^{intbits-2}
    switch (n) {
    case 16:  qtype_angle_scalarf = q16_angle_scalarf;  break;
    case 32:  qtype_angle_scalarf = q32_angle_scalarf;  break;
    default:;
    }

    printf("// cordic coefficients: A[k] = arctan(2^-k)\n");
    printf("const %s_t %s_cordic_Ak_tab[%u] = {\n", qtype,qtype,n);
    for (i=0; i<n; i++) {
        // precompute Ak, normalizing by angular scaling factor. This
        // is necessary by the nature of how angles are stored in
        // fixed-point decimal:
        //    2*pi  :   0x7fffffff (the largest positive number)
        //   -2*pi  :   0xffffffff (the largest negative number)
        Ak = atanf(inv_2_n) / qtype_angle_scalarf;

        printf("    0x%.*x,\n", n/4, qtype_float_to_fixed(Ak,intbits,fracbits));

        // update 1 / 2^n
        inv_2_n *= 0.5;
    }
    printf("};\n\n");

    return 0;
}

