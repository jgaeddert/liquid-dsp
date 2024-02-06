#
# Check for some headers, symbols, and type sizes. Most of this is used to generate the `config.h` file,
# but it's also used by CMake to allow for more better platform-agnostic compilation
#

include(CheckIncludeFile)
include(CheckIncludeFiles)
include(CheckTypeSize)
include(CheckCCompilerFlag)
include(CheckSymbolExists)

# Check for stdc headers (used for config.h)
check_include_files("stdlib.h;stddef.h" STDC_HEADERS)
if(NOT STDC_HEADERS)
  message(FATAL_ERROR "Couldn't find stdc headers (stdlib.h and stddef.h)")
endif()

# Check for more required headers
check_include_file("stdio.h" HAVE_STDIO_H)
check_include_file("stdlib.h" HAVE_STDLIB_H)
check_include_file("string.h" HAVE_STRING_H)
check_include_file("stdint.h" HAVE_STDINT_H)
check_include_file("inttypes.h" HAVE_INTTYPES_H)
check_include_file("complex.h" HAVE_COMPLEX_H)
check_include_file("float.h" HAVE_FLOAT_H)
check_include_file("limits.h" HAVE_LIMITS_H)
if(NOT HAVE_STDIO_H)
  message(FATAL_ERROR "Failed to find stdio.h")
endif()
if(NOT HAVE_STDLIB_H)
  message(FATAL_ERROR "Failed to find stdlib.h")
endif()
if(NOT HAVE_STRING_H)
  message(FATAL_ERROR "Failed to find string.h")
endif()
if(NOT HAVE_STDINT_H)
  message(FATAL_ERROR "Failed to find stdint.h")
endif()
if(NOT HAVE_INTTYPES_H)
  message(FATAL_ERROR "Failed to find inttypes.h")
endif()
if(NOT HAVE_COMPLEX_H)
  message(FATAL_ERROR "Failed to find complex.h")
endif()
if(NOT HAVE_FLOAT_H)
  message(FATAL_ERROR "Failed to find float.h")
endif()
if(NOT HAVE_LIMITS_H)
  message(FATAL_ERROR "Failed to find limits.h")
endif()

# Check for libc and libm
check_c_compiler_flag("-lc" HAVE_LIBC)
if(NOT HAVE_LIBC)
  message(FATAL_ERROR "Failed to find libc")
endif()
check_c_compiler_flag("-lm" HAVE_LIBM)
if(NOT HAVE_LIBM)
  message(FATAL_ERROR "Failed to find libm")
endif()

# Check for some optional headers (NOTE: These 3 header files shouldn't be required since they're only used for compiling the benchmarks and autotests)
check_include_file("sys/resource.h" HAVE_SYS_RESOURCE_H) # Required for benchmarks
check_include_file("unistd.h" HAVE_UNISTD_H) # Not required for any src files, but used for scripts, sandbox, and example binaries
check_include_file("getopt.h" HAVE_GETOPT_H) # Required for benchmarks, example binaries, sandbox, and tests

# Check type sizes
check_type_size("size_t" SIZEOF_SIZE_T)
check_type_size("uint32_t" SIZEOF_UINT32_T)
check_type_size("uint8_t" SIZEOF_UINT8_T)
check_type_size("short int" SIZEOF_SHORT_INT)
check_type_size("int" SIZEOF_INT)
check_type_size("long int" SIZEOF_LONG_INT)
check_type_size("long long int" SIZEOF_LONG_LONG_INT)

# Check for some required symbols
check_symbol_exists(malloc "stdlib.h" HAVE_MALLOC)
check_symbol_exists(realloc "stdlib.h" HAVE_REALLOC)
check_symbol_exists(free "stdlib.h" HAVE_FREE)
check_symbol_exists(memset "string.h" HAVE_MEMSET)
check_symbol_exists(memmove "string.h" HAVE_MEMMOVE)
check_symbol_exists(sinf "math.h" HAVE_SINF)
check_symbol_exists(cosf "math.h" HAVE_COSF)
check_symbol_exists(expf "math.h" HAVE_EXPF)
check_symbol_exists(sqrtf "math.h" HAVE_SQRTF)
check_symbol_exists(cargf "complex.h" HAVE_CARGF)
check_symbol_exists(cexpf "complex.h" HAVE_CEXPF)
check_symbol_exists(crealf "complex.h" HAVE_CREALF)
check_symbol_exists(cimagf "complex.h" HAVE_CIMAGF)
foreach(have_symbol
HAVE_MALLOC HAVE_REALLOC HAVE_FREE
HAVE_MEMSET HAVE_MEMMOVE
HAVE_SINF HAVE_COSF HAVE_EXPF HAVE_SQRTF
HAVE_CARGF HAVE_CEXPF HAVE_CREALF HAVE_CIMAGF)
  if(NOT ${have_symbol})
    message(FATAL_ERROR "${have_symbol} check failed (the symbol wasn't found).")
  endif()
endforeach()

# Enable SIMD optimizations if the override flag isn't enabled
if(NOT LIQUID_SIMDOVERRIDE)
  # Enable `-march=native`, telling our compiler to optimize our code for the host device
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native")
  include(CheckCSourceRuns)

  # Check for SIMD header files (some of these are only needed for the `config.h` file)
  check_include_file("mmintrin.h" HAVE_MMINTRIN_H) # MMX
  check_include_file("xmmintrin.h" HAVE_XMMINTRIN_H) # SSE
  check_include_file("emmintrin.h" HAVE_EMMINTRIN_H) # SSE2
  check_include_file("pmmintrin.h" HAVE_PMMINTRIN_H) # SSE3
  check_include_file("tmmintrin.h" HAVE_TMMINTRIN_H) # SSSE3
  check_include_file("smmintrin.h" HAVE_SMMINTRIN_H) # SSE4.1
  check_include_file("nmmintrin.h" HAVE_NMMINTRIN_H) # SSE4.2
  check_include_file("immintrin.h" HAVE_IMMINTRIN_H) # AVX/AVX2/AVX512
  check_include_file("arm_neon.h" HAVE_ARM_NEON_H) # NEON

  # Check for AVX512 support
  check_c_source_runs("
      #ifndef __AVX512F__
      #error AVX512 not supported
      #endif
      int main() { return 0; }
      " HAVE_AVX512
  )

  # Check for AVX support
  check_c_source_runs("
      #ifndef __AVX__
      #error AVX not supported
      #endif
      int main() { return 0; }
      " HAVE_AVX
  )

  # Check for SSE4.2 support
  check_c_source_runs("
      #ifndef __SSE4_2__
      #error SSE42 not supported
      #endif
      int main() { return 0; }
      " HAVE_SSE42
  )

  # Check for SSE4.1 support
  check_c_source_runs("
      #ifndef __SSE4_1__
      #error SSE41 not supported
      #endif
      int main() { return 0; }
      " HAVE_SSE41
  )

  # Check for SSE3 support (not to be confused with SSSE3)
  check_c_source_runs("
      #ifndef __SSE3__
      #error SSE3 not supported
      #endif
      int main() { return 0; }
      " HAVE_SSE3
  )

  # Check for SSSE3 support (not to be confused with SSE3)
  check_c_source_runs("
      #ifndef __SSSE3__
      #error SSSE3 not supported
      #endif
      int main() { return 0; }
      " HAVE_SSSE3
  )

  # Check for SSE2 support
  check_c_source_runs("
      #ifndef __SSE2__
      #error SSE2 not supported
      #endif
      int main() { return 0; }
      " HAVE_SSE2
  )

  # Check for SSE support
  check_c_source_runs("
      #ifndef __SSE__
      #error SSE not supported
      #endif
      int main() { return 0; }
      " HAVE_SSE
  )

  # Check for MMX support
  check_c_source_runs("
      #ifndef __MMX__
      #error MMX not supported
      #endif
      int main() { return 0; }
      " HAVE_MMX
  )

  # Check for NEON support (ARM)
  check_c_source_runs("
      #ifndef __ARM_NEON
      #error NEON not supported
      #endif
      int main() { return 0; }
      " HAVE_NEON
  )

  # Check for AltiVec support (PowerPC)
  check_c_source_runs("
      #ifndef __ALTIVEC__
      #error ALTIVEC not supported
      #endif
      int main() { return 0; }
      " HAVE_ALTIVEC
  )
endif()

# Check for libfec
find_library(HAVE_LIBFEC fec)
if(HAVE_LIBFEC)
  message(STATUS "Found libfec")
endif()

# Check for fftw3.h and libfftw3f
check_include_file("fftw3.h" HAVE_FFTW3_H)
find_library(HAVE_LIBFFTW3F fftw3f)
if(HAVE_LIBFFTW3F)
  message(STATUS "Found libfftw3f")
endif()

# Generate the `config.h` file
configure_file(config.h.cmake "${CMAKE_CURRENT_SOURCE_DIR}/config.h" @ONLY)
