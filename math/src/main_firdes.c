//
//
//

#include <stdio.h>
#include "firdes.h"

int main() {
    unsigned int n=21;
    float h[n];
    fir_design_halfband_windowed_sinc(h,n);

    unsigned int i;
    for (i=0; i<n; i++)
        printf("h(%u) = %f;\n", i+1, h[i]);

    printf("done.\n");
    return 0;
}

