char __docstr__[] = "Test executing architecture-specific code at runtime";

#include <complex.h>
#include <math.h>
#include <stdio.h>
#include <unistd.h>

#include "liquid.h"
#include "liquid.argparse.h"

// launch point
int runtime(void);

// platform-specific funcitons and portable fallback
int runtime_port(void);
int runtime_neon(void);
int runtime_sse (void);
int runtime_sse2(void);
int runtime_mmx (void);
int runtime_avx (void);
int runtime_avx2(void);
int runtime_avx512f(void);
int runtime_altivec(void);

//
int main(int argc, char*argv[])
{
    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    liquid_argparse_parse(argc,argv);

    // invoke platform-specific method
    return runtime();
}

// launch point
int runtime(void)
{
    struct liquid_cpuinfo_s info;
    liquid_runtime_supported(&info);

    if (info.neon)    return runtime_neon();
    if (info.avx512f) return runtime_avx512f();
    if (info.avx2)    return runtime_avx2();
    if (info.avx)     return runtime_avx();
    if (info.mmx)     return runtime_mmx();
    if (info.sse2)    return runtime_sse2();
    if (info.sse)     return runtime_sse();
    if (info.altivec) return runtime_altivec();

    // fall back to portable version
    return runtime_port();
}

int runtime_neon(void)    { printf("runtime_neon\n");    return LIQUID_OK; }
int runtime_sse(void)     { printf("runtime_sse\n");     return LIQUID_OK; }
int runtime_sse2(void)    { printf("runtime_sse2\n");    return LIQUID_OK; }
int runtime_mmx(void)     { printf("runtime_mmx\n");     return LIQUID_OK; }
int runtime_avx(void)     { printf("runtime_avx\n");     return LIQUID_OK; }
int runtime_avx2(void)    { printf("runtime_avx2\n");    return LIQUID_OK; }
int runtime_avx512f(void) { printf("runtime_avx512f\n"); return LIQUID_OK; }
int runtime_altivec(void) { printf("runtime_altivec\n"); return LIQUID_OK; }
int runtime_port(void)    { printf("runtime_port\n");    return LIQUID_OK; }

