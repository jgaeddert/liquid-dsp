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

// Run-time library version numbers

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "liquid.internal.h"

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
    // version information
    .version            = LIQUID_VERSION,
    .githash            = "unknown",
    .tag                = false,

    // date/time of build
    .build_datetime     = "unknown",
    .build_hostname     = "unknown",
    .build_os           = "unknown",
    .build_arch         = "unknown",
    .build_toolchain    = "unknown",

    //
    .target_os          = "unknown",
    .target_arch        = "unknown",

    //
    .build_type         = "unknown",

    // library metadata
    .author             = "Joseph D. Gaeddert <joseph@liquidsdr.org>",
    .license            = "MIT/X11",
    .url                = "https://liquidsdr.org",

    // SIMD support
};

int liquid_build_info_print(void)
{
    printf("version         = %s\n", liquid_build_info.version);
    printf("githash         = %s\n", liquid_build_info.githash);
    printf("tag             = %s\n", liquid_build_info.tag ? "true" : "false");
    printf("build_datetime  = %s\n", liquid_build_info.build_datetime);
    printf("build_hostname  = %s\n", liquid_build_info.build_hostname);
    printf("build_os        = %s\n", liquid_build_info.build_os);
    printf("build_arch      = %s\n", liquid_build_info.build_arch);
    printf("build_toolchain = %s\n", liquid_build_info.build_toolchain);
    printf("target_os       = %s\n", liquid_build_info.target_os);
    printf("target_arch     = %s\n", liquid_build_info.target_arch);
    printf("build_type      = %s\n", liquid_build_info.build_type);
    printf("author          = %s\n", liquid_build_info.author);
    printf("license         = %s\n", liquid_build_info.license);
    printf("url             = %s\n", liquid_build_info.url);

    // SIMD support
    return LIQUID_OK;
}

