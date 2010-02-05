//
//
//

#include <stdio.h>
#include <stdlib.h>

#include "liquid.h"

//#define OUTPUT_FILENAME "iirdes_butterworth_example.c"

int main() {
    // options
    unsigned int n=5;   // filter order
    float fc = 0.1f;    // cutoff

    float b[n+1];       // numerator
    float a[n+1];       // denominator

    butterf(n,fc,b,a);

    // print coefficients
    unsigned int i;
    for (i=0; i<=n; i++) printf("a(%3u) = %12.8f;\n", i+1, a[i]);
    for (i=0; i<=n; i++) printf("b(%3u) = %12.8f;\n", i+1, b[i]);

    printf("done.\n");
    return 0;
}

