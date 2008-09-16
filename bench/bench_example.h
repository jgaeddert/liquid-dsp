//
// Example of a benchmark header
//

// includes:
//   #include <sys/resource.h>
//   #include <getopt.h>
#include "liquid/benchmark.h"

// strings parsed by benchmarkgen.py
const char * mybench_opts[3] = {
    "opt1a opt1b",
    "opt2a opt2b opt2c",
    "opt3a opt3b opt3c"
};


void benchmark_mybench(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations,
    unsigned int argc,
    char *argv[])
{
    // DSP initiazation goes here

    unsigned int i;
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        // DSP execution goes here
    }
    getrusage(RUSAGE_SELF, _finish);

    // DSP cleanup goes here
}


