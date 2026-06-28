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

// 'liquid_build_info' is templated in cmake/build_info.c.in and populated
// into 'build_info.c' by cmake when the command is invoked.
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

    // target options
    printf("target_os       = %s\n", liquid_build_info.target_os);
    printf("target_arch     = %s\n", liquid_build_info.target_arch);

    // TODO: other compile-time options (logging, SIMD, etc.)

    return LIQUID_OK;
}

