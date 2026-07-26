// Compile-time library information

#ifndef __CONFIG_H__
#define __CONFIG_H__

// library metadata
#define PROJECT_COPYRIGHT   "Copyright (c) 2007 - 2026 Joseph D. Gaeddert <joseph@liquidsdr.org>"
#define PROJECT_LICENSE     "MIT"
#define PROJECT_AUTHOR      "Joseph D. Gaeddert <joseph@liquidsdr.org>"
#define PROJECT_HOMEPAGE    "https://liquidsdr.org"
#define PROJECT_DESCRIPTION "Software-defined radio digital signal processing library"

// version information
// const char   version[]      = "";
// unsigned int version_major  = 0;
// unsigned int version_minor  = 0;
// unsigned int version_patch  = 0;
// unsigned int version_tweak  = 0;
#define BUILD_GITHASH       "dev"

// date/time of build
#define BUILD_DATETIME      "2026-01-01T00:00:00Z"
#define BUILD_HOSTNAME      "null"
#define BUILD_OS            "null"
#define BUILD_ARCH          "null"
#define BUILD_TOOLCHAIN     "null"
#define BUILD_TYPE          "Release"
#define BUILD_ENV           "null"

// target platform
#define TARGET_OS           "null"
#define TARGET_ARCH         "null"

// other compile-time options
#define LOGGING_ENABLED     1
#define LOGGING_LEVEL       0
#define COLOR_ENABLED       1

// SIMD extensions
#define BUILD_ALTIVEC       0
#define BUILD_NEON          0
#define BUILD_MMX           0
#define BUILD_SSE           0
#define BUILD_SSE2          0
#define BUILD_SSE3          0
#define BUILD_SSSE3         0
#define BUILD_SSE41         0
#define BUILD_SSE42         0
#define BUILD_AVX           0
#define BUILD_FMA3          0
#define BUILD_AVX2          0
#define BUILD_AVX512        0
#define BUILD_AMX           0
#define BUILD_AMX101        0
#define BUILD_AMX102        0

// other information
/* #undef HAVE_GETOPT_LONG */
/* #undef HAVE_GETOPT_H */
/* #undef HAVE_MEMORY_H */
/* #undef HAVE_STDIO_H */
/* #undef HAVE_STDLIB_H */
/* #undef HAVE_LIBC */
//#define SIZEOF_INT 4
//#define fftw3f_FOUND 1

#endif // __CONFIG_H__

