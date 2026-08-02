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

// include OS-specific headers
#ifdef _WIN32
#  include <windows.h>
#else
#  include <unistd.h>
#endif

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
#elif defined(__ARM_NEON)
    // 32-bit ARM: NEON is optional.
    _q->neon = true;
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
    bool has_sse3  = (c & (1u << 0 )) != 0; // SSE3
    bool has_ssse3 = (c & (1u << 9 )) != 0; // SSSE3
    bool has_sse41 = (c & (1u << 19)) != 0; // SSE4.1
    bool has_sse42 = (c & (1u << 20)) != 0; // SSE4.2
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
    _q->sse3    = has_sse3;
    _q->ssse3   = has_ssse3;
    _q->sse41   = has_sse41;
    _q->sse42   = has_sse42;
    _q->avx     = avx_ok;
    _q->fma3    = false;    // TODO: check for this
    _q->avx2    = avx_ok && has_avx2;
    _q->avx512  = has_avx512f;
    _q->amx101  = false;    // TODO: check for this
    _q->amx102  = false;    // TODO: check for this
#else
    // not x86; none are supported
    _q->mmx     = false;
    _q->sse     = false;
    _q->sse2    = false;
    _q->sse3    = false;
    _q->ssse3   = false;
    _q->sse41   = false;
    _q->sse42   = false;
    _q->avx     = false;
    _q->fma3    = false;
    _q->avx2    = false;
    _q->avx512  = false;
    _q->amx101  = false;
    _q->amx102  = false;
#endif
    return LIQUID_OK;
}

int liquid_runtime_supported_ppc(liquid_cpuinfo _q)
{
#if defined __ALTIVEC__ || defined __VEC__
    _q->altivec = true;
#else
    _q->altivec = false;
#endif
    return LIQUID_OK;
}

// check which instruction extensions are supported on this system
int liquid_get_cpuinfo(liquid_cpuinfo _cpu)
{
    if (liquid_runtime_supported_arm(_cpu))
        return liquid_error(LIQUID_EUMODE,"liquid_runtime_supported(), could not get ARM flags");

    if (liquid_runtime_supported_ppc(_cpu))
        return liquid_error(LIQUID_EUMODE,"liquid_runtime_supported(), could not get PPC flags");

    if (liquid_runtime_supported_x86(_cpu))
        return liquid_error(LIQUID_EUMODE,"liquid_runtime_supported(), could not get x86 flags");

    return LIQUID_OK;
}

// get number of cores available
int liquid_runtime_cores(liquid_sysinfo _q)
{
    _q->cores = 1;
#if defined __APPLE__ || defined __linux__  || defined __unix__
    // POSIX (Linux, macOS, BSD) implementation
    long cores = sysconf(_SC_NPROCESSORS_ONLN);
    if (cores < 1)
        return liquid_error(LIQUID_EICONFIG,"liquid_runtime_cores(), error getting POSIX core count with sysconf()");
    _q->cores = (int)cores;
#elif defined _WIN32
    // Windows
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    _q->cores = (int)sysinfo.dwNumberOfProcessors;
#else
    liquid_log_warn("liquid_runtime_cores(), could not get core count; unknown/unsupported OS");
#endif
    return LIQUID_OK;
}

// check which instruction extensions are supported on this system
int liquid_runtime_supported(liquid_sysinfo _q)
{
    if (liquid_get_cpuinfo(&_q->cpuinfo))
        return liquid_error(LIQUID_EUMODE,"liquid_runtime_supported(), could not get cpuinfo");

    if (liquid_runtime_cores(_q))
        return liquid_error(LIQUID_EUMODE,"liquid_runtime_supported(), could not get number of CPU cores");

    return LIQUID_OK;
}

// detect best runtime hardware acceleration mode
liquid_runtime_t liquid_runtime_detect(struct liquid_cpuinfo_s * _impl)
{
    if (_impl == NULL)
    {
        // simply return best mode
        struct liquid_cpuinfo_s impl = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,};
        return liquid_runtime_detect(&impl);
    }

    struct liquid_cpuinfo_s info;
    if (liquid_get_cpuinfo(&info))
    {
        liquid_error(LIQUID_EUMODE,"liquid_runtime_detect(), could not get supported runtime instructions");
        return LIQUID_RUNTIME_UNKNOWN;
    }

    // reverse order from highest priority to lowest
    if (info.amx102  && liquid_build_info.cpuinfo.amx102  && _impl->amx102 ) return LIQUID_RUNTIME_AMX102;
    if (info.amx101  && liquid_build_info.cpuinfo.amx101  && _impl->amx101 ) return LIQUID_RUNTIME_AMX101;
    if (info.avx512  && liquid_build_info.cpuinfo.avx512  && _impl->avx512 ) return LIQUID_RUNTIME_AVX512;
    if (info.avx2    && liquid_build_info.cpuinfo.avx2    && _impl->avx2   ) return LIQUID_RUNTIME_AVX2;
    if (info.fma3    && liquid_build_info.cpuinfo.fma3    && _impl->fma3   ) return LIQUID_RUNTIME_FMA3;
    if (info.avx     && liquid_build_info.cpuinfo.avx     && _impl->avx    ) return LIQUID_RUNTIME_AVX;
    if (info.sse42   && liquid_build_info.cpuinfo.sse42   && _impl->sse42  ) return LIQUID_RUNTIME_SSE42;
    if (info.sse41   && liquid_build_info.cpuinfo.sse41   && _impl->sse41  ) return LIQUID_RUNTIME_SSE41;
    if (info.ssse3   && liquid_build_info.cpuinfo.ssse3   && _impl->ssse3  ) return LIQUID_RUNTIME_SSSE3;
    if (info.sse3    && liquid_build_info.cpuinfo.sse3    && _impl->sse3   ) return LIQUID_RUNTIME_SSE3;
    if (info.sse2    && liquid_build_info.cpuinfo.sse2    && _impl->sse2   ) return LIQUID_RUNTIME_SSE2;
    if (info.sse     && liquid_build_info.cpuinfo.sse     && _impl->sse    ) return LIQUID_RUNTIME_SSE;
    if (info.mmx     && liquid_build_info.cpuinfo.mmx     && _impl->mmx    ) return LIQUID_RUNTIME_MMX;
    if (info.neon    && liquid_build_info.cpuinfo.neon    && _impl->neon   ) return LIQUID_RUNTIME_NEON;
    if (info.altivec && liquid_build_info.cpuinfo.altivec && _impl->altivec) return LIQUID_RUNTIME_ALTIVEC;

    // fall back to portable
    return LIQUID_RUNTIME_PORT;
}

