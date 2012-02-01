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
// Table generation for sin|cos CORDIC
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
            usage();
            return 1;
        }
    }

    // validate length
    char qtype[64];
    switch (n) {
    case 16:  strcpy(qtype,"q16");  break;
    case 32:  strcpy(qtype,"q32");  break;
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
    switch (n) {
    case 16:  printf("const %s_t %s_cordic_k_inv = 0x%.4x;\n\n", qtype,qtype,q16_float_to_fixed(k_inv)); break;
    case 32:  printf("const %s_t %s_cordic_k_inv = 0x%.8x;\n\n", qtype,qtype,q32_float_to_fixed(k_inv)); break;
    default:;
    }

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

        // write output
        switch (n) {
        case 16:  printf("    0x%.4x,\n", q16_float_to_fixed(Ak));  break;
        case 32:  printf("    0x%.8x,\n", q32_float_to_fixed(Ak));  break;
        default:;
        }

        // update 1 / 2^n
        inv_2_n *= 0.5;
    }
    printf("};\n\n");

    return 0;
}

