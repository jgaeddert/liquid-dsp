/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */
/* This is the default config.h to be included without running bootstrap and configure */

#ifndef __LIQUID_CONFIG_H__
#define __LIQUID_CONFIG_H__


/* Support AVX (Advanced Vector Extensions) instructions */
/* #undef HAVE_AVX */

/* Define to 1 if you have the `cargf' function. */
#define HAVE_CARGF 1

/* Define to 1 if you have the `cexpf' function. */
#define HAVE_CEXPF 1

/* Define to 1 if you have the `cimagf' function. */
#define HAVE_CIMAGF 1

/* Define to 1 if you have the <complex.h> header file. */
#define HAVE_COMPLEX_H 1

/* Define to 1 if you have the `cosf' function. */
#define HAVE_COSF 1

/* Define to 1 if you have the `crealf' function. */
#define HAVE_CREALF 1

/* Define to 1 if you have the <emmintrin.h> header file. */
/* #undef HAVE_EMMINTRIN_H */

/* Define to 1 if you have the `expf' function. */
#define HAVE_EXPF 1

/* Define to 1 if you have the <fec.h> header file. */
/* #undef HAVE_FEC_H */

/* Define to 1 if you have the <fftw3.h> header file. */
/* #undef HAVE_FFTW3_H */

/* Define to 1 if you have the <float.h> header file. */
#define HAVE_FLOAT_H 1

/* Define to 1 if you have the `free' function. */
#define HAVE_FREE 1

/* Define to 1 if you have the <getopt.h> header file. */
#define HAVE_GETOPT_H 1

/* Define to 1 if you have the <immintrin.h> header file. */
/* #undef HAVE_IMMINTRIN_H */

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the `c' library (-lc). */
#define HAVE_LIBC 1

/* Define to 1 if you have the `fec' library (-lfec). */
/* #undef HAVE_LIBFEC */

/* Define to 1 if you have the `fftw3f' library (-lfftw3f). */
#define HAVE_LIBFFTW3F 1

/* Define to 1 if you have the `m' library (-lm). */
#define HAVE_LIBM 1

/* Define to 1 if you have the <limits.h> header file. */
#define HAVE_LIMITS_H 1

/* Define to 1 if you have the `malloc' function. */
#define HAVE_MALLOC 1

/* Define to 1 if you have the `memmove' function. */
#define HAVE_MEMMOVE 1

/* Define to 1 if you have the `memset' function. */
#define HAVE_MEMSET 1

/* Define to 1 if you have the <mmintrin.h> header file. */
/* #undef HAVE_MMINTRIN_H */

/* Support MMX instructions */
/* #undef HAVE_MMX */

/* Define to 1 if you have the <pmmintrin.h> header file. */
/* #undef HAVE_PMMINTRIN_H */

/* Define to 1 if you have the `realloc' function. */
#define HAVE_REALLOC 1

/* Define to 1 if you have the `sinf' function. */
#define HAVE_SINF 1

/* Define to 1 if you have the <smmintrin.h> header file. */
/* #undef HAVE_SMMINTRIN_H */

/* Define to 1 if you have the `sqrtf' function. */
#define HAVE_SQRTF 1

/* Support SSE (Streaming SIMD Extensions) instructions */
/* #undef HAVE_SSE */

/* Support SSE2 (Streaming SIMD Extensions 2) instructions */
/* #undef HAVE_SSE2 */

/* Support SSE3 (Streaming SIMD Extensions 3) instructions */
/* #undef HAVE_SSE3 */

/* Support SSE4.1 (Streaming SIMD Extensions 4.1) instructions */
/* #undef HAVE_SSE41 */

/* Support SSE4.2 (Streaming SIMD Extensions 4.2) instructions */
/* #undef HAVE_SSE42 */

/* Support SSSE3 (Supplemental Streaming SIMD Extensions 3) instructions */
/* #undef HAVE_SSSE3 */

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdio.h> header file. */
#define HAVE_STDIO_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/resource.h> header file. */
#define HAVE_SYS_RESOURCE_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <tmmintrin.h> header file. */
/* #undef HAVE_TMMINTRIN_H */

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to 1 if you have the <xmmintrin.h> header file. */
/* #undef HAVE_XMMINTRIN_H */

/* Force internal FFT even if libfftw is available */
#define LIQUID_FFTOVERRIDE 1

/* Force overriding of SIMD (use portable C code) */
#define LIQUID_SIMDOVERRIDE 1

/* Enable strict program exit on error */
/* #undef LIQUID_STRICT_EXIT */

/* Suppress printing errors to stderr */
/* #undef LIQUID_SUPPRESS_ERROR_OUTPUT */

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "joseph@liquidsdr.org"

/* Define to the full name of this package. */
#define PACKAGE_NAME "liquid-dsp"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "liquid-dsp 1.5.0"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "liquid-dsp"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "1.5.0"

/* The size of `int', as computed by sizeof. */
#define SIZEOF_INT 4

/* The size of `long int', as computed by sizeof. */
#define SIZEOF_LONG_INT 8

/* The size of `long long int', as computed by sizeof. */
#define SIZEOF_LONG_LONG_INT 8

/* The size of `short int', as computed by sizeof. */
#define SIZEOF_SHORT_INT 2

/* Define to 1 if all of the C90 standard headers exist (not just the ones
   required in a freestanding environment). This macro is provided for
   backward compatibility; new code need not use it. */
#define STDC_HEADERS 1

/* Define for Solaris 2.5.1 so the uint32_t typedef from <sys/synch.h>,
   <pthread.h>, or <semaphore.h> is not used. If the typedef were allowed, the
   #define below would cause a syntax error. */
/* #undef _UINT32_T */

/* Define for Solaris 2.5.1 so the uint8_t typedef from <sys/synch.h>,
   <pthread.h>, or <semaphore.h> is not used. If the typedef were allowed, the
   #define below would cause a syntax error. */
/* #undef _UINT8_T */

/* Define to `__inline__' or `__inline' if that's what the C compiler
   calls it, or to nothing if 'inline' is not supported under any name.  */
#ifndef __cplusplus
/* #undef inline */
#endif

/* Define to rpl_malloc if the replacement function should be used. */
/* #undef malloc */

/* Define to rpl_realloc if the replacement function should be used. */
/* #undef realloc */

/* Define to `unsigned int' if <sys/types.h> does not define. */
/* #undef size_t */

/* Define to the type of an unsigned integer type of width exactly 32 bits if
   such a type exists and the standard includes do not define it. */
/* #undef uint32_t */

/* Define to the type of an unsigned integer type of width exactly 8 bits if
   such a type exists and the standard includes do not define it. */
/* #undef uint8_t */


#endif // __LIQUID_CONFIG_H__

