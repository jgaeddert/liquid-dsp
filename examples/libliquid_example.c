// test library versioning
#include <stdio.h>
#include <stdlib.h>
#include "liquid.h"

int main() {
    // macro to validate version in header matches linked library
    LIQUID_VALIDATE_LIBVERSION

    printf("liquid version              :   %s\n", liquid_version);
    printf("  major                     :   %d\n", LIQUID_VERSION_MAJOR);
    printf("  minor                     :   %d\n", LIQUID_VERSION_MINOR);
    printf("  patch                     :   %d\n", LIQUID_VERSION_PATCH);
    printf("liquid libversion           :   %s\n", liquid_libversion());
    printf("liquid libversion number    : 0x%.8x\n", liquid_libversion_number());
    return 0;
}
