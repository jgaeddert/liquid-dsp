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
    // test message handling
    int rc = liquid_format_error("myfile",22,"main",LIQUID_ERROR_FILE_IO,"my message");

    if (rc != LIQUID_OK)
        liquid_error_print();

    //
    printf("done.\n");
    return 0;
}
