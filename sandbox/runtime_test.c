char __docstr__[] = "Test executing architecture-specific code at runtime";

#include <complex.h>
#include <math.h>
#include <stdio.h>
#include <unistd.h>

#include "liquid.h"
#include "liquid.argparse.h"

// custom object, e.g. dotprod
typedef struct object_s * object;
struct object_s
{
    // function pointer to execute method
    int (*execute)(void);
};
object object_create(void);
int    object_destroy(object);
int    object_execute(object);

//
int main(int argc, char*argv[])
{
    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    liquid_argparse_parse(argc,argv);

    // invoke platform-specific method
    //return runtime();
    object q = object_create();
    object_execute(q);
    object_destroy(q);

    return LIQUID_OK;
}

// platform-specific funcitons and portable fallback
int execute_neon(void)    { printf("execute_neon\n");    return LIQUID_OK; }
int execute_sse(void)     { printf("execute_sse\n");     return LIQUID_OK; }
int execute_sse2(void)    { printf("execute_sse2\n");    return LIQUID_OK; }
int execute_mmx(void)     { printf("execute_mmx\n");     return LIQUID_OK; }
int execute_avx(void)     { printf("execute_avx\n");     return LIQUID_OK; }
int execute_avx2(void)    { printf("execute_avx2\n");    return LIQUID_OK; }
int execute_avx512f(void) { printf("execute_avx512f\n"); return LIQUID_OK; }
int execute_altivec(void) { printf("execute_altivec\n"); return LIQUID_OK; }
int execute_port(void)    { printf("execute_port\n");    return LIQUID_OK; }

object object_create(void)
{
    object _q = (object)malloc(sizeof(struct object_s));

    // set execute function pointer based on run-time availability
    struct liquid_cpuinfo_s info;
    liquid_runtime_supported(&info);

    if      (info.neon)    { _q->execute = &execute_neon;    }
    else if (info.avx512f) { _q->execute = &execute_avx512f; }
    else if (info.avx2)    { _q->execute = &execute_avx2;    }
    else if (info.avx)     { _q->execute = &execute_avx;     }
    else if (info.mmx)     { _q->execute = &execute_mmx;     }
    else if (info.sse2)    { _q->execute = &execute_sse2;    }
    else if (info.sse)     { _q->execute = &execute_sse;     }
    else if (info.altivec) { _q->execute = &execute_altivec; }
    else                   { _q->execute = &execute_port;    }

    return _q;
}

int object_destroy(object _q)
{
    free(_q);
    return LIQUID_OK;
}

int object_execute(object _q)
{
    return _q->execute();
}

