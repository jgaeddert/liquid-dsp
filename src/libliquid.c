/*
 * Copyright (c) 2007 - 2026 Joseph Gaeddert
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

// Compile-time library version numbers

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "liquid.h"
#include "liquid.config.h"

const char liquid_version[] = LIQUID_VERSION;

const char * liquid_libversion(void)
{
    return LIQUID_VERSION;
}

int liquid_libversion_number(void)
{
    return LIQUID_VERSION_NUMBER;
}

const struct liquid_build_info_s liquid_build_info =
{
    // library metadata
    .copyright          = PROJECT_COPYRIGHT,
    .license            = PROJECT_LICENSE,
    //.author             = PROJECT_AUTHOR,
    //.homepage           = PROJECT_HOMEPAGE,
    //.description        = PROJECT_DESCRIPTION,

    // version information
    .version            = LIQUID_VERSION, // PROJECT_VERSION
    .githash            = BUILD_GITHASH,

    // date/time of build
    .build_datetime     = BUILD_DATETIME,
    .build_hostname     = BUILD_HOSTNAME,
    .build_os           = BUILD_OS,
    .build_arch         = BUILD_ARCH,
    .build_toolchain    = BUILD_TOOLCHAIN,
    .build_type         = BUILD_TYPE,
    .build_env          = BUILD_ENV, // cmake, autotools, etc.

    // target platform
    .target_os          = TARGET_OS,
    .target_arch        = TARGET_ARCH,

    // other compile-time options
    .logging_enabled    = LOGGING_ENABLED,
    .logging_level      = LOGGING_LEVEL,
    .color_enabled      = COLOR_ENABLED,

    // vector extensions
    .cpuinfo = {
        .altivec        = BUILD_ALTIVEC,
        .neon           = BUILD_NEON,
        .mmx            = 0,
        .sse            = BUILD_SSE,
        .sse2           = BUILD_SSE2,
        .sse3           = BUILD_SSE3,
        .ssse3          = BUILD_SSSE3,
        .sse41          = BUILD_SSE41,
        .sse42          = BUILD_SSE42,
        .avx            = BUILD_AVX,
        .fma3           = 0,
        .avx2           = BUILD_AVX2,
        .avx512         = BUILD_AVX512,
        .amx            = 0,
        .amx101         = 0,
        .amx102         = 0,
    },
};

int liquid_build_info_print(void)
{
    // version information
    printf("copyright       = %s\n", liquid_build_info.copyright);
    printf("license         = %s\n", liquid_build_info.license);
    //printf("author          = %s\n", liquid_build_info.author);
    //printf("homepage        = %s\n", liquid_build_info.homepage);
    //printf("description     = %s\n", liquid_build_info.description);

    // version information
    printf("version         = %s\n", liquid_build_info.version);
    printf("githash         = %s\n", liquid_build_info.githash);

    // date/time of build
    printf("build_datetime  = %s\n", liquid_build_info.build_datetime);
    printf("build_hostname  = %s\n", liquid_build_info.build_hostname);
    printf("build_os        = %s\n", liquid_build_info.build_os);
    printf("build_arch      = %s\n", liquid_build_info.build_arch);
    printf("build_toolchain = %s\n", liquid_build_info.build_toolchain);
    printf("build_type      = %s\n", liquid_build_info.build_type);
    printf("build_env       = %s\n", liquid_build_info.build_env);

    // target options
    printf("target_os       = %s\n", liquid_build_info.target_os);
    printf("target_arch     = %s\n", liquid_build_info.target_arch);

    // other compile-time options
    printf("logging_enabled = %d\n", liquid_build_info.logging_enabled);
    printf("logging_level   = %d\n", liquid_build_info.logging_level);
    printf("color_enabled   = %d\n", liquid_build_info.color_enabled);

    // vector extensions
    printf("altivec         = %d\n", liquid_build_info.cpuinfo.altivec);
    printf("neon            = %d\n", liquid_build_info.cpuinfo.neon);
    printf("mmx             = %d\n", liquid_build_info.cpuinfo.mmx);
    printf("sse             = %d\n", liquid_build_info.cpuinfo.sse);
    printf("sse2            = %d\n", liquid_build_info.cpuinfo.sse2);
    printf("sse3            = %d\n", liquid_build_info.cpuinfo.sse3);
    printf("ssse3           = %d\n", liquid_build_info.cpuinfo.ssse3);
    printf("sse4.1          = %d\n", liquid_build_info.cpuinfo.sse41);
    printf("sse4.2          = %d\n", liquid_build_info.cpuinfo.sse42);
    printf("avx             = %d\n", liquid_build_info.cpuinfo.avx);
    printf("fma3            = %d\n", liquid_build_info.cpuinfo.fma3);
    printf("avx2            = %d\n", liquid_build_info.cpuinfo.avx2);
    printf("avx512          = %d\n", liquid_build_info.cpuinfo.avx512);
    printf("amx             = %d\n", liquid_build_info.cpuinfo.amx);
    printf("amx10.1         = %d\n", liquid_build_info.cpuinfo.amx101);
    printf("amx10.2         = %d\n", liquid_build_info.cpuinfo.amx102);

    return LIQUID_OK;
}

