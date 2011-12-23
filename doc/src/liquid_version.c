// 
// liquid_version.c
//

#include <stdio.h>

#include "liquid.h"

int main()
{
    printf("\\newcommand{\\liquidversion}{%s}\n", LIQUID_VERSION);
    return 0;
}

