// 
// error_handling.c
//
// This example demonstates error handling in liquid.
//

#include <stdlib.h>
#include <stdio.h>

#include "liquid.internal.h"

int main(int argc, char*argv[])
{
    /*
    // test message handling
    liquid_error(LIQUID_ERROR_FILE_IO,"main(), found %u, expected %u", 1, 0);
    liquid_error_print();
    */

    // create agc object
    agc_crcf q = agc_crcf_create();

    // try to set invalid parameter
    int rc = agc_crcf_set_bandwidth(q, -1e-3f);
    if (rc != LIQUID_OK)
        liquid_error_print();

    // destroy object
    agc_crcf_destroy(q);

    //
    printf("done.\n");
    return 0;
}
