#cmakedefine HAVE_GETOPT_LONG 1
#cmakedefine HAVE_GETOPT_H
#cmakedefine HAVE_MEMORY_H
#cmakedefine HAVE_STDIO_H
#cmakedefine HAVE_STDLIB_H
#cmakedefine HAVE_LIBC
#cmakedefine SIZEOF_INT @SIZEOF_INT@
#cmakedefine01 fftw3f_FOUND

// SIMD extensions
#cmakedefine01 BUILD_NEON
#cmakedefine01 BUILD_AVX512
#cmakedefine01 BUILD_AVX2
#cmakedefine01 BUILD_AVX
#cmakedefine01 BUILD_SSE4
#cmakedefine01 BUILD_ALTIVEC
