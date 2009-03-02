//
//
//

#include <stdlib.h>
#include <stdio.h>

#include "liquid.h"

int main() {
    float x=-1.0f, y;
    unsigned int i, q, num_bits = 8;

    printf("  analog   ADC     DAC\n");
    printf("  ------   ---     ---\n");
    // x = -1.0, -0.9, ... 1.0
    for (i=0; i<21; i++) {
        // limit input value
        x = (x<-1.0f) ? -1.0f : x;
        x = (x> 1.0f) ?  1.0f : x;

        // analog to digital converter
        q = quantize_adc(x,num_bits);

        // digital to analog converter
        y = quantize_dac(q,num_bits);

        printf("%8.4f,  0x%2.2x, %8.4f\n", x, q, y);
        x += 0.1f;
    }
    printf("done.\n");
    return 0;
}

