/* config.h.in.  Generated from configure.ac by autoheader.  */


#ifndef __LIQUID_CONFIG_H__
#define __LIQUID_CONFIG_H__


/* Define to 1 to support Advanced Vector Extensions */
#cmakedefine HAVE_AVX 1

/* Define to 1 if you have the `cargf' function. */
#cmakedefine HAVE_CARGF 1

/* Define to 1 if you have the `cexpf' function. */
#cmakedefine HAVE_CEXPF 1

/* Define to 1 if you have the `cimagf' function. */
#cmakedefine HAVE_CIMAGF 1

/* Define to 1 if you have the <complex.h> header file. */
#cmakedefine HAVE_COMPLEX_H 1

/* Define to 1 if you have the `cosf' function. */
#cmakedefine HAVE_COSF 1

/* Define to 1 if you have the `crealf' function. */
#cmakedefine HAVE_CREALF 1

/* Define to 1 if you have the `expf' function. */
#cmakedefine HAVE_EXPF 1

/* Define to 1 if you have the <fec.h> header file. */
#cmakedefine HAVE_FEC_H 1

/* Define to 1 if you have the <fftw3.h> header file. */
#cmakedefine HAVE_FFTW3_H 1

/* Define to 1 if you have the <float.h> header file. */
#cmakedefine HAVE_FLOAT_H 1

/* Define to 1 if you have the `free' function. */
#cmakedefine HAVE_FREE 1

/* Define to 1 if you have the <getopt.h> header file. */
#cmakedefine HAVE_GETOPT_H 1

/* Define to 1 if you have the <inttypes.h> header file. */
#cmakedefine HAVE_INTTYPES_H 1

/* Define to 1 if you have the `c' library (-lc). */
#cmakedefine HAVE_LIBC 1

/* Define to 1 if you have the `fec' library (-lfec). */
#cmakedefine HAVE_LIBFEC 1

/* Define to 1 if you have the `fftw3f' library (-lfftw3f). */
#cmakedefine HAVE_LIBFFTW3F 1

/* Define to 1 if you have the `m' library (-lm). */
#cmakedefine HAVE_LIBM 1

/* Define to 1 if you have the <limits.h> header file. */
#cmakedefine HAVE_LIMITS_H 1

/* Define to 1 if you have the `malloc' function. */
#cmakedefine HAVE_MALLOC 1

/* Define to 1 if you have the `memmove' function. */
#cmakedefine HAVE_MEMMOVE 1

/* Define to 1 if you have the `memset' function. */
#cmakedefine HAVE_MEMSET 1

/* Define to 1 to support Multimedia Extensions */
#cmakedefine HAVE_MMX 1

/* Define to 1 if you have the `realloc' function. */
#cmakedefine HAVE_REALLOC 1

/* Define to 1 if you have the `sinf' function. */
#cmakedefine HAVE_SINF 1

/* Define to 1 if you have the `sqrtf' function. */
#cmakedefine HAVE_SQRTF 1

/* Define to 1 to support Streaming SIMD Extensions */
#cmakedefine HAVE_SSE 1

/* Define to 1 to support Streaming SIMD Extensions */
#cmakedefine HAVE_SSE2 1

/* Define to 1 to support Streaming SIMD Extensions 3 */
#cmakedefine HAVE_SSE3 1

/* Define to 1 to support Streaming SIMD Extensions 4.1 */
#cmakedefine HAVE_SSE41 1

/* Define to 1 to support Streaming SIMD Extensions 4.2 */
#cmakedefine HAVE_SSE42 1

/* Define to 1 to support Supplemental Streaming SIMD Extensions 3 */
#cmakedefine HAVE_SSSE3 1

/* Define to 1 if you have the <stdint.h> header file. */
#cmakedefine HAVE_STDINT_H 1

/* Define to 1 if you have the <stdio.h> header file. */
#cmakedefine HAVE_STDIO_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#cmakedefine HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#cmakedefine HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#cmakedefine HAVE_STRING_H 1

/* Define to 1 if you have the <sys/resource.h> header file. */
#cmakedefine HAVE_SYS_RESOURCE_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#cmakedefine HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#cmakedefine HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#cmakedefine HAVE_UNISTD_H 1

/* Force internal FFT even if libfftw is available */
#cmakedefine LIQUID_FFTOVERRIDE 1

/* Force overriding of SIMD (use portable C code) */
#cmakedefine LIQUID_SIMDOVERRIDE 1

/* Enable strict program exit on error */
#cmakedefine LIQUID_STRICT_EXIT 1

/* Suppress printing errors to stderr */
#cmakedefine LIQUID_SUPPRESS_ERROR_OUTPUT 1

/* Define to the address where bug reports for this package should be sent. */
#cmakedefine PACKAGE_BUGREPORT "@PACKAGE_BUGREPORT@"

/* Define to the full name of this package. */
#cmakedefine PACKAGE_NAME "@PACKAGE_NAME@"

/* Define to the full name and version of this package. */
#cmakedefine PACKAGE_STRING "@PACKAGE_STRING@"

/* Define to the one symbol short name of this package. */
#cmakedefine PACKAGE_TARNAME "@PACKAGE_TARNAME@"

/* Define to the home page for this package. */
#cmakedefine PACKAGE_URL "@PACKAGE_URL@"

/* Define to the version of this package. */
#cmakedefine PACKAGE_VERSION "@PACKAGE_VERSION@"

/* The size of `int', as computed by sizeof. */
#cmakedefine SIZEOF_INT @SIZEOF_INT@

/* The size of `long int', as computed by sizeof. */
#cmakedefine SIZEOF_LONG_INT @SIZEOF_LONG_INT@

/* The size of `long long int', as computed by sizeof. */
#cmakedefine SIZEOF_LONG_LONG_INT @SIZEOF_LONG_LONG_INT@

/* The size of `short int', as computed by sizeof. */
#cmakedefine SIZEOF_SHORT_INT @SIZEOF_SHORT_INT@

/* Define to 1 if all of the C90 standard headers exist (not just the ones
   required in a freestanding environment). This macro is provided for
   backward compatibility; new code need not use it. */
#cmakedefine STDC_HEADERS 1

/* Define for Solaris 2.5.1 so the uint32_t typedef from <sys/synch.h>,
   <pthread.h>, or <semaphore.h> is not used. If the typedef were allowed, the
   #define below would cause a syntax error. */
#cmakedefine _UINT32_T @_UINT32_T@

/* Define for Solaris 2.5.1 so the uint8_t typedef from <sys/synch.h>,
   <pthread.h>, or <semaphore.h> is not used. If the typedef were allowed, the
   #define below would cause a syntax error. */
#cmakedefine _UINT8_T @_UINT8_T@

/* Define to `__inline__' or `__inline' if that's what the C compiler
   calls it, or to nothing if 'inline' is not supported under any name.  */
#ifndef __cplusplus
#cmakedefine inline @INLINE@
#endif

/* Define to rpl_malloc if the replacement function should be used. */
#cmakedefine malloc @MALLOC@

/* Define to rpl_realloc if the replacement function should be used. */
#cmakedefine realloc @REALLOC@

/* Define to `unsigned int' if <sys/types.h> does not define. */
#cmakedefine size_t @SIZE_T@

/* Define to the type of an unsigned integer type of width exactly 32 bits if
   such a type exists and the standard includes do not define it. */
#cmakedefine uint32_t @UINT32_T@

/* Define to the type of an unsigned integer type of width exactly 8 bits if
   such a type exists and the standard includes do not define it. */
#cmakedefine uint8_t @UINT8_T@


#endif // __LIQUID_CONFIG_H__

