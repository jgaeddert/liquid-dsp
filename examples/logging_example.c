// This example demonstrates logging capabilities with liquid,
#include <stdlib.h>
#include <stdio.h>
#include "liquid.h"

int main(int argc, char*argv[])
{
    liquid_logger_set_level(NULL,LIQUID_INFO);
    liquid_log_warn("this is a warning message with a value: %d", 12);
    liquid_log_debug("this will not be logged since it is below the specified level");

    return 0;
}

