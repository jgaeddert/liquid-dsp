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
 * liquid_simd_dispatch.c
 *
 * Runtime SIMD dispatch via function pointer vtable. Detects CPU
 * capabilities at library load and resolves function pointers to the
 * best available SIMD implementation.
 */

#include <stdio.h>
#include "liquid.h"
#include "liquid_cpuid.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================================================================
 * Forward declarations for ALL variant-suffixed functions.
 * These symbols are produced by compiling each SIMD source file
 * with -DLIQUID_SIMD_SUFFIX=<variant> and including
 * liquid_simd_rename.h.
 * ================================================================== */

/* Helper macro: declare all dotprod functions for a given type and suffix */
#define DECLARE_DOTPROD_RRRF(sfx) \
	extern int dotprod_rrrf_run__##sfx(float*, float*, unsigned int, float*); \
	extern int dotprod_rrrf_run4__##sfx(float*, float*, unsigned int, float*); \
	extern dotprod_rrrf dotprod_rrrf_create__##sfx(float*, unsigned int); \
	extern dotprod_rrrf dotprod_rrrf_create_rev__##sfx(float*, unsigned int); \
	extern dotprod_rrrf dotprod_rrrf_recreate__##sfx(dotprod_rrrf, float*, unsigned int); \
	extern dotprod_rrrf dotprod_rrrf_recreate_rev__##sfx(dotprod_rrrf, float*, unsigned int); \
	extern dotprod_rrrf dotprod_rrrf_copy__##sfx(dotprod_rrrf); \
	extern int dotprod_rrrf_destroy__##sfx(dotprod_rrrf); \
	extern int dotprod_rrrf_print__##sfx(dotprod_rrrf); \
	extern int dotprod_rrrf_execute__##sfx(dotprod_rrrf, float*, float*);

#define DECLARE_DOTPROD_CRCF(sfx) \
	extern int dotprod_crcf_run__##sfx(float*, liquid_float_complex*, unsigned int, liquid_float_complex*); \
	extern int dotprod_crcf_run4__##sfx(float*, liquid_float_complex*, unsigned int, liquid_float_complex*); \
	extern dotprod_crcf dotprod_crcf_create__##sfx(float*, unsigned int); \
	extern dotprod_crcf dotprod_crcf_create_rev__##sfx(float*, unsigned int); \
	extern dotprod_crcf dotprod_crcf_recreate__##sfx(dotprod_crcf, float*, unsigned int); \
	extern dotprod_crcf dotprod_crcf_recreate_rev__##sfx(dotprod_crcf, float*, unsigned int); \
	extern dotprod_crcf dotprod_crcf_copy__##sfx(dotprod_crcf); \
	extern int dotprod_crcf_destroy__##sfx(dotprod_crcf); \
	extern int dotprod_crcf_print__##sfx(dotprod_crcf); \
	extern int dotprod_crcf_execute__##sfx(dotprod_crcf, liquid_float_complex*, liquid_float_complex*);

#define DECLARE_DOTPROD_CCCF(sfx) \
	extern int dotprod_cccf_run__##sfx(liquid_float_complex*, liquid_float_complex*, unsigned int, liquid_float_complex*); \
	extern int dotprod_cccf_run4__##sfx(liquid_float_complex*, liquid_float_complex*, unsigned int, liquid_float_complex*); \
	extern dotprod_cccf dotprod_cccf_create__##sfx(liquid_float_complex*, unsigned int); \
	extern dotprod_cccf dotprod_cccf_create_rev__##sfx(liquid_float_complex*, unsigned int); \
	extern dotprod_cccf dotprod_cccf_recreate__##sfx(dotprod_cccf, liquid_float_complex*, unsigned int); \
	extern dotprod_cccf dotprod_cccf_recreate_rev__##sfx(dotprod_cccf, liquid_float_complex*, unsigned int); \
	extern dotprod_cccf dotprod_cccf_copy__##sfx(dotprod_cccf); \
	extern int dotprod_cccf_destroy__##sfx(dotprod_cccf); \
	extern int dotprod_cccf_print__##sfx(dotprod_cccf); \
	extern int dotprod_cccf_execute__##sfx(dotprod_cccf, liquid_float_complex*, liquid_float_complex*);

#define DECLARE_SUMSQ(sfx) \
	extern float liquid_sumsqf__##sfx(float*, unsigned int); \
	extern float liquid_sumsqcf__##sfx(liquid_float_complex*, unsigned int);

/* Declare all variants (dotprod + sumsq only; vector is compile-time selected) */
#define DECLARE_ALL(sfx) \
	DECLARE_DOTPROD_RRRF(sfx) \
	DECLARE_DOTPROD_CRCF(sfx) \
	DECLARE_DOTPROD_CCCF(sfx) \
	DECLARE_SUMSQ(sfx)

/* Portable variant is always compiled */
DECLARE_ALL(port)

/* x86 variants */
#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
DECLARE_ALL(sse)
DECLARE_ALL(avx)
DECLARE_ALL(avx512f)
#endif

/* ARM variants (no NEON sumsq implementation, only dotprod) */
#if defined(__aarch64__) || defined(_M_ARM64) || defined(__ARM_NEON) || defined(__ARM_NEON__)
DECLARE_DOTPROD_RRRF(neon)
DECLARE_DOTPROD_CRCF(neon)
DECLARE_DOTPROD_CCCF(neon)
#endif

/* ==================================================================
 * Dispatch vtable
 * ================================================================== */

struct liquid_simd_vtable {
	/* dotprod_rrrf */
	int (*dotprod_rrrf_run)(float*, float*, unsigned int, float*);
	int (*dotprod_rrrf_run4)(float*, float*, unsigned int, float*);
	dotprod_rrrf (*dotprod_rrrf_create)(float*, unsigned int);
	dotprod_rrrf (*dotprod_rrrf_create_rev)(float*, unsigned int);
	dotprod_rrrf (*dotprod_rrrf_recreate)(dotprod_rrrf, float*, unsigned int);
	dotprod_rrrf (*dotprod_rrrf_recreate_rev)(dotprod_rrrf, float*, unsigned int);
	dotprod_rrrf (*dotprod_rrrf_copy)(dotprod_rrrf);
	int (*dotprod_rrrf_destroy)(dotprod_rrrf);
	int (*dotprod_rrrf_print)(dotprod_rrrf);
	int (*dotprod_rrrf_execute)(dotprod_rrrf, float*, float*);

	/* dotprod_crcf */
	int (*dotprod_crcf_run)(float*, liquid_float_complex*, unsigned int, liquid_float_complex*);
	int (*dotprod_crcf_run4)(float*, liquid_float_complex*, unsigned int, liquid_float_complex*);
	dotprod_crcf (*dotprod_crcf_create)(float*, unsigned int);
	dotprod_crcf (*dotprod_crcf_create_rev)(float*, unsigned int);
	dotprod_crcf (*dotprod_crcf_recreate)(dotprod_crcf, float*, unsigned int);
	dotprod_crcf (*dotprod_crcf_recreate_rev)(dotprod_crcf, float*, unsigned int);
	dotprod_crcf (*dotprod_crcf_copy)(dotprod_crcf);
	int (*dotprod_crcf_destroy)(dotprod_crcf);
	int (*dotprod_crcf_print)(dotprod_crcf);
	int (*dotprod_crcf_execute)(dotprod_crcf, liquid_float_complex*, liquid_float_complex*);

	/* dotprod_cccf */
	int (*dotprod_cccf_run)(liquid_float_complex*, liquid_float_complex*, unsigned int, liquid_float_complex*);
	int (*dotprod_cccf_run4)(liquid_float_complex*, liquid_float_complex*, unsigned int, liquid_float_complex*);
	dotprod_cccf (*dotprod_cccf_create)(liquid_float_complex*, unsigned int);
	dotprod_cccf (*dotprod_cccf_create_rev)(liquid_float_complex*, unsigned int);
	dotprod_cccf (*dotprod_cccf_recreate)(dotprod_cccf, liquid_float_complex*, unsigned int);
	dotprod_cccf (*dotprod_cccf_recreate_rev)(dotprod_cccf, liquid_float_complex*, unsigned int);
	dotprod_cccf (*dotprod_cccf_copy)(dotprod_cccf);
	int (*dotprod_cccf_destroy)(dotprod_cccf);
	int (*dotprod_cccf_print)(dotprod_cccf);
	int (*dotprod_cccf_execute)(dotprod_cccf, liquid_float_complex*, liquid_float_complex*);

	/* sumsq */
	float (*liquid_sumsqf)(float*, unsigned int);
	float (*liquid_sumsqcf)(liquid_float_complex*, unsigned int);
};

static struct liquid_simd_vtable vtable;
static volatile int vtable_initialized = 0;
static liquid_simd_level current_level = LIQUID_SIMD_NONE;

/* ==================================================================
 * Resolve: populate vtable for a given SIMD level
 * ================================================================== */

/* Macro to set all vtable entries for a given suffix */
#define RESOLVE_DOTPROD(sfx) do { \
	vtable.dotprod_rrrf_run        = dotprod_rrrf_run__##sfx; \
	vtable.dotprod_rrrf_run4       = dotprod_rrrf_run4__##sfx; \
	vtable.dotprod_rrrf_create     = dotprod_rrrf_create__##sfx; \
	vtable.dotprod_rrrf_create_rev = dotprod_rrrf_create_rev__##sfx; \
	vtable.dotprod_rrrf_recreate   = dotprod_rrrf_recreate__##sfx; \
	vtable.dotprod_rrrf_recreate_rev = dotprod_rrrf_recreate_rev__##sfx; \
	vtable.dotprod_rrrf_copy       = dotprod_rrrf_copy__##sfx; \
	vtable.dotprod_rrrf_destroy    = dotprod_rrrf_destroy__##sfx; \
	vtable.dotprod_rrrf_print      = dotprod_rrrf_print__##sfx; \
	vtable.dotprod_rrrf_execute    = dotprod_rrrf_execute__##sfx; \
	vtable.dotprod_crcf_run        = dotprod_crcf_run__##sfx; \
	vtable.dotprod_crcf_run4       = dotprod_crcf_run4__##sfx; \
	vtable.dotprod_crcf_create     = dotprod_crcf_create__##sfx; \
	vtable.dotprod_crcf_create_rev = dotprod_crcf_create_rev__##sfx; \
	vtable.dotprod_crcf_recreate   = dotprod_crcf_recreate__##sfx; \
	vtable.dotprod_crcf_recreate_rev = dotprod_crcf_recreate_rev__##sfx; \
	vtable.dotprod_crcf_copy       = dotprod_crcf_copy__##sfx; \
	vtable.dotprod_crcf_destroy    = dotprod_crcf_destroy__##sfx; \
	vtable.dotprod_crcf_print      = dotprod_crcf_print__##sfx; \
	vtable.dotprod_crcf_execute    = dotprod_crcf_execute__##sfx; \
	vtable.dotprod_cccf_run        = dotprod_cccf_run__##sfx; \
	vtable.dotprod_cccf_run4       = dotprod_cccf_run4__##sfx; \
	vtable.dotprod_cccf_create     = dotprod_cccf_create__##sfx; \
	vtable.dotprod_cccf_create_rev = dotprod_cccf_create_rev__##sfx; \
	vtable.dotprod_cccf_recreate   = dotprod_cccf_recreate__##sfx; \
	vtable.dotprod_cccf_recreate_rev = dotprod_cccf_recreate_rev__##sfx; \
	vtable.dotprod_cccf_copy       = dotprod_cccf_copy__##sfx; \
	vtable.dotprod_cccf_destroy    = dotprod_cccf_destroy__##sfx; \
	vtable.dotprod_cccf_print      = dotprod_cccf_print__##sfx; \
	vtable.dotprod_cccf_execute    = dotprod_cccf_execute__##sfx; \
} while (0)

#define RESOLVE_SUMSQ(sfx) do { \
	vtable.liquid_sumsqf           = liquid_sumsqf__##sfx; \
	vtable.liquid_sumsqcf          = liquid_sumsqcf__##sfx; \
} while (0)

#define RESOLVE_ALL(sfx) do { \
	RESOLVE_DOTPROD(sfx); \
	RESOLVE_SUMSQ(sfx); \
} while (0)

static void liquid_simd_resolve(liquid_simd_level level)
{
	switch (level) {
#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
	case LIQUID_SIMD_AVX512F:
		RESOLVE_ALL(avx512f);
		break;
	case LIQUID_SIMD_AVX2:
		RESOLVE_ALL(avx);
		break;
	case LIQUID_SIMD_AVX:
	case LIQUID_SIMD_SSE42:
		/* AVX variant is compiled with AVX2+FMA, so AVX-only CPUs use SSE */
		RESOLVE_ALL(sse);
		break;
#endif
#if defined(__aarch64__) || defined(_M_ARM64) || defined(__ARM_NEON) || defined(__ARM_NEON__)
	case LIQUID_SIMD_NEON:
		RESOLVE_DOTPROD(neon);
		RESOLVE_SUMSQ(port);
		break;
#endif
	default:
		RESOLVE_ALL(port);
		break;
	}
	current_level = level;
}

/* ==================================================================
 * Initialization
 * ================================================================== */

static void liquid_simd_init(void)
{
	if (vtable_initialized)
		return;
	liquid_simd_level level = liquid_cpu_detect();
	liquid_simd_resolve(level);
	vtable_initialized = 1;
}

/* Auto-init at library load time */
#if defined(_MSC_VER)
/* MSVC: CRT initializer section */
#pragma section(".CRT$XCU", read)
static void liquid_simd_init_msvc(void) { liquid_simd_init(); }
__declspec(allocate(".CRT$XCU"))
void (*liquid_simd_init_ptr)(void) = liquid_simd_init_msvc;
#elif defined(__GNUC__) || defined(__clang__)
static void __attribute__((constructor)) liquid_simd_init_ctor(void)
{
	liquid_simd_init();
}
#endif

/* Ensure init (fallback for platforms without constructor support) */
static inline void liquid_simd_ensure_init(void)
{
	if (!vtable_initialized)
		liquid_simd_init();
}

/* ==================================================================
 * Public API: level management
 * ================================================================== */

liquid_simd_level liquid_simd_get_level(void)
{
	liquid_simd_ensure_init();
	return current_level;
}

int liquid_simd_override(liquid_simd_level _level)
{
	liquid_simd_resolve(_level);
	vtable_initialized = 1;
	return 0;
}

/* ==================================================================
 * Public API wrappers: thin functions delegating through vtable
 * ================================================================== */

/* --- dotprod_rrrf --- */
int dotprod_rrrf_run(float *_h, float *_x, unsigned int _n, float *_y) {
	liquid_simd_ensure_init();
	return vtable.dotprod_rrrf_run(_h, _x, _n, _y);
}
int dotprod_rrrf_run4(float *_h, float *_x, unsigned int _n, float *_y) {
	liquid_simd_ensure_init();
	return vtable.dotprod_rrrf_run4(_h, _x, _n, _y);
}
dotprod_rrrf dotprod_rrrf_create(float *_h, unsigned int _n) {
	liquid_simd_ensure_init();
	return vtable.dotprod_rrrf_create(_h, _n);
}
dotprod_rrrf dotprod_rrrf_create_rev(float *_h, unsigned int _n) {
	liquid_simd_ensure_init();
	return vtable.dotprod_rrrf_create_rev(_h, _n);
}
dotprod_rrrf dotprod_rrrf_recreate(dotprod_rrrf _q, float *_h, unsigned int _n) {
	return vtable.dotprod_rrrf_recreate(_q, _h, _n);
}
dotprod_rrrf dotprod_rrrf_recreate_rev(dotprod_rrrf _q, float *_h, unsigned int _n) {
	return vtable.dotprod_rrrf_recreate_rev(_q, _h, _n);
}
dotprod_rrrf dotprod_rrrf_copy(dotprod_rrrf _q) {
	return vtable.dotprod_rrrf_copy(_q);
}
int dotprod_rrrf_destroy(dotprod_rrrf _q) {
	return vtable.dotprod_rrrf_destroy(_q);
}
int dotprod_rrrf_print(dotprod_rrrf _q) {
	return vtable.dotprod_rrrf_print(_q);
}
int dotprod_rrrf_execute(dotprod_rrrf _q, float *_x, float *_y) {
	return vtable.dotprod_rrrf_execute(_q, _x, _y);
}

/* --- dotprod_crcf --- */
int dotprod_crcf_run(float *_h, liquid_float_complex *_x, unsigned int _n, liquid_float_complex *_y) {
	liquid_simd_ensure_init();
	return vtable.dotprod_crcf_run(_h, _x, _n, _y);
}
int dotprod_crcf_run4(float *_h, liquid_float_complex *_x, unsigned int _n, liquid_float_complex *_y) {
	liquid_simd_ensure_init();
	return vtable.dotprod_crcf_run4(_h, _x, _n, _y);
}
dotprod_crcf dotprod_crcf_create(float *_h, unsigned int _n) {
	liquid_simd_ensure_init();
	return vtable.dotprod_crcf_create(_h, _n);
}
dotprod_crcf dotprod_crcf_create_rev(float *_h, unsigned int _n) {
	liquid_simd_ensure_init();
	return vtable.dotprod_crcf_create_rev(_h, _n);
}
dotprod_crcf dotprod_crcf_recreate(dotprod_crcf _q, float *_h, unsigned int _n) {
	return vtable.dotprod_crcf_recreate(_q, _h, _n);
}
dotprod_crcf dotprod_crcf_recreate_rev(dotprod_crcf _q, float *_h, unsigned int _n) {
	return vtable.dotprod_crcf_recreate_rev(_q, _h, _n);
}
dotprod_crcf dotprod_crcf_copy(dotprod_crcf _q) {
	return vtable.dotprod_crcf_copy(_q);
}
int dotprod_crcf_destroy(dotprod_crcf _q) {
	return vtable.dotprod_crcf_destroy(_q);
}
int dotprod_crcf_print(dotprod_crcf _q) {
	return vtable.dotprod_crcf_print(_q);
}
int dotprod_crcf_execute(dotprod_crcf _q, liquid_float_complex *_x, liquid_float_complex *_y) {
	return vtable.dotprod_crcf_execute(_q, _x, _y);
}

/* --- dotprod_cccf --- */
int dotprod_cccf_run(liquid_float_complex *_h, liquid_float_complex *_x, unsigned int _n, liquid_float_complex *_y) {
	liquid_simd_ensure_init();
	return vtable.dotprod_cccf_run(_h, _x, _n, _y);
}
int dotprod_cccf_run4(liquid_float_complex *_h, liquid_float_complex *_x, unsigned int _n, liquid_float_complex *_y) {
	liquid_simd_ensure_init();
	return vtable.dotprod_cccf_run4(_h, _x, _n, _y);
}
dotprod_cccf dotprod_cccf_create(liquid_float_complex *_h, unsigned int _n) {
	liquid_simd_ensure_init();
	return vtable.dotprod_cccf_create(_h, _n);
}
dotprod_cccf dotprod_cccf_create_rev(liquid_float_complex *_h, unsigned int _n) {
	liquid_simd_ensure_init();
	return vtable.dotprod_cccf_create_rev(_h, _n);
}
dotprod_cccf dotprod_cccf_recreate(dotprod_cccf _q, liquid_float_complex *_h, unsigned int _n) {
	return vtable.dotprod_cccf_recreate(_q, _h, _n);
}
dotprod_cccf dotprod_cccf_recreate_rev(dotprod_cccf _q, liquid_float_complex *_h, unsigned int _n) {
	return vtable.dotprod_cccf_recreate_rev(_q, _h, _n);
}
dotprod_cccf dotprod_cccf_copy(dotprod_cccf _q) {
	return vtable.dotprod_cccf_copy(_q);
}
int dotprod_cccf_destroy(dotprod_cccf _q) {
	return vtable.dotprod_cccf_destroy(_q);
}
int dotprod_cccf_print(dotprod_cccf _q) {
	return vtable.dotprod_cccf_print(_q);
}
int dotprod_cccf_execute(dotprod_cccf _q, liquid_float_complex *_x, liquid_float_complex *_y) {
	return vtable.dotprod_cccf_execute(_q, _x, _y);
}

/* --- sumsq --- */
float liquid_sumsqf(float *_v, unsigned int _n) {
	liquid_simd_ensure_init();
	return vtable.liquid_sumsqf(_v, _n);
}
float liquid_sumsqcf(liquid_float_complex *_v, unsigned int _n) {
	liquid_simd_ensure_init();
	return vtable.liquid_sumsqcf(_v, _n);
}

/* Note: vector functions (liquid_vectorf_*, liquid_vectorcf_*) are NOT dispatched.
 * They use compile-time SIMD selection since only mul/mulscalar have SIMD variants.
 */

#ifdef __cplusplus
}
#endif
