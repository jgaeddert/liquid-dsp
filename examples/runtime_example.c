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
    printf("AltiVec     : %u\n", info.altivec);
    printf("MMX         : %u\n", info.mmx);
    printf("SSE         : %u\n", info.sse);
    printf("SSE2        : %u\n", info.sse2);
    printf("SSE3        : %u\n", info.sse3);
    printf("SSSE3       : %u\n", info.ssse3);
    printf("SSE4.1      : %u\n", info.sse41);
    printf("SSE4.2      : %u\n", info.sse42);
    printf("AVX         : %u\n", info.avx);
    printf("FMA3        : %u\n", info.fma3);
    printf("AVX2        : %u\n", info.avx2);
    printf("AVX512      : %u\n", info.avx512);
    printf("AMX10.1     : %u\n", info.amx101);
    printf("AMX10.2     : %u\n", info.amx102);
    printf("cores       : %u\n", info.cores);

    return 0;
}
