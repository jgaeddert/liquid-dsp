char __docstr__[] = "Test runtime availability of certain architectures";

#include <stdio.h>
#include <stdlib.h>
#include "liquid.h"
#include "liquid.argparse.h"

int main(int argc, char* argv[])
{
    // define variables and parse command-line arguments
    liquid_argparse_init(__docstr__);
    liquid_argparse_parse(argc,argv);

    struct liquid_cpuinfo_s info;
    liquid_runtime_supported(&info);

    printf("Neon        : %u\n", info.neon);
    printf("SSE         : %u\n", info.sse);
    printf("SSE2        : %u\n", info.sse2);
    printf("MMX         : %u\n", info.mmx);
    printf("AVX         : %u\n", info.avx);
    printf("AVX2        : %u\n", info.avx2);
    printf("AVX512f     : %u\n", info.avx512f);
    printf("AltiVec     : %u\n", info.altivec);

    return 0;
}
