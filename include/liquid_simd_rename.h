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
 * liquid_simd_rename.h
 *
 * When LIQUID_SIMD_SUFFIX is defined, renames all SIMD-dispatchable
 * public symbols with a variant suffix (e.g., dotprod_rrrf_create
 * becomes dotprod_rrrf_create_avx). This allows all SIMD variants
 * to coexist in a single binary for runtime dispatch.
 */

#ifndef __LIQUID_SIMD_RENAME_H__
#define __LIQUID_SIMD_RENAME_H__

#ifdef LIQUID_SIMD_SUFFIX

/* Token concatenation helpers.
 * Uses double-underscore separator to avoid collisions with existing
 * internal function names like dotprod_rrrf_execute_sse.
 */
#define LIQUID_SIMD_CAT2(a, b)	a ## __ ## b
#define LIQUID_SIMD_CAT(a, b)	LIQUID_SIMD_CAT2(a, b)
#define LIQUID_SIMD_RENAME(sym)	LIQUID_SIMD_CAT(sym, LIQUID_SIMD_SUFFIX)

/*
 * Note: struct types (dotprod_rrrf_s, etc.) are NOT renamed here.
 * Each variant's .c file defines its own struct layout in a separate
 * compilation unit, so there are no ODR conflicts at link time.
 * The linker only sees function symbols, not struct definitions.
 */

/* ----------------------------------------------------------------
 * dotprod_rrrf
 * ---------------------------------------------------------------- */
#define dotprod_rrrf_run	LIQUID_SIMD_RENAME(dotprod_rrrf_run)
#define dotprod_rrrf_run4	LIQUID_SIMD_RENAME(dotprod_rrrf_run4)
#define dotprod_rrrf_create	LIQUID_SIMD_RENAME(dotprod_rrrf_create)
#define dotprod_rrrf_create_rev	LIQUID_SIMD_RENAME(dotprod_rrrf_create_rev)
#define dotprod_rrrf_create_opt	LIQUID_SIMD_RENAME(dotprod_rrrf_create_opt)
#define dotprod_rrrf_recreate	LIQUID_SIMD_RENAME(dotprod_rrrf_recreate)
#define dotprod_rrrf_recreate_rev LIQUID_SIMD_RENAME(dotprod_rrrf_recreate_rev)
#define dotprod_rrrf_copy	LIQUID_SIMD_RENAME(dotprod_rrrf_copy)
#define dotprod_rrrf_destroy	LIQUID_SIMD_RENAME(dotprod_rrrf_destroy)
#define dotprod_rrrf_print	LIQUID_SIMD_RENAME(dotprod_rrrf_print)
#define dotprod_rrrf_execute	LIQUID_SIMD_RENAME(dotprod_rrrf_execute)

/* ----------------------------------------------------------------
 * dotprod_crcf
 * ---------------------------------------------------------------- */
#define dotprod_crcf_run	LIQUID_SIMD_RENAME(dotprod_crcf_run)
#define dotprod_crcf_run4	LIQUID_SIMD_RENAME(dotprod_crcf_run4)
#define dotprod_crcf_create	LIQUID_SIMD_RENAME(dotprod_crcf_create)
#define dotprod_crcf_create_rev	LIQUID_SIMD_RENAME(dotprod_crcf_create_rev)
#define dotprod_crcf_create_opt	LIQUID_SIMD_RENAME(dotprod_crcf_create_opt)
#define dotprod_crcf_recreate	LIQUID_SIMD_RENAME(dotprod_crcf_recreate)
#define dotprod_crcf_recreate_rev LIQUID_SIMD_RENAME(dotprod_crcf_recreate_rev)
#define dotprod_crcf_copy	LIQUID_SIMD_RENAME(dotprod_crcf_copy)
#define dotprod_crcf_destroy	LIQUID_SIMD_RENAME(dotprod_crcf_destroy)
#define dotprod_crcf_print	LIQUID_SIMD_RENAME(dotprod_crcf_print)
#define dotprod_crcf_execute	LIQUID_SIMD_RENAME(dotprod_crcf_execute)

/* ----------------------------------------------------------------
 * dotprod_cccf
 * ---------------------------------------------------------------- */
#define dotprod_cccf_run	LIQUID_SIMD_RENAME(dotprod_cccf_run)
#define dotprod_cccf_run4	LIQUID_SIMD_RENAME(dotprod_cccf_run4)
#define dotprod_cccf_create	LIQUID_SIMD_RENAME(dotprod_cccf_create)
#define dotprod_cccf_create_rev	LIQUID_SIMD_RENAME(dotprod_cccf_create_rev)
#define dotprod_cccf_create_opt	LIQUID_SIMD_RENAME(dotprod_cccf_create_opt)
#define dotprod_cccf_recreate	LIQUID_SIMD_RENAME(dotprod_cccf_recreate)
#define dotprod_cccf_recreate_rev LIQUID_SIMD_RENAME(dotprod_cccf_recreate_rev)
#define dotprod_cccf_copy	LIQUID_SIMD_RENAME(dotprod_cccf_copy)
#define dotprod_cccf_destroy	LIQUID_SIMD_RENAME(dotprod_cccf_destroy)
#define dotprod_cccf_print	LIQUID_SIMD_RENAME(dotprod_cccf_print)
#define dotprod_cccf_execute	LIQUID_SIMD_RENAME(dotprod_cccf_execute)

/* ----------------------------------------------------------------
 * sumsq
 * ---------------------------------------------------------------- */
#define liquid_sumsqf		LIQUID_SIMD_RENAME(liquid_sumsqf)
#define liquid_sumsqcf		LIQUID_SIMD_RENAME(liquid_sumsqcf)

/*
 * Vector functions are NOT dispatched at runtime because only
 * mul/mulscalar have SIMD variants (AVX for float, NEON for complex).
 * They remain compile-time selected.
 */

#endif /* LIQUID_SIMD_SUFFIX */

#endif /* __LIQUID_SIMD_RENAME_H__ */
