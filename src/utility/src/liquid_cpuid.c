/*
 * Copyright (c) 2026 Benjamin Vernoux
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

/*
 * liquid_cpuid.c
 *
 * Runtime CPU feature detection for SIMD dispatch.
 * Supports: x86/x64 (MSVC, GCC, MinGW, Clang) and ARM (Linux, macOS).
 */

#include "liquid_cpuid.h"

/* ----------------------------------------------------------------
 * x86/x64 CPUID detection
 * ---------------------------------------------------------------- */
#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
#define LIQUID_ARCH_X86 1

#if defined(_MSC_VER)
#include <intrin.h>
/* MSVC intrinsics: __cpuid, __cpuidex, _xgetbv */
static void liquid_cpuid(int info[4], int leaf)
{
	__cpuid(info, leaf);
}

static void liquid_cpuidex(int info[4], int leaf, int subleaf)
{
	__cpuidex(info, leaf, subleaf);
}

static unsigned long long liquid_xgetbv(unsigned int xcr)
{
	return _xgetbv(xcr);
}

#elif defined(__GNUC__) || defined(__clang__)
#include <cpuid.h>

static void liquid_cpuid(int info[4], int leaf)
{
	__cpuid(leaf,
		info[0], info[1], info[2], info[3]);
}

static void liquid_cpuidex(int info[4], int leaf, int subleaf)
{
	__cpuid_count(leaf, subleaf,
		info[0], info[1], info[2], info[3]);
}

static unsigned long long liquid_xgetbv(unsigned int xcr)
{
	unsigned int eax, edx;
	__asm__ __volatile__(
		"xgetbv"
		: "=a"(eax), "=d"(edx)
		: "c"(xcr));
	return ((unsigned long long)edx << 32) | eax;
}
#endif /* compiler */

liquid_simd_level liquid_cpu_detect(void)
{
	int info[4];
	liquid_simd_level level = LIQUID_SIMD_NONE;

	/* Check basic CPUID support */
	liquid_cpuid(info, 0);
	int max_leaf = info[0];
	if (max_leaf < 1)
		return LIQUID_SIMD_NONE;

	/* Leaf 1: SSE4.2, OSXSAVE, AVX */
	liquid_cpuid(info, 1);
	int ecx1 = info[2];

	int has_sse42   = (ecx1 >> 20) & 1;	/* SSE4.2 */
	int has_osxsave = (ecx1 >> 27) & 1;	/* OSXSAVE (OS supports xgetbv) */
	int has_avx     = (ecx1 >> 28) & 1;	/* AVX */

	if (has_sse42)
		level = LIQUID_SIMD_SSE42;

	if (!has_osxsave || !has_avx)
		return level;

	/* Check OS support for AVX (XCR0 bits 1-2: XMM + YMM state) */
	unsigned long long xcr0 = liquid_xgetbv(0);
	if ((xcr0 & 0x06) != 0x06)
		return level;

	level = LIQUID_SIMD_AVX;

	/* Leaf 7: AVX2, AVX-512 */
	if (max_leaf >= 7) {
		liquid_cpuidex(info, 7, 0);
		int ebx7 = info[1];

		int has_avx2    = (ebx7 >>  5) & 1;
		int has_avx512f = (ebx7 >> 16) & 1;

		if (has_avx2)
			level = LIQUID_SIMD_AVX2;

		/* Check OS support for AVX-512 (XCR0 bits 5-7: opmask, ZMM_Hi256, Hi16_ZMM) */
		if (has_avx512f && ((xcr0 & 0xE0) == 0xE0))
			level = LIQUID_SIMD_AVX512F;
	}

	return level;
}

/* ----------------------------------------------------------------
 * ARM NEON detection
 * ---------------------------------------------------------------- */
#elif defined(__aarch64__) || defined(_M_ARM64) || defined(__ARM_NEON) || defined(__ARM_NEON__)
#define LIQUID_ARCH_ARM 1

#if defined(__APPLE__)
/* Apple Silicon: NEON is always available on aarch64 */
liquid_simd_level liquid_cpu_detect(void)
{
	return LIQUID_SIMD_NEON;
}

#elif defined(__linux__)
#include <sys/auxv.h>
#include <asm/hwcap.h>

liquid_simd_level liquid_cpu_detect(void)
{
#if defined(__aarch64__)
	/* NEON is mandatory on aarch64 */
	return LIQUID_SIMD_NEON;
#else
	/* ARMv7: check HWCAP for NEON */
	unsigned long hwcap = getauxval(AT_HWCAP);
	if (hwcap & HWCAP_NEON)
		return LIQUID_SIMD_NEON;
	return LIQUID_SIMD_NONE;
#endif
}

#else
/* Other ARM platforms: assume NEON on aarch64, none otherwise */
liquid_simd_level liquid_cpu_detect(void)
{
#if defined(__aarch64__) || defined(_M_ARM64)
	return LIQUID_SIMD_NEON;
#else
	return LIQUID_SIMD_NONE;
#endif
}
#endif /* ARM OS */

/* ----------------------------------------------------------------
 * Fallback: no SIMD
 * ---------------------------------------------------------------- */
#else

liquid_simd_level liquid_cpu_detect(void)
{
	return LIQUID_SIMD_NONE;
}

#endif /* architecture */

/* ----------------------------------------------------------------
 * String representation
 * ---------------------------------------------------------------- */
const char *liquid_simd_level_str(liquid_simd_level _level)
{
	switch (_level) {
	case LIQUID_SIMD_NONE:    return "none (portable)";
	case LIQUID_SIMD_SSE42:   return "SSE4.2";
	case LIQUID_SIMD_AVX:     return "AVX";
	case LIQUID_SIMD_AVX2:    return "AVX2+FMA";
	case LIQUID_SIMD_AVX512F: return "AVX-512F";
	case LIQUID_SIMD_NEON:    return "NEON";
	default:                  return "unknown";
	}
}
