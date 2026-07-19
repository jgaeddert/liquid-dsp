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

    struct liquid_sysinfo_s info;
    liquid_runtime_supported(&info);

    printf("cpu info:\n");
    printf("  AltiVec   : %u\n", info.cpuinfo.altivec);
    printf("  Neon      : %u\n", info.cpuinfo.neon);
    printf("  MMX       : %u\n", info.cpuinfo.mmx);
    printf("  SSE       : %u\n", info.cpuinfo.sse);
    printf("  SSE2      : %u\n", info.cpuinfo.sse2);
    printf("  SSE3      : %u\n", info.cpuinfo.sse3);
    printf("  SSSE3     : %u\n", info.cpuinfo.ssse3);
    printf("  SSE4.1    : %u\n", info.cpuinfo.sse41);
    printf("  SSE4.2    : %u\n", info.cpuinfo.sse42);
    printf("  AVX       : %u\n", info.cpuinfo.avx);
    printf("  FMA3      : %u\n", info.cpuinfo.fma3);
    printf("  AVX2      : %u\n", info.cpuinfo.avx2);
    printf("  AVX512    : %u\n", info.cpuinfo.avx512);
    printf("  AMX10.1   : %u\n", info.cpuinfo.amx101);
    printf("  AMX10.2   : %u\n", info.cpuinfo.amx102);
    printf("system info:\n");
    printf("  cores     : %u\n", info.cores);

    return 0;
}
