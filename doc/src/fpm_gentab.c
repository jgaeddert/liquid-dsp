//
// fpm_gentab.c
//
// Generate fixed-point constants tables.
//  latex.gen/
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <getopt.h>

#include "liquid.h"
#include "liquid.doc.h"

typedef enum {
   QTYPE_UNKNOWN,
   QTYPE_Q16,
   QTYPE_Q32,
} qtype;

typedef enum {
    TABLE_UNKNOWN,
    TABLE_PRIMITIVES,
    TABLE_MATH,
} table;

void q16_gentab_math();
void q32_gentab_math();

void q16_gentab_primitives();
void q32_gentab_primitives();

// print usage/help message
void usage()
{
    printf("fpm_gentab [options]\n");
    printf("  h     : print this help file\n");
    printf("  t     : type (q16, q32)\n");
    printf("  T     : table (primitives, math)\n");
}

int main(int argc, char*argv[]) {
    // parameters
    qtype t = QTYPE_UNKNOWN;
    table T = TABLE_UNKNOWN;

    int dopt;
    while ((dopt = getopt(argc,argv,"ht:T:")) != EOF) {
        switch (dopt) {
        case 'h':   usage();    return 0;
        case 't':
            if      (strcmp(optarg,"q16")==0) t = QTYPE_Q16;
            else if (strcmp(optarg,"q32")==0) t = QTYPE_Q32;
            else {
                fprintf(stderr,"error: %s, invalid type '%s'\n", argv[0], optarg);
                exit(1);
            }
            break;
        case 'T':
            if      (strcmp(optarg,"primitives")==0) T = TABLE_PRIMITIVES;
            else if (strcmp(optarg,"math")==0)       T = TABLE_MATH;
            else {
                fprintf(stderr,"error: %s, invalid table '%s'\n", argv[0], optarg);
                exit(1);
            }
            break;
        default:
            exit(1);
        }
    }

    // validate input
    if (t == QTYPE_UNKNOWN) {
        fprintf(stderr,"error: %s, unspecified qtype\n", argv[0]);
        exit(1);
    } else if (T == TABLE_UNKNOWN) {
        fprintf(stderr,"error: %s, unspecified table\n", argv[0]);
        exit(1);
    }

    if (t == QTYPE_Q16 && T == TABLE_MATH) {
        q16_gentab_math();

    } else if (t == QTYPE_Q16 && T == TABLE_PRIMITIVES) {
        q16_gentab_primitives();
        
    } else if (t == QTYPE_Q32 && T == TABLE_MATH) {
        q32_gentab_math();
        
    } else if (t == QTYPE_Q32 && T == TABLE_PRIMITIVES) {
        q32_gentab_primitives();
        
    } else {
        fprintf(stderr,"error: %s, invalid configuration\n", argv[0]);
        exit(1);
    }

    return 0;
}

void q16_gentab_math()
{
    unsigned int px = 4;    // hex precision
    unsigned int pf = 8;    // float precision
    printf("    {\\tt q16\\_E}         & {\\tt 0x%.*x} & {\\tt %16.*f} &   $e$ \\\\\n",              px, q16_E,        pf, q16_fixed_to_float(q16_E));
    printf("    {\\tt q16\\_LOG2E}     & {\\tt 0x%.*x} & {\\tt %16.*f} &   $\\log_2(e)$ \\\\\n",     px, q16_LOG2E,    pf, q16_fixed_to_float(q16_LOG2E));
    printf("    {\\tt q16\\_LOG10E}    & {\\tt 0x%.*x} & {\\tt %16.*f} &   $\\log_{10}(e)$ \\\\\n",  px, q16_LOG10E,   pf, q16_fixed_to_float(q16_LOG10E));
    printf("    {\\tt q16\\_LN2}       & {\\tt 0x%.*x} & {\\tt %16.*f} &   $\\ln(2)$ \\\\\n",        px, q16_LN2,      pf, q16_fixed_to_float(q16_LN2));
    printf("    {\\tt q16\\_LN10}      & {\\tt 0x%.*x} & {\\tt %16.*f} &   $\\ln(1)$ \\\\\n",        px, q16_LN10,     pf, q16_fixed_to_float(q16_LN10));
    printf("    {\\tt q16\\_PI}        & {\\tt 0x%.*x} & {\\tt %16.*f} &   $\\pi$ \\\\\n",           px, q16_PI,       pf, q16_fixed_to_float(q16_PI));
    printf("    {\\tt q16\\_PI\\_2}     & {\\tt 0x%.*x} & {\\tt %16.*f} &   $\\pi/2$ \\\\\n",        px, q16_PI_2,     pf, q16_fixed_to_float(q16_PI_2));
    printf("    {\\tt q16\\_PI\\_4}     & {\\tt 0x%.*x} & {\\tt %16.*f} &   $\\pi/4$ \\\\\n",        px, q16_PI_4,     pf, q16_fixed_to_float(q16_PI_4));
    printf("    {\\tt q16\\_1\\_PI}     & {\\tt 0x%.*x} & {\\tt %16.*f} &   $1/\\pi$ \\\\\n",        px, q16_1_PI,     pf, q16_fixed_to_float(q16_1_PI));
    printf("    {\\tt q16\\_2\\_PI}     & {\\tt 0x%.*x} & {\\tt %16.*f} &   $2/\\pi$ \\\\\n",        px, q16_2_PI,     pf, q16_fixed_to_float(q16_2_PI));
    printf("    {\\tt q16\\_2\\_SQRTPI} & {\\tt 0x%.*x} & {\\tt %16.*f} &   $2/\\sqrt{\\pi}$ \\\\\n",px, q16_2_SQRTPI, pf, q16_fixed_to_float(q16_2_SQRTPI));
    printf("    {\\tt q16\\_SQRT2}     & {\\tt 0x%.*x} & {\\tt %16.*f} &   $\\sqrt{2}$ \\\\\n",      px, q16_SQRT2,    pf, q16_fixed_to_float(q16_SQRT2));
    printf("    {\\tt q16\\_SQRT1\\_2}  & {\\tt 0x%.*x} & {\\tt %16.*f} &   $\\sqrt{1/2}$ \\\\\n",    px, q16_SQRT1_2,  pf, q16_fixed_to_float(q16_SQRT1_2));
}

void q32_gentab_math()
{
    unsigned int px = 8;    // hex precision
    unsigned int pf = 12;   // float precision
    printf("    {\\tt q32\\_E}         & {\\tt 0x%.*x} & {\\tt %16.*f} & $e$ \\\\\n",              px, q32_E,        pf, q32_fixed_to_float(q32_E));
    printf("    {\\tt q32\\_LOG2E}     & {\\tt 0x%.*x} & {\\tt %16.*f} & $\\log_2(e)$ \\\\\n",     px, q32_LOG2E,    pf, q32_fixed_to_float(q32_LOG2E));
    printf("    {\\tt q32\\_LOG10E}    & {\\tt 0x%.*x} & {\\tt %16.*f} & $\\log_{10}(e)$ \\\\\n",  px, q32_LOG10E,   pf, q32_fixed_to_float(q32_LOG10E));
    printf("    {\\tt q32\\_LN2}       & {\\tt 0x%.*x} & {\\tt %16.*f} & $\\ln(2)$ \\\\\n",        px, q32_LN2,      pf, q32_fixed_to_float(q32_LN2));
    printf("    {\\tt q32\\_LN10}      & {\\tt 0x%.*x} & {\\tt %16.*f} & $\\ln(1)$ \\\\\n",        px, q32_LN10,     pf, q32_fixed_to_float(q32_LN10));
    printf("    {\\tt q32\\_PI}        & {\\tt 0x%.*x} & {\\tt %16.*f} & $\\pi$ \\\\\n",           px, q32_PI,       pf, q32_fixed_to_float(q32_PI));
    printf("    {\\tt q32\\_PI\\_2}     & {\\tt 0x%.*x} & {\\tt %16.*f} & $\\pi/2$ \\\\\n",        px, q32_PI_2,     pf, q32_fixed_to_float(q32_PI_2));
    printf("    {\\tt q32\\_PI\\_4}     & {\\tt 0x%.*x} & {\\tt %16.*f} & $\\pi/4$ \\\\\n",        px, q32_PI_4,     pf, q32_fixed_to_float(q32_PI_4));
    printf("    {\\tt q32\\_1\\_PI}     & {\\tt 0x%.*x} & {\\tt %16.*f} & $1/\\pi$ \\\\\n",        px, q32_1_PI,     pf, q32_fixed_to_float(q32_1_PI));
    printf("    {\\tt q32\\_2\\_PI}     & {\\tt 0x%.*x} & {\\tt %16.*f} & $2/\\pi$ \\\\\n",        px, q32_2_PI,     pf, q32_fixed_to_float(q32_2_PI));
    printf("    {\\tt q32\\_2\\_SQRTPI} & {\\tt 0x%.*x} & {\\tt %16.*f} & $2/\\sqrt{\\pi}$ \\\\\n", px, q32_2_SQRTPI, pf, q32_fixed_to_float(q32_2_SQRTPI));
    printf("    {\\tt q32\\_SQRT2}     & {\\tt 0x%.*x} & {\\tt %16.*f} & $\\sqrt{2}$ \\\\\n",      px, q32_SQRT2,    pf, q32_fixed_to_float(q32_SQRT2));
    printf("    {\\tt q32\\_SQRT1\\_2}  & {\\tt 0x%.*x} & {\\tt %16.*f} & $\\sqrt{1/2}$ \\\\\n",    px, q32_SQRT1_2,  pf, q32_fixed_to_float(q32_SQRT1_2));
}

void q16_gentab_primitives()
{
    unsigned int px = 4;    // hex precision
    unsigned int pf = 8;    // float precision
    printf("    {\\tt q16\\_min}       & {\\tt 0x%.*x} & {\\tt %16.*f} & smallest representable value \\\\\n", px, q16_min, pf, q16_fixed_to_float(q16_min));
    printf("    {\\tt q16\\_max}       & {\\tt 0x%.*x} & {\\tt %16.*f} & largest representable value \\\\\n", px, q16_max, pf, q16_fixed_to_float(q16_max));
    printf("    {\\tt q16\\_zero}      & {\\tt 0x%.*x} & {\\tt %16.*f} & zero \\\\\n", px, q16_zero, pf, q16_fixed_to_float(q16_zero));
    printf("    {\\tt q16\\_one}       & {\\tt 0x%.*x} & {\\tt %16.*f} & one  \\\\\n", px, q16_one, pf, q16_fixed_to_float(q16_one));
    printf("    {\\tt q16\\_2pi}       & {\\tt 0x%.*x} & {\\tt %16.*f} & $2\\pi$ (angular)  \\\\\n", px, q16_2pi, pf, q16_fixed_to_float(q16_2pi));
    printf("    {\\tt q16\\_pi}        & {\\tt 0x%.*x} & {\\tt %16.*f} & $\\pi$ (angular)  \\\\\n", px, q16_pi, pf, q16_fixed_to_float(q16_pi));
    printf("    {\\tt q16\\_pi\\_by\\_2} & {\\tt 0x%.*x} & {\\tt %16.*f} & $\\pi/2$ (angular)  \\\\\n", px, q16_pi_by_2, pf, q16_fixed_to_float(q16_pi_by_2));
    printf("    {\\tt q16\\_pi\\_by\\_4} & {\\tt 0x%.*x} & {\\tt %16.*f} & $\\pi/4$ (angular)  \\\\\n", px, q16_pi_by_4, pf, q16_fixed_to_float(q16_pi_by_4));
    printf("    {\\tt q16\\_angle\\_scalarf} & -       & {\\tt %16.*f} & angular/scalar relationship  \\\\\n", pf, q16_angle_scalarf);
}

void q32_gentab_primitives()
{
    unsigned int px = 8;    // hex precision
    unsigned int pf = 12;   // float precision
    printf("    {\\tt q32\\_min}       & {\\tt 0x%.*x} & {\\tt %16.*f} & smallest representable value \\\\\n", px, q32_min, pf, q32_fixed_to_float(q32_min));
    printf("    {\\tt q32\\_max}       & {\\tt 0x%.*x} & {\\tt %16.*f} & largest representable value \\\\\n", px, q32_max, pf, q32_fixed_to_float(q32_max));
    printf("    {\\tt q32\\_zero}      & {\\tt 0x%.*x} & {\\tt %16.*f} & zero \\\\\n", px, q32_zero, pf, q32_fixed_to_float(q32_zero));
    printf("    {\\tt q32\\_one}       & {\\tt 0x%.*x} & {\\tt %16.*f} & one  \\\\\n", px, q32_one, pf, q32_fixed_to_float(q32_one));
    printf("    {\\tt q32\\_2pi}       & {\\tt 0x%.*x} & {\\tt %16.*f} & $2\\pi$ (angular)  \\\\\n", px, q32_2pi, pf, q32_fixed_to_float(q32_2pi));
    printf("    {\\tt q32\\_pi}        & {\\tt 0x%.*x} & {\\tt %16.*f} & $\\pi$ (angular)  \\\\\n", px, q32_pi, pf, q32_fixed_to_float(q32_pi));
    printf("    {\\tt q32\\_pi\\_by\\_2} & {\\tt 0x%.*x} & {\\tt %16.*f} & $\\pi/2$ (angular)  \\\\\n", px, q32_pi_by_2, pf, q32_fixed_to_float(q32_pi_by_2));
    printf("    {\\tt q32\\_pi\\_by\\_4} & {\\tt 0x%.*x} & {\\tt %16.*f} & $\\pi/4$ (angular)  \\\\\n", px, q32_pi_by_4, pf, q32_fixed_to_float(q32_pi_by_4));
    printf("    {\\tt q32\\_angle\\_scalarf} & -       & {\\tt %16.*f} & angular/scalar relationship  \\\\\n", pf, q32_angle_scalarf);
}

