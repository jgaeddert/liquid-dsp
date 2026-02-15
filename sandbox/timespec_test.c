char __docstr__[] = "This example demonstrates logging capabilities with liquid";

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h> // gettimeofday

#include "liquid.h"
#include "liquid.argparse.h"

int main(int argc, char*argv[])
{
    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*, filename, "logging_example.log",'o', "output filename",NULL);
    liquid_argparse_parse(argc,argv);

    // test timestamp/timespec

    // 1: traditional method
    {
        printf("method 1:\n");
        time_t t = time(NULL); // time_t is essentially a long int
        printf("time_t t = %ld\n", t);
        struct tm * now = localtime(&t); // parse out time object into local time components
        char time_str[64];
        time_str[
            strftime(time_str, sizeof(time_str), "%Y-%m-%d %T", now)
        ] = '\0';
        printf("parsed: %s\n\n", time_str);
    }

    // 2: get higher resolution time
    {
        printf("method 2:\n");
        struct timeval tv; // get both seconds & microseconds
        gettimeofday(&tv, NULL);
        time_t t = tv.tv_sec; // time_t is really just a long int representing seconds
        printf("time_t t = %ld + %d\n", t, tv.tv_usec);
        struct tm * now = localtime(&t); // parse out time object into local time components
        char time_str[64]; // string to hold formatted time
        size_t n = strftime(time_str, sizeof(time_str), "%Y-%m-%d %T", now);
        sprintf(time_str+n,".%.3d", tv.tv_usec / 1000);
        printf("parsed: %s\n\n", time_str);
    }

    return 0;
}

