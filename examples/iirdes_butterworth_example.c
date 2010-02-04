//
//
//

#include <stdio.h>
#include <stdlib.h>

#include "liquid.h"

//#define OUTPUT_FILENAME "iirdes_butterworth_example.c"

int main() {
    // options
    unsigned int n=5;   // order

    float b[n+1];       // numerator
    float a[n+1];       // denominator

    butterf(b,a,n);

    printf("done.\n");
    return 0;
}

