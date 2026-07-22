char __docstr__[] =
"Test throttling down processor to reach target rate by inserting sleep"
" statements in between processing blocks";

#include <complex.h>
#include <math.h>
#include <stdio.h>
#include <unistd.h>

#include "liquid.h"
#include "liquid.argparse.h"

int main(int argc, char*argv[])
{
    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(float, target_rate, 200e3, 'r', "target processing rate [samples/second]", NULL);
    liquid_argparse_parse(argc,argv);

    // design filter from prototype
    firfilt_crcf filter = firfilt_crcf_create_kaiser(25, 0.25f, 60.0f, 0.0f);
    float sleep_time = 200.0f; // initial guess

    // generate dummy buffers for processing
    unsigned int  buf_len = 1024;
    float complex buf_0[buf_len];
    float complex buf_1[buf_len];
    memset(buf_0, 0x00, buf_len*sizeof(complex float));

    // run with updates
    liquid_timer timer = liquid_timer_create(LIQUID_TIMER_CLOCK);
    unsigned int i;
    for (i=0; i<100; i++) {
        // run block of samples for 200 ms
        unsigned int num_samples_processed = 0;
        liquid_timer_tic(timer);
        while (liquid_timer_toc(timer) < 0.2) {
            // process a block of samples
            firfilt_crcf_execute_block(filter, buf_0, buf_len, buf_1);
            num_samples_processed += buf_len;

            // throttle by calling sleep
            usleep((unsigned int)sleep_time);
        }

        // get processing rate
        float runtime = liquid_timer_toc(timer);
        float rate = (float)num_samples_processed / runtime;
        printf("  %6u, rate = %12.3f k (target = %12.3f k), sleep time = %12.1f us\r",
                i, rate*1e-3f, target_rate*1e-3f, sleep_time);
        fflush(stdout);

        // adjust sleep time proportional to deviation from target rate
        sleep_time *= expf(0.2f*logf(rate/target_rate));
    }
    printf("\n");

    // destroy filter object
    firfilt_crcf_destroy(filter);
    return 0;
}

