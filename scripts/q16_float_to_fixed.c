//
// q16_float_to_fixed.c : convert floating point to fixed point
//

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "liquidfpm.internal.h"

int main(int argc, char * argv[])
{
    if (argc < 2) {
        printf("usage: %s [number] [number] ...\n", __FILE__);
        exit(1);
    }

    int i;
    int n = argc - 1;
    for (i=0; i<n; i++) {
        float vf = atof(argv[i+1]);
        q16_t v  = q16_float_to_fixed(vf);

        printf("  %12.8f > 0x%.4x\n", vf, v);
    }

    return 0;
}
