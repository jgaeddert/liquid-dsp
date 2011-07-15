// 
// modem_demodulate_soft_test.c
//
// Tests soft demoulation using log-likelihood ratio
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "liquid.h"

#define OUTPUT_FILENAME "iirdes_example.m"


int main() {
    srand(time(NULL));

    // options
    modulation_scheme ms = LIQUID_MODEM_QAM;    // modulation scheme
    unsigned int bps = 4;                       // bits/symbol
    float complex e = 0.1f + _Complex_I*0.1f;   // error

    unsigned int i;

    // derived values
    unsigned int M = 1 << bps;  // constellation size
    float sig = 0.2f;           // noise standard deviation

    // generate constellation
    modem q = modem_create(ms, bps);
    float complex c[M];         // constellation
    for (i=0; i<M; i++)
        modem_modulate(q, i, &c[i]);
    modem_destroy(q);

    // select input symbol and compute received symbol
    unsigned int sym_in = rand() % M;
    float complex r = c[sym_in] + e;

    // run soft demodulation for each bit
    float soft_bits[bps];
    unsigned int k;
    for (k=0; k<bps; k++) {
        printf("\n");
        printf("********* bit index %u ************\n", k);
        // reset soft bit value
        soft_bits[k] = 0.0f;
        float bit_0 = 0.0f;
        float bit_1 = 0.0f;

        // compute LLR for this bit
        for (i=0; i<M; i++) {
            // compute distance between received point and symbol
            float d = crealf( (r-c[i])*conjf(r-c[i]) );
            float t = expf( -d / (2.0f*sig*sig) );

            // check if this symbol has a '0' or '1' at this bit index
            unsigned int bit = (i >> (bps-k-1)) & 0x01;
            //printf("%c", bit ? '1' : '0');

            if (bit) bit_1 += t;
            else     bit_0 += t;

            printf("  symbol : ");
            unsigned int j;
            for (j=0; j<bps; j++)
                printf("%c", (i >> (bps-j-1)) & 0x01 ? '1' : '0');
            printf(" [%c]", bit ? '1' : '0');
            printf(" { %7.4f %7.4f}, d=%7.4f, t=%12.8f\n", crealf(c[i]), cimagf(c[i]), d, t);

        }

        soft_bits[k] = logf(bit_1) - logf(bit_0);
        printf(" {0 : %12.8f, 1 : %12.8f}\n", bit_0, bit_1);
    }

    // print results
    printf("\n");
    printf("  input symbol : ");
    for (k=0; k<bps; k++)
        printf("%c", (sym_in >> (bps-k-1)) & 0x01 ? '1' : '0');
    printf(" {%12.8f, %12.8f}\n", crealf(c[sym_in]), cimagf(c[sym_in]));

    printf("  soft bits :\n");
    for (k=0; k<bps; k++)
        printf("    %c : %12.8f\n", (sym_in >> (bps-k-1)) & 0x01 ? '1' : '0', soft_bits[k]);

    printf("done.\n");
    return 0;
}
