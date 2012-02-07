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
// constant generation
//
// regular constants
//      qtype_E         = 2.71828182845905  : e
//      qtype_LOG2E     = 1.44269504088896  : log2(e)
//      qtype_LOG10E    = 0.43429448190325  : log10(e)
//      qtype_LN2       = 0.69314718055995  : log(2)
//      qtype_LN10      = 2.30258509299405  : log(10)
//      qtype_PI        = 3.14159265358979  : pi (actual)
//      qtype_PI_2      = 1.57079632679490  : pi/2
//      qtype_PI_4      = 0.78539816339745  : pi/4
//      qtype_1_PI      = 0.31830988618379  : 1/pi
//      qtype_2_PI      = 0.63661977236758  : 2/pi
//      qtype_2_SQRTPI  = 1.12837916709551  : 2/sqrt(pi)
//      qtype_SQRT2     = 1.41421356237310  : sqrt(2)
//      qtype_SQRT1_2   = 0.70710678118655  : 1/sqrt(2)
//
// conversion:
//      qtype_angle_scalar                  : (1 << (intbits-2)) / pi
//
// qtype_lgamma:
//      qtype_ln2pi     = 1.83787706640935  : ln(2*pi)
//      qtype_inv_12    = 0.08333333333333  : 1/12

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>

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
    while ((dopt = getopt(argc,argv,"uhn:i:f:")) != EOF) {
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

    printf("// auto-generated file : do not edit\n");
    printf("// invoked as : ");
    for (i=0; i<argc; i++)
        printf("%s ", argv[i]);
    printf("\n\n");

    printf("#include \"liquidfpm.internal.h\"\n");
    printf("\n");

    printf("// math constants\n");
    printf("const %s_t %s_E        = 0x%.*x; // e\n",          qtype, qtype, n/4, qtype_float_to_fixed(M_E,intbits,fracbits));
    printf("const %s_t %s_LOG2E    = 0x%.*x; // log2(e)\n",    qtype, qtype, n/4, qtype_float_to_fixed(M_LOG2E,intbits,fracbits));
    printf("const %s_t %s_LOG10E   = 0x%.*x; // log10(e)\n",   qtype, qtype, n/4, qtype_float_to_fixed(M_LOG10E,intbits,fracbits));
    printf("const %s_t %s_LN2      = 0x%.*x; // log(2)\n",     qtype, qtype, n/4, qtype_float_to_fixed(M_LN2,intbits,fracbits));
    printf("const %s_t %s_LN10     = 0x%.*x; // log(10)\n",    qtype, qtype, n/4, qtype_float_to_fixed(M_LN10,intbits,fracbits));
    printf("const %s_t %s_PI       = 0x%.*x; // pi\n",         qtype, qtype, n/4, qtype_float_to_fixed(M_PI,intbits,fracbits));
    printf("const %s_t %s_PI_2     = 0x%.*x; // pi/2\n",       qtype, qtype, n/4, qtype_float_to_fixed(M_PI_2,intbits,fracbits));
    printf("const %s_t %s_PI_4     = 0x%.*x; // pi/4\n",       qtype, qtype, n/4, qtype_float_to_fixed(M_PI_4,intbits,fracbits));
    printf("const %s_t %s_1_PI     = 0x%.*x; // 1/pi\n",       qtype, qtype, n/4, qtype_float_to_fixed(M_1_PI,intbits,fracbits));
    printf("const %s_t %s_2_PI     = 0x%.*x; // 2/pi\n",       qtype, qtype, n/4, qtype_float_to_fixed(M_2_PI,intbits,fracbits));
    printf("const %s_t %s_2_SQRTPI = 0x%.*x; // 2/sqrt(pi)\n", qtype, qtype, n/4, qtype_float_to_fixed(M_2_SQRTPI,intbits,fracbits));
    printf("const %s_t %s_SQRT2    = 0x%.*x; // sqrt(2)\n",    qtype, qtype, n/4, qtype_float_to_fixed(M_SQRT2,intbits,fracbits));
    printf("const %s_t %s_SQRT1_2  = 0x%.*x; // 1/sqrt(2)\n",  qtype, qtype, n/4, qtype_float_to_fixed(M_SQRT1_2,intbits,fracbits));
    printf("\n");

    // 
    // conversion
    //
    printf("// conversion\n");
    float angle_scalar = (1<<(intbits-2)) / M_PI;
    printf("const %s_t %s_angle_scalar = 0x%.*x; // qtype_pi / M_PI = 2^(intbits-2)/3.14159\n",
            qtype, qtype, n/4, qtype_float_to_fixed(angle_scalar,intbits,fracbits));
    printf("\n");

    // 
    // qtype_lgamma
    //
    float ln2pi     = logf(2*M_PI);
    float inv_12    = 1.0f / 12.0f;
    printf("// %s_lgamma\n",qtype);
    printf("const %s_t %s_ln2pi    = 0x%.*x; // log2(pi)\n", qtype, qtype, n/4, qtype_float_to_fixed(ln2pi,intbits,fracbits));
    printf("const %s_t %s_inv_12   = 0x%.*x; // 1/12\n", qtype, qtype, n/4, qtype_float_to_fixed(inv_12,intbits,fracbits));
    printf("\n\n");

    return 0;
}

