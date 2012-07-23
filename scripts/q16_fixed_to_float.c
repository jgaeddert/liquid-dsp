//
// q16_fixed_to_float.c : convert fixed point to floating point
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
        unsigned int v;
        sscanf(argv[i+1],"0x%x", &v);
        float vf = q16_fixed_to_float(v);

        printf("  0x%.4x > %12.8f\n", v, vf);
    }

    return 0;
}
