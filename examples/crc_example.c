const char __docstr__[] =
"Cyclic redundancy check (CRC) example.  This example demonstrates"
" how a CRC can be used to validate data received through un-reliable"
" means (e.g. a noisy channel).  A CRC is, in essence, a strong"
" algebraic error detection code that computes a key on a block of data"
" using base-2 polynomials.";

#include <stdio.h>
#include <stdlib.h>

#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char*argv[])
{
    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*,    crc_type, "crc32", 'v', "data integrity check", liquid_argparse_crc);
    liquid_argparse_add(unsigned, n,        32,      'n', "original data message length", NULL);
    liquid_argparse_parse(argc,argv);

    // validate options
    crc_scheme crc  = (crc_scheme)liquid_getopt_str2crc(crc_type);

    unsigned int i;

    // initialize data array, leaving space for key at the end
    LIQUID_VLA(unsigned char, data, n+4);
    for (i=0; i<n; i++)
        data[i] = rand() & 0xff;

    // append key to data message
    crc_append_key(crc, data, n);

    // check uncorrupted data
    printf("testing uncorrupted data... %s\n", crc_check_key(crc, data, n) ? "pass" : "FAIL");

    // corrupt message and check data again
    data[0]++;
    printf("testing corrupted data...   %s\n", crc_check_key(crc, data, n) ? "pass (unexpected)" : "FAIL (expected)");

    printf("done.\n");
    return 0;
}
