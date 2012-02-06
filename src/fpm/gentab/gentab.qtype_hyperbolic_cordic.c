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
// Table generation for hyperbolic CORDIC (sinh, cosh)
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

    // 1/Kp scaling factor
    float kp_inv = 1.207497067763072128878;

    printf("// scaling factor 1/Kp:  \n");
    printf("//   Kp = prod(k=0,infty){ sqrt(1 - 2^(-2*k)) } *\n");
    printf("//        prod(k=0,infty){ sqrt(1 - 2^(-2*i_k)) }\n");
    printf("// where\n");
    printf("//   i_0     = 4,\n");
    printf("//   i_(k+1) = 3*i_k + 1\n");
    printf("// 1/Kp = %16.14f\n", kp_inv);
    printf("const %s_t %s_sinhcosh_cordic_kp_inv = 0x%.*x;\n\n", 
            qtype,qtype,
            n/4,
            qtype_float_to_fixed(kp_inv,intbits,fracbits));

    // generate table
    printf("// sinh|cosh cordic coefficients: A[k] = arctanh(2^-k)\n");
    printf("// NOTE: first value is ignored in algorithm\n");
    printf("const %s_t %s_sinhcosh_cordic_Ak_tab[%u] = {\n", qtype,qtype,n);
    double inv_2_n   = 1.0;
    double Ak;
    unsigned int k;
    for (k=0; k<n; k++) {
        // precompute Ak (first value is ignored in algorithm regardless)
        Ak = k == 0 ? 0 : atanhf(inv_2_n);

        // write outpt value
        printf("    0x%.*x%s",
                n/4,
                qtype_float_to_fixed(Ak,intbits,fracbits),
                (k<n-1) ? ",\n" : "};\n\n");

        // update 1 / 2^n
        inv_2_n *= 0.5;
    }

    return 0;
}

