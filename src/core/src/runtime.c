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

// check runtime architecture support

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "liquid.h"

// check cpuid on x86 architectures
//  _leaf       : the main function number to pass to cpuid to execute
//  _subleaf    : the variant selector for that leaf
int liquid_runtime_cpuid_x86(uint32_t   _leaf,
                             uint32_t   _subleaf,
                             uint32_t * _a,
                             uint32_t * _b,
                             uint32_t * _c,
                             uint32_t * _d)
{
#if defined(__i386__) || defined(__x86_64__)
    __asm__ volatile("cpuid"
      : "=a"(*_a), "=b"(*_b), "=c"(*_c), "=d"(*_d)
      : "a"(_leaf), "c"(_subleaf));
    return LIQUID_OK;
#else
    return liquid_error(LIQUID_EUMODE,"liquid_runtime_cpuid_x86(), cannot run on non-x86 architecture");
#endif
}

// check the extended control register for x86 systems, check XMM and YMM states
uint64_t liquid_runtime_xgetbv_x86(uint32_t _xcr)
{
#if defined(__i386__) || defined(__x86_64__)
    uint32_t eax, edx;
    __asm__ volatile("xgetbv" : "=a"(eax), "=d"(edx) : "c"(_xcr));
    return ((uint64_t)edx << 32) | eax;
#else
    liquid_error(LIQUID_EUMODE,"liquid_runtime_xgetbv_x86(), cannot run on non-x86 architecture");
    return -1;
#endif
}

int liquid_runtime_supported_arm(liquid_cpuinfo _q)
{
//#include <sys/auxv.h>
//#ifdef HWCAP_NEON
//    // HWCAP flags (ARM). NEON presence depends on platform/toolchain.
//    // If your libc headers define HWCAP_NEON, use it
//    unsigned long hwcap = getauxval(AT_HWCAP);
//    if (hwcap & HWCAP_NEON)
//        _q->neon = true;
//#endif

#if defined(__aarch64__)
    // AArch64 requires NEON (Advanced SIMD) by architectural definition.
    _q->neon = true;
#elif defined(__arm__)
    // 32-bit ARM: NEON is optional.
# if defined(__ARM_NEON)
    _q->neon = true;
# else
    _q->neon = false;
# endif
    }
#else
    _q->neon = false;
#endif
    return LIQUID_OK;
}

// check which x86 instruction extensions are supported on this system
int liquid_runtime_supported_x86(liquid_cpuinfo _q)
{
#if defined(__i386__) || defined(__x86_64__)
    uint32_t a,b,c,d;
    if (liquid_runtime_cpuid_x86(1, 0, &a,&b,&c,&d))
        return liquid_error(LIQUID_EUMODE,"liquid_runtime_supported_x86(), could not run cpuid with leaf(1)");

    bool has_sse   = (d & (1u << 25)) != 0; // SSE
    bool has_sse2  = (d & (1u << 26)) != 0; // SSE2
    bool has_mmx   = (d & (1u << 23)) != 0; // MMX
    bool has_avx   = (c & (1u << 28)) != 0; // AVX
    //bool has_fma   = (c & (1u << 12)) != 0; // FMA (not asked, but often tied to AVX2/AVX)
    bool has_osxsave = (c & (1u << 27)) != 0; // check if operating system supports the xsave/xrstor mechanism for extended CPU state

    // AVX requires OS support for YMM state via xgetbv
    bool avx_ok = false;
    if (has_avx && has_osxsave) {
        uint64_t xcr0 = liquid_runtime_xgetbv_x86(0);
        // Usually require XMM (bit 1) and YMM (bit 2) enabled
        avx_ok = ((xcr0 & 0x6) == 0x6);
    }
    
    // Extended flags: AVX2 is in leaf 7
    int has_avx2 = 0;
    int has_avx512f = 0;
    if (liquid_runtime_cpuid_x86(7, 0, &a,&b,&c,&d))
        return liquid_error(LIQUID_EUMODE,"liquid_runtime_supported_x86(), could not run cpuid with leaf(7)");
    has_avx2    = (b & (1u <<  5)) != 0; // AVX2
    has_avx512f = (b & (1u << 16)) != 0; // AVX-512F

    // check specific flags
    _q->mmx     = has_mmx;
    _q->sse     = has_sse;
    _q->sse2    = has_sse2;
    _q->avx     = avx_ok;
    _q->avx2    = avx_ok && has_avx2;
    _q->avx512f = has_avx512f;
#else
    // not x86; none are supported
    _q->sse     = false;
    _q->sse2    = false;
    _q->mmx     = false;
    _q->avx     = false;
    _q->avx2    = false;
    _q->avx512f = false;
#endif
    return LIQUID_OK;
}

// check which instruction extensions are supported on this system
int liquid_runtime_supported(liquid_cpuinfo _q)
{
    if (liquid_runtime_supported_arm(_q))
        return liquid_error(LIQUID_EUMODE,"liquid_runtime_supported(), could not get ARM flags");

    if (liquid_runtime_supported_x86(_q))
        return liquid_error(LIQUID_EUMODE,"liquid_runtime_supported(), could not get ARM flags");

    // TODO: add PowerPC AltiVec checks

    return LIQUID_OK;
}

