// demonstrate error handling in liquid
#include <stdlib.h>
#include <stdio.h>
#include "liquid.internal.h"

int main(int argc, char*argv[])
{
    // create agc object
    agc_crcf q = agc_crcf_create();

    // try to set invalid parameter
    int rc = agc_crcf_set_bandwidth(q, -1e-3f);
    if (rc != LIQUID_OK)
        printf("received error %d: %s\n", rc, liquid_error_info(rc));

    // destroy object
    agc_crcf_destroy(q);

    // set custom error
    liquid_error(LIQUID_EICONFIG,"configuration (%s), invalid code %u > %u", "hello", 4, 2);

    //
    printf("done.\n");
    return 0;
}
