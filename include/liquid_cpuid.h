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
 * liquid_cpuid.h
 *
 * Runtime CPU feature detection and SIMD dispatch level management.
 */

#ifndef __LIQUID_CPUID_H__
#define __LIQUID_CPUID_H__

#ifdef __cplusplus
extern "C" {
#endif

/* SIMD instruction set levels, ordered by capability */
typedef enum {
	LIQUID_SIMD_NONE    = 0,	/* portable C */
	LIQUID_SIMD_SSE42   = 1,	/* x86 SSE4.2 */
	LIQUID_SIMD_AVX     = 2,	/* x86 AVX */
	LIQUID_SIMD_AVX2    = 3,	/* x86 AVX2 + FMA */
	LIQUID_SIMD_AVX512F = 4,	/* x86 AVX-512F */
	LIQUID_SIMD_NEON    = 5,	/* ARM NEON */
} liquid_simd_level;

/* Detect the highest SIMD level supported by the current CPU */
liquid_simd_level liquid_cpu_detect(void);

/* Get the currently active dispatch level */
liquid_simd_level liquid_simd_get_level(void);

/*
 * Override the dispatch level. Returns 0 on success, -1 if the
 * requested level is not supported by the compiled binary.
 *
 * WARNING: All objects created with create/recreate must be destroyed
 * before calling this function. Objects created at one level must not
 * be used with a different level (different memory allocators).
 */
int liquid_simd_override(liquid_simd_level _level);

/* Get a human-readable string for a SIMD level */
const char *liquid_simd_level_str(liquid_simd_level _level);

#ifdef __cplusplus
}
#endif

#endif /* __LIQUID_CPUID_H__ */
