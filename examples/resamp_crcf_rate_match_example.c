// Demonstration of rate matching with the arbitrary resampler
#include <complex.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "liquid.h"

int main(int argc, char*argv[])
{
    // options
    float        r          = 1.712f;   // true resampling rate (output/input) offset
    unsigned int num_blocks = 400;      // number of blocks
    unsigned int block_len  = 256;      // number of samples in block
    float        alpha      = 0.1f;     // loop bandwidth factor

    // buffers
    float complex buf_0[2*block_len];   // original
    float complex buf_1[2*block_len];   // resampled
    float complex buf_2[2*block_len];   // recovered
    memset(buf_0, 0x00, block_len*sizeof(float complex));

    // create resamplers
    resamp_crcf resamp_0 = resamp_crcf_create_default(r);
    resamp_crcf resamp_1 = resamp_crcf_create_default(1);

    // run loop and print results
    unsigned int i;
    float log_error = 0.0f; // accumulated log error
    float rate      = 1.0f;
    for (i=0; i<num_blocks; i++) {
        // initial offset
        unsigned int n1=0;
        resamp_crcf_execute_block(resamp_0, buf_0, block_len, buf_1, &n1);

        // compensated
        unsigned int n2=0;
        resamp_crcf_execute_block(resamp_1, buf_1, n1, buf_2, &n2);

        // adjust resampling rate
        log_error = (1-alpha)*log_error + alpha*logf((float)block_len/(float)n2);
        resamp_crcf_adjust_rate(resamp_1, expf(alpha*log_error));
        rate = resamp_crcf_get_rate(resamp_1);

        // print results
        printf("  %6u %6u %6u %12.9f %12.9f %12.9f\n", block_len, n1, n2, r, rate, r*rate);
    }
    printf("# true rate: %12.9f, recovered: %12.9f\n", r, r*rate);

    // clean up allocated objects
    resamp_crcf_destroy(resamp_0);
    resamp_crcf_destroy(resamp_1);
    return 0;
}
