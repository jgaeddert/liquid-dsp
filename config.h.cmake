#ifndef __LIQUID_CONFIG_H__
#define __LIQUID_CONFIG_H__

/* Support AVX (Advanced Vector Extensions) instructions */
#cmakedefine HAVE_AVX @HAVE_AVX@

/* Define to 1 if you have the <complex.h> header file. */
#cmakedefine HAVE_COMPLEX_H @HAVE_COMPLEX_H@

/* Define to 1 if you have the <emmintrin.h> header file. */
#cmakedefine HAVE_EMMINTRIN_H @HAVE_EMMINTRIN_H@

/* Define to 1 if you have the <fec.h> header file. */
#cmakedefine HAVE_FEC_H @HAVE_FEC_H@

/* Define to 1 if you have the <fftw3.h> header file. */
#cmakedefine HAVE_FFTW3_H @HAVE_FFTW3_H@

/* Define to 1 if you have the <float.h> header file. */
#cmakedefine HAVE_FLOAT_H @HAVE_FLOAT_H@

/* Define to 1 if you have the <getopt.h> header file. */
#cmakedefine HAVE_GETOPT_H @HAVE_GETOPT_H@

/* Define to 1 if you have the <immintrin.h> header file. */
#cmakedefine HAVE_IMMINTRIN_H @HAVE_IMMINTRIN_H@

/* Define to 1 if you have the <inttypes.h> header file. */
#cmakedefine HAVE_INTTYPES_H @HAVE_INTTYPES_H@

/* Define to 1 if you have the `fec' library (-lfec). */
#cmakedefine HAVE_LIBFEC @HAVE_LIBFEC@

/* Define to 1 if you have the <memory.h> header file. */
#cmakedefine HAVE_MEMORY_H @HAVE_MEMORY_H@

/* Define to 1 if you have the <mmintrin.h> header file. */
#cmakedefine HAVE_MMINTRIN_H @HAVE_MMINTRIN_H@

/* Support MMX instructions */
#cmakedefine HAVE_MMX @HAVE_MMX@

/* Define to 1 if you have the <pmmintrin.h> header file. */
#cmakedefine HAVE_PMMINTRIN_H @HAVE_PMMINTRIN_H@

/* Define to 1 if you have the <smmintrin.h> header file. */
#cmakedefine HAVE_SMMINTRIN_H @HAVE_SMMINTRIN_H@

/* Support SSE (Streaming SIMD Extensions) instructions */
#cmakedefine HAVE_SSE @HAVE_SSE@

/* Support SSE2 (Streaming SIMD Extensions 2) instructions */
#cmakedefine HAVE_SSE2 @HAVE_SSE2@

/* Support SSE3 (Streaming SIMD Extensions 3) instructions */
#cmakedefine HAVE_SSE3 @HAVE_SSE3@

/* Support SSE4.1 (Streaming SIMD Extensions 4.1) instructions */
#cmakedefine HAVE_SSE41 @HAVE_SSE41@

/* Support SSE4.2 (Streaming SIMD Extensions 4.2) instructions */
#cmakedefine HAVE_SSE42 @HAVE_SSE42@

/* Support SSSE3 (Supplemental Streaming SIMD Extensions 3) instructions */
#cmakedefine HAVE_SSSE3 @HAVE_SSSE3@

/* Define to 1 if you have the <stdint.h> header file. */
#cmakedefine HAVE_STDINT_H @HAVE_STDINT_H@

/* Define to 1 if you have the <sys/resource.h> header file. */
#cmakedefine HAVE_SYS_RESOURCE_H @HAVE_SYS_RESOURCE_H@

/* Define to 1 if you have the <tmmintrin.h> header file. */
#cmakedefine HAVE_TMMINTRIN_H @HAVE_TMMINTRIN_H@

/* Define to 1 if you have the <unistd.h> header file. */
#cmakedefine HAVE_UNISTD_H @HAVE_UNISTD_H@

/* Define to 1 if you have the <xmmintrin.h> header file. */
#cmakedefine HAVE_XMMINTRIN_H @HAVE_XMMINTRIN_H@

/* Force internal FFT even if libfftw is available */
#cmakedefine LIQUID_FFTOVERRIDE @LIQUID_FFTOVERRIDE@

/* Force overriding of SIMD (use portable C code) */
#cmakedefine LIQUID_SIMDOVERRIDE @LIQUID_SIMDOVERRIDE@

/* The size of `int', as computed by sizeof. */
#cmakedefine SIZEOF_INT @SIZEOF_INT@

/* The size of `unsigned int', as computed by sizeof. */
#cmakedefine SIZEOF_UNSIGNED_INT @SIZEOF_UNSIGNED_INT@

#endif // __LIQUID_CONFIG_H__
