char __docstr__[] = "Test libliquid library versioning";

#include <stdio.h>
#include <stdlib.h>
#include "liquid.h"
#include "liquid.argparse.h"

int main(int argc, char* argv[])
{
    // define variables and parse command-line arguments
    liquid_argparse_init(__docstr__);
    liquid_argparse_parse(argc,argv);

    // validate that the included header matches linked version
    LIQUID_VALIDATE_LIBVERSION;

    // print values
    printf("liquid version           : %s\n", liquid_version);
    printf("liquid libversion        : %s\n", liquid_libversion());
    printf("liquid libversion number : 0x%.6x\n", liquid_libversion_number());

    // consistent check for library versions given than starting with 1.7.0 version number
    // representation changed
    int liquid_version_major = 0;
    int liquid_version_minor = 0;
    int liquid_version_patch = 0;
    if (LIQUID_VERSION_NUMBER >= 1000000) {
        // strip major/minor/patch values using base-10 logic
        liquid_version_major = (LIQUID_VERSION_NUMBER / 1000000);
        liquid_version_minor = (LIQUID_VERSION_NUMBER - 1000000*liquid_version_major) / 1000;
        liquid_version_patch = (LIQUID_VERSION_NUMBER % 1000);
    } else {
        // strip major/minor/patch values using bit masks
        liquid_version_major = (LIQUID_VERSION_NUMBER >> 16) & 0xff;
        liquid_version_minor = (LIQUID_VERSION_NUMBER >>  8) & 0xff;
        liquid_version_patch = (LIQUID_VERSION_NUMBER      ) & 0xff;
        // alterantively we could check for the existence of these macros
        //liquid_version_major = LIQUID_VERSION_MAJOR;
        //liquid_version_minor = LIQUID_VERSION_MINOR;
        //liquid_version_patch = LIQUID_VERSION_PATCH;
    }
    printf("major = %d, minor = %d, patch = %d\n",
        liquid_version_major, liquid_version_minor, liquid_version_patch);

    return 0;
}
