const char __docstr__[] = "Demonstrate filter copy operation";

#include <stdio.h>
#include <math.h>
#ifndef _MSC_VER
#include <complex.h>
#endif
#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char* argv[])
{
    // define variables and parse command-line arguments
    liquid_argparse_init(__docstr__);
    liquid_argparse_parse(argc,argv);

    // design filter from prototype
    firfilt_crcf filt_orig = firfilt_crcf_create_kaiser(21, 0.345f, 60.0f, 0.0f);
    firfilt_crcf_set_scale(filt_orig, 2.0f);
    firfilt_crcf_print(filt_orig);

    // start running input through filter
    unsigned int n = 32;
    unsigned int i;
    liquid_float_complex x, y_orig, y_copy;
    for (i=0; i<n; i++) {
        // run filter
        x = randnf() + _Complex_I*randnf();
        firfilt_crcf_execute_one(filt_orig, x, &y_orig);
        printf(" [%3u] orig: %12.8f + j%12.8f\n", i, crealf(y_orig), cimagf(y_orig));
    }

    // copy filter
    firfilt_crcf filt_copy = firfilt_crcf_copy(filt_orig);

    // continue running through both filters
    for (i=0; i<n; i++) {
        // run filters
        x = randnf() + _Complex_I*randnf();
        firfilt_crcf_execute_one(filt_orig, x, &y_orig);
        firfilt_crcf_execute_one(filt_copy, x, &y_copy);

        float error = cabsf( y_orig - y_copy );
        printf(" [%3u] orig: %12.8f + j%12.8f, copy: %12.8f + j%12.8f, error: %8g\n", i+n,
                crealf(y_orig), cimagf(y_orig),
                crealf(y_copy), cimagf(y_copy),
                error);
    }

    // destroy filter objects
    firfilt_crcf_destroy(filt_orig);
    firfilt_crcf_destroy(filt_copy);

    printf("done.\n");
    return 0;
}

