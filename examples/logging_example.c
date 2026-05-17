char __docstr__[] = "This example demonstrates logging capabilities with liquid";

#include <stdlib.h>
#include <stdio.h>
#include "liquid.h"
#include "liquid.argparse.h"

int main(int argc, char*argv[])
{
    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*, filename, "logging_example.log",'o', "output filename",NULL);
    liquid_argparse_parse(argc,argv);

    //
    liquid_logger_set_level(NULL,LIQUID_INFO);
    liquid_log_warn("this is a warning message with a value: %d", 12);
    liquid_log_debug("this will not be logged since it is below the specified level");

    unsigned int i;
    FILE * fid = NULL;
    for (i=0; i<10; i++)
    {
        liquid_log_info("the value of i is %u", i);

        // open a file for logging
        if (i==4)
            fid = liquid_logger_add_filename(NULL,filename,LIQUID_INFO);

        // close the file part-way through the program
        if (i==8)
            fclose(fid);
    }

    return 0;
}

