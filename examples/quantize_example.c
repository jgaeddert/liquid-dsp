//
//
//

#include <stdlib.h>
#include <stdio.h>

#include "liquid.h"

int main() {
    unsigned int num_steps=30;
    unsigned int num_bits=3;

    unsigned int i, q;
    float x=-1.5f, y, dx=3.0f/((float)num_steps);

    printf("  analog   ADC     DAC\n");
    printf("  ------   ---     ---\n");
    // x = -1.0, -0.9, ... 1.0
    for (i=0; i<num_steps+1; i++) {
        // analog to digital converter
        q = quantize_adc(x,num_bits);

        // digital to analog converter
        y = quantize_dac(q,num_bits);

        printf("%8.4f,  0x%2.2x, %8.4f\n", x, q, y);
        x += dx;
    }
    printf("done.\n");
    return 0;
}

