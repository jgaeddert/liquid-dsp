cmake_minimum_required(VERSION 3.15)
project(liquid-dsp VERSION 1.5 LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# CXX language support optional
include(CheckLanguage)
check_language(CXX)
if(CMAKE_CXX_COMPILER)
  enable_language(CXX)
  set(CMAKE_CXX_STANDARD 14)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
else()
  message(STATUS
    "The CXX compiler identification is unknown. "
    "No CMAKE_Fortran_COMPILER could be found."
  )
endif()

if(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
  message(FATAL_ERROR [=[
The Microsoft Visual Studio C Compiler is not fully compatible with the C99
standard. It does not support the complex/_Complex keywords or
variable-length arrays, both of which are optional C11 features. These are
currently required for compilation. For reference:
https://learn.microsoft.com/en-us/cpp/overview/visual-cpp-language-conformance
https://learn.microsoft.com/en-us/cpp/preprocessor/predefined-macros
]=]
  )
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(GNUInstallDirs)

if(CMAKE_SYSTEM_PROCESSOR MATCHES "^arm")
  set(PROCESSOR_IS_ARM TRUE)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "(x86_64)|(AMD64|amd64)|(^i.86$)")
  set(PROCESSOR_IS_X86 TRUE)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(powerpc|ppc)")
  set(PROCESSOR_IS_POWER TRUE)
endif()

# Configure options
option(LIQUID_DEBUG_MESSAGES "Enable verbose debug messages (-DDEBUG)" OFF)
option(LIQUID_FFTOVERRIDE "Force internal FFT even if libfftw is available" OFF)
option(LIQUID_SIMDOVERRIDE "Force overriding of SIMD (use portable C code)" OFF)
option(LIQUID_COVERAGE "Enable flags to test code coverage" OFF)
option(LIQUID_SUPPRESS_ERROR_OUTPUT "Suppress printing errors to stderr" OFF)
option(LIQUID_STRICT_EXIT "Enable strict program exit on error" OFF)

# Configure init
set(PACKAGE_BUGREPORT "joseph@liquidsdr.org" CACHE INTERNAL
  "address where bug reports for this package should be sent."
)
set(PACKAGE_NAME "${PROJECT_NAME}" CACHE INTERNAL "full name of this package")
set(PACKAGE_STRING "${PROJECT_NAME} ${PROJECT_VERSION}" CACHE INTERNAL
  "full name and version of this package"
)
set(PACKAGE_TARNAME "${PROJECT_NAME}" CACHE INTERNAL
  "one symbol short name of this package"
)
set(PACKAGE_URL "" CACHE INTERNAL "home page for this package")
set(PACKAGE_VERSION "${PROJECT_VERSION}" CACHE INTERNAL
  "version of this package"
)

# code coverage
if(LIQUID_COVERAGE)
  find_program(HAVE_GCOVR gcovr)
  if(NOT HAVE_GCOVR)
    message(FATAL_ERROR "Need gcovr with coverage option")
  endif()
endif()

include(CheckIncludeFile)

# check default includes
check_include_file("stdio.h" HAVE_STDIO_H)
check_include_file("stdlib.h" HAVE_STDLIB_H)
check_include_file("string.h" HAVE_STRING_H)
check_include_file("inttypes.h" HAVE_INTTYPES_H)
check_include_file("stdint.h" HAVE_STDINT_H)

foreach(have_include
    HAVE_STDIO_H HAVE_STDLIB_H HAVE_STRING_H HAVE_INTTYPES_H HAVE_STDINT_H)
  if(NOT ${have_include})
    message(FATAL_ERROR "${have_include} required.")
  endif()
endforeach()

# check default POSIX includes
check_include_file("unistd.h" HAVE_UNISTD_H)

if(NOT HAVE_UNISTD_H)
  message(FATAL_ERROR "HAVE_UNISTD_H required.")
endif()

# these are checked because they are in the default scripts/config.h file,
# however these headers and/or #defines do not appear to be #included or used
# directly by the liquid source files
check_include_file("sys/types.h" HAVE_SYS_TYPES_H)
check_include_file("sys/stat.h" HAVE_SYS_STAT_H)
check_include_file("strings.h" HAVE_STRINGS_H)

include(CheckIncludeFiles)
check_include_files("stdlib.h;stddef.h" STDC_HEADERS)

foreach(have_include
    HAVE_SYS_TYPES_H HAVE_SYS_STAT_H STDC_HEADERS HAVE_STRINGS_H)
  if(NOT ${have_include})
    message(FATAL_ERROR "${have_include} required.")
  endif()
endforeach()

include(CheckSymbolExists)

# check for necessary libraries, library functions
check_symbol_exists(malloc "stdlib.h" HAVE_MALLOC)
check_symbol_exists(realloc "stdlib.h" HAVE_REALLOC)
check_symbol_exists(free "stdlib.h" HAVE_FREE)
check_symbol_exists(memset "string.h" HAVE_MEMSET)
check_symbol_exists(memmove "string.h" HAVE_MEMMOVE)

foreach(have_symbol HAVE_MALLOC HAVE_REALLOC HAVE_FREE HAVE_MEMSET HAVE_MEMMOVE)
  if(NOT ${have_symbol})
    message(FATAL_ERROR "${have_symbol} required.")
  endif()
endforeach()

# check for linking against required system libraries
include(CMakeCheckCompilerFlagCommonPatterns)
check_compiler_flag_common_patterns(_common_patterns)

include(CheckCSourceCompiles)
set(_c_main "int main() { return 0; }")

include(CMakePushCheckState)
cmake_push_check_state(RESET)

set(CMAKE_REQUIRED_LINK_OPTIONS -lc)
check_c_source_compiles("${_c_main}" HAVE_LIBC ${_common_patterns})

cmake_reset_check_state()

set(CMAKE_REQUIRED_LINK_OPTIONS -lm)
check_c_source_compiles("${_c_main}" HAVE_LIBM ${_common_patterns})

foreach(have_library HAVE_LIBC HAVE_LIBM)
  if(NOT ${have_library})
    message(FATAL_ERROR "${have_library} required.")
  endif()
endforeach()

# check for required math functions
check_symbol_exists(sinf "math.h" HAVE_SINF)
check_symbol_exists(cosf "math.h" HAVE_COSF)
check_symbol_exists(expf "math.h" HAVE_EXPF)
check_symbol_exists(cargf "complex.h" HAVE_CARGF)
check_symbol_exists(cexpf "complex.h" HAVE_CEXPF)
check_symbol_exists(crealf "complex.h" HAVE_CREALF)
check_symbol_exists(cimagf "complex.h" HAVE_CIMAGF)
check_symbol_exists(sqrtf "math.h" HAVE_SQRTF)

foreach(have_symbol HAVE_SINF HAVE_COSF HAVE_EXPF HAVE_CARGF HAVE_CEXPF
    HAVE_CREALF HAVE_CIMAGF HAVE_SQRTF)
  if(NOT ${have_symbol})
    message(FATAL_ERROR "${have_symbol} required.")
  endif()
endforeach()

cmake_pop_check_state()

# check for other necessary header files
check_include_file("complex.h" HAVE_COMPLEX_H)
check_include_file("float.h" HAVE_FLOAT_H)
check_include_file("limits.h" HAVE_LIMITS_H)

foreach(have_include HAVE_COMPLEX_H HAVE_FLOAT_H HAVE_LIMITS_H)
  if(NOT ${have_include})
    message(FATAL_ERROR "${have_include} required.")
  endif()
endforeach()

# check for other necessary POSIX header files
check_include_file("getopt.h" HAVE_GETOPT_H)
check_include_file("sys/resource.h" HAVE_SYS_RESOURCE_H)

foreach(have_include HAVE_GETOPT_H HAVE_SYS_RESOURCE_H)
  if(NOT ${have_include})
    message(FATAL_ERROR "${have_include} required.")
  endif()
endforeach()

# Check for optional header files, libraries, programs

find_package(fec)

if(fec_FOUND)
  set(HAVE_FEC_H TRUE)
  set(HAVE_LIBFEC TRUE)
endif()

find_package(FFTW)

if(FFTW_FOUND)
  set(HAVE_FFTW3_H TRUE)
  set(HAVE_LIBFFTW3F TRUE)
endif()

# Checks for typedefs, structures, and compiler characteristics.
include(CheckInlineSupport)
check_inline_support(HAVE_INLINE)

include(CheckTypeSize)
check_type_size("size_t" SIZEOF_SIZE_T)
check_type_size("uint32_t" SIZEOF_UINT32_T)
check_type_size("uint8_t" SIZEOF_UINT8_T)

check_type_size("short int" SIZEOF_SHORT_INT)
check_type_size("int" SIZEOF_INT)
check_type_size("long int" SIZEOF_LONG_INT)
check_type_size("long long int" SIZEOF_LONG_LONG_INT)

if(NOT CpuFeatures_DIR)
  # download/build/install cpu_features dependency at configure time.
  message(STATUS "Preparing to install cpu_features dependency")
  include(InstallCpuFeatures)
endif()

include(InstructionSet) # _CPUID_AVAILABLE_EXTENSIONS, check_cpuid

# check for MMX/SSE/AVX CPU extensions and intrinsics headers
check_cpuid("mmx" HAVE_MMX)
check_include_file(mmintrin.h HAVE_MMINTRIN_H)

check_cpuid("sse" HAVE_SSE)
check_include_file(xmmintrin.h HAVE_XMMINTRIN_H)

check_cpuid("sse2" HAVE_SSE2)
check_include_file(emmintrin.h HAVE_EMMINTRIN_H)

check_cpuid("sse3" HAVE_SSE3)
check_include_file(pmmintrin.h HAVE_PMMINTRIN_H)

check_cpuid("ssse3" HAVE_SSSE3)
check_include_file(tmmintrin.h HAVE_TMMINTRIN_H)

check_cpuid("sse4_1" HAVE_SSE41)
check_cpuid("sse4_2" HAVE_SSE42)
check_include_file(smmintrin.h HAVE_SMMINTRIN_H)

check_cpuid("avx" HAVE_AVX)
check_cpuid("avx2" HAVE_AVX2)
check_include_file(immintrin.h HAVE_IMMINTRIN_H)

check_cpuid("altivec" HAVE_ALTIVEC)
check_cpuid("neon" HAVE_NEON)

# get architecture option
add_library(arch_option INTERFACE)

if(PROCESSOR_IS_X86)
  if (HAVE_AVX2)
    # AVX2 extensions
    target_compile_options(arch_option INTERFACE
      "$<$<C_COMPILER_ID:AppleClang,Clang,GNU>:-mavx2>"
    )
  elseif (HAVE_AVX)
    # AVX extensions
    target_compile_options(arch_option INTERFACE
      "$<$<C_COMPILER_ID:AppleClang,Clang,GNU>:-mavx>"
    )
  elseif (HAVE_SSE41 AND HAVE_SSE42)
    # SSE4.1/2 extensions
    target_compile_options(arch_option INTERFACE
      "$<$<C_COMPILER_ID:AppleClang,Clang,GNU>:-msse4.1>"
    )
  elseif (HAVE_SSE3)
    # SSE3 extensions
    target_compile_options(arch_option INTERFACE
      "$<$<C_COMPILER_ID:AppleClang,Clang,GNU>:-msse3>"
    )
  elseif (HAVE_SSE2)
    # SSE2 extensions
    target_compile_options(arch_option INTERFACE
      "$<$<C_COMPILER_ID:AppleClang,Clang,GNU>:-msse2>"
    )
  endif()
elseif(PROCESSOR_IS_POWER)
  if(HAVE_ALTIVEC)
    target_compile_options(arch_option INTERFACE -fno-common -faltivec)
  endif()
elseif(PROCESSOR_IS_ARM)
  target_compile_options(arch_option INTERFACE -ffast-math)
  if(HAVE_NEON AND NOT APPLE)
    # TODO: check these flags
    target_compile_options(arch_option INTERFACE
      -mcpu=cortex-a7 -mfloat-abi=hard -mfpu=neon-vfpv4
    )
  endif()
endif()

#
# Generate config.h file.
#
configure_file(config.h.cmake "${CMAKE_CURRENT_SOURCE_DIR}/config.h" @ONLY)

#
# liquid headers
#

##
## liquid-dsp modules
##

#
# MODULE : agc - automatic gain control
#

set(agc_SOURCES
  src/agc/src/agc_crcf.c
  src/agc/src/agc_rrrf.c
)

set(agc_AUTOTEST_SOURCES src/agc/tests/agc_crcf_autotest.c)

set(agc_BENCHMARK_SOURCES
  src/agc/bench/agc_crcf_benchmark.c
)

#
# MODULE : audio
#

set(audio_SOURCES src/audio/src/cvsd.c)

set(audio_AUTOTEST_SOURCES src/audio/tests/cvsd_autotest.c)

set(audio_BENCHMARK_SOURCES
  src/audio/bench/cvsd_benchmark.c
)

#
# MODULE : buffer
#

set(buffer_SOURCES
  src/buffer/src/bufferf.c
  src/buffer/src/buffercf.c
)

set(buffer_AUTOTEST_SOURCES
  src/buffer/tests/cbuffer_autotest.c
  src/buffer/tests/wdelay_autotest.c
  src/buffer/tests/window_autotest.c
)

set(buffer_BENCHMARK_SOURCES
  src/buffer/bench/cbuffercf_benchmark.c
  src/buffer/bench/window_push_benchmark.c
  src/buffer/bench/window_read_benchmark.c
)

#
# MODULE : channel
#

set(channel_SOURCES src/channel/src/channel_cccf.c)

set(channel_AUTOTEST_SOURCES
  src/channel/tests/channel_copy_autotest.c
  src/channel/tests/tvmpch_copy_autotest.c
)

set(channel_BENCHMARK_SOURCES)

#
# MODULE : dotprod
#

# Portable C Version
set(dotprod_C_SOURCES
  src/dotprod/src/dotprod_cccf.c
  src/dotprod/src/dotprod_crcf.c
  src/dotprod/src/dotprod_rrrf.c
  src/dotprod/src/sumsq.c
)

# AVX
set(dotprod_AVX_SOURCES
  src/dotprod/src/dotprod_cccf.avx.c
  src/dotprod/src/dotprod_crcf.avx.c
  src/dotprod/src/dotprod_rrrf.avx.c
  src/dotprod/src/sumsq.avx.c
)

# SSE4.1/2
set(dotprod_SSE4_SOURCES
  src/dotprod/src/dotprod_cccf.mmx.c
  src/dotprod/src/dotprod_crcf.mmx.c
  src/dotprod/src/dotprod_rrrf.sse4.c
  src/dotprod/src/sumsq.mmx.c
)

# SSE3/SSE2/MMX
set(dotprod_MMX_SOURCES
  src/dotprod/src/dotprod_cccf.mmx.c
  src/dotprod/src/dotprod_crcf.mmx.c
  src/dotprod/src/dotprod_rrrf.mmx.c
  src/dotprod/src/sumsq.mmx.c
)

# AltiVec
set(dotprod_ALTIVEC_SOURCES
  src/dotprod/src/dotprod_cccf.c
  src/dotprod/src/dotprod_crcf.av.c
  src/dotprod/src/dotprod_rrrf.av.c
  src/dotprod/src/sumsq.c
)

# ARM Neon
set(dotprod_NEON_SOURCES
  src/dotprod/src/dotprod_cccf.neon.c
  src/dotprod/src/dotprod_crcf.neon.c
  src/dotprod/src/dotprod_rrrf.neon.c
  src/dotprod/src/sumsq.c
)

if (LIQUID_SIMDOVERRIDE)
  set(dotprod_SOURCES ${dotprod_C_SOURCES})
else()
  # should resemble target compile option branching for arch_option
  if(PROCESSOR_IS_X86)
    if((HAVE_AVX2 OR HAVE_AVX) AND HAVE_IMMINTRIN_H)
      set(dotprod_SOURCES ${dotprod_AVX_SOURCES})
    elseif(HAVE_SSE41 AND HAVE_SMMINTRIN_H)
      set(dotprod_SOURCES ${dotprod_SSE4_SOURCES})
    elseif((HAVE_SSE3 AND HAVE_PMMINTRIN_H) OR (HAVE_SSE2 AND HAVE_EMMINTRIN_H))
      set(dotprod_SOURCES ${dotprod_MMX_SOURCES})
    else()
      set(dotprod_SOURCES ${dotprod_C_SOURCES})
    endif()
  elseif(PROCESSOR_IS_POWER)
    if(HAVE_ALTIVEC)
      set(dotprod_SOURCES ${dotprod_ALTIVEC_SOURCES})
    else()
      set(dotprod_SOURCES ${dotprod_C_SOURCES})
    endif()
  elseif(PROCESSOR_IS_ARM)
    if(HAVE_NEON)
      set(dotprod_SOURCES ${dotprod_NEON_SOURCES})
    else()
      set(dotprod_SOURCES ${dotprod_C_SOURCES})
    endif()
  else()
    set(dotprod_SOURCES ${dotprod_C_SOURCES})
  endif()
endif()

set(dotprod_AUTOTEST_SOURCES
  src/dotprod/tests/dotprod_rrrf_autotest.c
  src/dotprod/tests/dotprod_crcf_autotest.c
  src/dotprod/tests/dotprod_cccf_autotest.c
  src/dotprod/tests/sumsqf_autotest.c
  src/dotprod/tests/sumsqcf_autotest.c
)

set(dotprod_BENCHMARK_SOURCES
  src/dotprod/bench/dotprod_cccf_benchmark.c
  src/dotprod/bench/dotprod_crcf_benchmark.c
  src/dotprod/bench/dotprod_rrrf_benchmark.c
  src/dotprod/bench/sumsqf_benchmark.c
  src/dotprod/bench/sumsqcf_benchmark.c
)

#
# MODULE : equalization
#

set(equalization_SOURCES
  src/equalization/src/equalizer_cccf.c
  src/equalization/src/equalizer_rrrf.c
)

set(equalization_AUTOTEST_SOURCES
  src/equalization/tests/eqlms_cccf_autotest.c
  src/equalization/tests/eqrls_rrrf_autotest.c
)

set(equalization_BENCHMARK_SOURCES
  src/equalization/bench/eqlms_cccf_benchmark.c
  src/equalization/bench/eqrls_cccf_benchmark.c
)

#
# MODULE : fec - forward error-correction
#

set(fec_SOURCES
  src/fec/src/crc.c
  src/fec/src/fec.c
  src/fec/src/fec_conv.c
  src/fec/src/fec_conv_poly.c
  src/fec/src/fec_conv_pmatrix.c
  src/fec/src/fec_conv_punctured.c
  src/fec/src/fec_golay2412.c
  src/fec/src/fec_hamming74.c
  src/fec/src/fec_hamming84.c
  src/fec/src/fec_hamming128.c
  src/fec/src/fec_hamming1511.c
  src/fec/src/fec_hamming3126.c
  src/fec/src/fec_hamming128_gentab.c
  src/fec/src/fec_pass.c
  src/fec/src/fec_rep3.c
  src/fec/src/fec_rep5.c
  src/fec/src/fec_rs.c
  src/fec/src/fec_secded2216.c
  src/fec/src/fec_secded3932.c
  src/fec/src/fec_secded7264.c
  src/fec/src/interleaver.c
  src/fec/src/packetizer.c
  src/fec/src/sumproduct.c
)

set(fec_AUTOTEST_SOURCES
  src/fec/tests/crc_autotest.c
  src/fec/tests/fec_autotest.c
  src/fec/tests/fec_copy_autotest.c
  src/fec/tests/fec_soft_autotest.c
  src/fec/tests/fec_golay2412_autotest.c
  src/fec/tests/fec_hamming74_autotest.c
  src/fec/tests/fec_hamming84_autotest.c
  src/fec/tests/fec_hamming128_autotest.c
  src/fec/tests/fec_hamming1511_autotest.c
  src/fec/tests/fec_hamming3126_autotest.c
  src/fec/tests/fec_reedsolomon_autotest.c
  src/fec/tests/fec_rep3_autotest.c
  src/fec/tests/fec_rep5_autotest.c
  src/fec/tests/fec_secded2216_autotest.c
  src/fec/tests/fec_secded3932_autotest.c
  src/fec/tests/fec_secded7264_autotest.c
  src/fec/tests/interleaver_autotest.c
  src/fec/tests/packetizer_copy_autotest.c
  src/fec/tests/packetizer_autotest.c
)

set(fec_BENCHMARK_SOURCES
  src/fec/bench/crc_benchmark.c
  src/fec/bench/fec_encode_benchmark.c
  src/fec/bench/fec_decode_benchmark.c
  src/fec/bench/fecsoft_decode_benchmark.c
  src/fec/bench/sumproduct_benchmark.c
  src/fec/bench/interleaver_benchmark.c
  src/fec/bench/packetizer_decode_benchmark.c
)

#
# MODULE : fft - fast Fourier transforms, discrete sine/cosine transforms, etc.
#

set(fft_SOURCES
  src/fft/src/fftf.c
  src/fft/src/spgramcf.c
  src/fft/src/spgramf.c
  src/fft/src/fft_utilities.c
)

set(fft_AUTOTEST_SOURCES
  src/fft/tests/asgram_autotest.c
  src/fft/tests/fft_small_autotest.c
  src/fft/tests/fft_radix2_autotest.c
  src/fft/tests/fft_composite_autotest.c
  src/fft/tests/fft_prime_autotest.c
  src/fft/tests/fft_r2r_autotest.c
  src/fft/tests/fft_shift_autotest.c
  src/fft/tests/spgram_autotest.c
  src/fft/tests/spwaterfall_autotest.c
)

list(APPEND autotest_EXTRA_SOURCES
  src/fft/tests/fft_runtest.c
  src/fft/tests/data/fft_data_2.c
  src/fft/tests/data/fft_data_3.c
  src/fft/tests/data/fft_data_4.c
  src/fft/tests/data/fft_data_5.c
  src/fft/tests/data/fft_data_6.c
  src/fft/tests/data/fft_data_7.c
  src/fft/tests/data/fft_data_8.c
  src/fft/tests/data/fft_data_9.c
  src/fft/tests/data/fft_data_10.c
  src/fft/tests/data/fft_data_16.c
  src/fft/tests/data/fft_data_17.c
  src/fft/tests/data/fft_data_20.c
  src/fft/tests/data/fft_data_21.c
  src/fft/tests/data/fft_data_22.c
  src/fft/tests/data/fft_data_24.c
  src/fft/tests/data/fft_data_26.c
  src/fft/tests/data/fft_data_30.c
  src/fft/tests/data/fft_data_32.c
  src/fft/tests/data/fft_data_35.c
  src/fft/tests/data/fft_data_36.c
  src/fft/tests/data/fft_data_43.c
  src/fft/tests/data/fft_data_48.c
  src/fft/tests/data/fft_data_63.c
  src/fft/tests/data/fft_data_64.c
  src/fft/tests/data/fft_data_79.c
  src/fft/tests/data/fft_data_92.c
  src/fft/tests/data/fft_data_96.c
  src/fft/tests/data/fft_data_120.c
  src/fft/tests/data/fft_data_130.c
  src/fft/tests/data/fft_data_157.c
  src/fft/tests/data/fft_data_192.c
  src/fft/tests/data/fft_data_317.c
  src/fft/tests/data/fft_data_509.c
  src/fft/tests/data/fft_r2rdata_8.c
  src/fft/tests/data/fft_r2rdata_27.c
  src/fft/tests/data/fft_r2rdata_32.c
)

set(fft_AUTOTEST_BYPRODUCTS
  autotest/logs/spgram.gnu
  autotest/logs/spgramcf_signal.m
  autotest/logs/spwaterfall.bin
  autotest/logs/spwaterfall.gnu
)

set(fft_BENCHMARK_SOURCES
  src/fft/bench/fft_composite_benchmark.c
  src/fft/bench/fft_prime_benchmark.c
  src/fft/bench/fft_radix2_benchmark.c
  src/fft/bench/fft_r2r_benchmark.c
  src/fft/bench/spgramcf_benchmark.c
)

list(APPEND benchmark_EXTRA_SOURCES src/fft/bench/fft_runbench.c)

#
# MODULE : filter
#

set(filter_SOURCES
  src/filter/src/bessel.c
  src/filter/src/butter.c
  src/filter/src/cheby1.c
  src/filter/src/cheby2.c
  src/filter/src/ellip.c
  src/filter/src/filter_rrrf.c
  src/filter/src/filter_crcf.c
  src/filter/src/filter_cccf.c
  src/filter/src/firdes.c
  src/filter/src/firdespm.c
  src/filter/src/firdespm_halfband.c
  src/filter/src/fnyquist.c
  src/filter/src/gmsk.c
  src/filter/src/group_delay.c
  src/filter/src/hM3.c
  src/filter/src/iirdes.pll.c
  src/filter/src/iirdes.c
  src/filter/src/lpc.c
  src/filter/src/rcos.c
  src/filter/src/rkaiser.c
  src/filter/src/rrcos.c
)

set(filter_AUTOTEST_SOURCES
  src/filter/tests/dds_cccf_autotest.c
  src/filter/tests/fdelay_rrrf_autotest.c
  src/filter/tests/fftfilt_xxxf_autotest.c
  src/filter/tests/filter_crosscorr_autotest.c
  src/filter/tests/firdecim_autotest.c
  src/filter/tests/firdecim_xxxf_autotest.c
  src/filter/tests/firdes_autotest.c
  src/filter/tests/firdespm_autotest.c
  src/filter/tests/firfilt_autotest.c
  src/filter/tests/firfilt_cccf_notch_autotest.c
  src/filter/tests/firfilt_coefficients_autotest.c
  src/filter/tests/firfilt_rnyquist_autotest.c
  src/filter/tests/firfilt_xxxf_autotest.c
  src/filter/tests/firfilt_copy_autotest.c
  src/filter/tests/firhilb_autotest.c
  src/filter/tests/firinterp_autotest.c
  src/filter/tests/firpfb_autotest.c
  src/filter/tests/groupdelay_autotest.c
  src/filter/tests/iirdecim_autotest.c
  src/filter/tests/iirdes_autotest.c
  src/filter/tests/iirdes_support_autotest.c
  src/filter/tests/iirfilt_xxxf_autotest.c
  src/filter/tests/iirfilt_copy_autotest.c
  src/filter/tests/iirfiltsos_autotest.c
  src/filter/tests/iirhilb_autotest.c
  src/filter/tests/iirinterp_autotest.c
  src/filter/tests/lpc_autotest.c
  src/filter/tests/msresamp_crcf_autotest.c
  src/filter/tests/msresamp2_crcf_autotest.c
  src/filter/tests/ordfilt_autotest.c
  src/filter/tests/rresamp_crcf_autotest.c
  src/filter/tests/rresamp_crcf_partition_autotest.c
  src/filter/tests/resamp_crcf_autotest.c
  src/filter/tests/resamp2_crcf_autotest.c
  src/filter/tests/symsync_copy_autotest.c
  src/filter/tests/symsync_crcf_autotest.c
  src/filter/tests/symsync_rrrf_autotest.c
)

list(APPEND autotest_EXTRA_SOURCES
  src/filter/tests/fftfilt_runtest.c
  src/filter/tests/data/fftfilt_rrrf_data_h4x256.c
  src/filter/tests/data/fftfilt_crcf_data_h4x256.c
  src/filter/tests/data/fftfilt_cccf_data_h4x256.c

  src/filter/tests/data/fftfilt_rrrf_data_h7x256.c
  src/filter/tests/data/fftfilt_crcf_data_h7x256.c
  src/filter/tests/data/fftfilt_cccf_data_h7x256.c

  src/filter/tests/data/fftfilt_rrrf_data_h13x256.c
  src/filter/tests/data/fftfilt_crcf_data_h13x256.c
  src/filter/tests/data/fftfilt_cccf_data_h13x256.c

  src/filter/tests/data/fftfilt_rrrf_data_h23x256.c
  src/filter/tests/data/fftfilt_crcf_data_h23x256.c
  src/filter/tests/data/fftfilt_cccf_data_h23x256.c

  src/filter/tests/firdecim_runtest.c

  src/filter/tests/data/firdecim_rrrf_data_M2h4x20.c
  src/filter/tests/data/firdecim_crcf_data_M2h4x20.c
  src/filter/tests/data/firdecim_cccf_data_M2h4x20.c

  src/filter/tests/data/firdecim_rrrf_data_M3h7x30.c
  src/filter/tests/data/firdecim_crcf_data_M3h7x30.c
  src/filter/tests/data/firdecim_cccf_data_M3h7x30.c

  src/filter/tests/data/firdecim_rrrf_data_M4h13x40.c
  src/filter/tests/data/firdecim_crcf_data_M4h13x40.c
  src/filter/tests/data/firdecim_cccf_data_M4h13x40.c

  src/filter/tests/data/firdecim_rrrf_data_M5h23x50.c
  src/filter/tests/data/firdecim_crcf_data_M5h23x50.c
  src/filter/tests/data/firdecim_cccf_data_M5h23x50.c

  src/filter/tests/firfilt_runtest.c

  src/filter/tests/data/firfilt_rrrf_data_h4x8.c
  src/filter/tests/data/firfilt_crcf_data_h4x8.c
  src/filter/tests/data/firfilt_cccf_data_h4x8.c

  src/filter/tests/data/firfilt_rrrf_data_h7x16.c
  src/filter/tests/data/firfilt_crcf_data_h7x16.c
  src/filter/tests/data/firfilt_cccf_data_h7x16.c

  src/filter/tests/data/firfilt_rrrf_data_h13x32.c
  src/filter/tests/data/firfilt_crcf_data_h13x32.c
  src/filter/tests/data/firfilt_cccf_data_h13x32.c

  src/filter/tests/data/firfilt_rrrf_data_h23x64.c
  src/filter/tests/data/firfilt_crcf_data_h23x64.c
  src/filter/tests/data/firfilt_cccf_data_h23x64.c

  src/filter/tests/iirfilt_runtest.c

  src/filter/tests/data/iirfilt_rrrf_data_h3x64.c
  src/filter/tests/data/iirfilt_crcf_data_h3x64.c
  src/filter/tests/data/iirfilt_cccf_data_h3x64.c

  src/filter/tests/data/iirfilt_rrrf_data_h5x64.c
  src/filter/tests/data/iirfilt_crcf_data_h5x64.c
  src/filter/tests/data/iirfilt_cccf_data_h5x64.c

  src/filter/tests/data/iirfilt_rrrf_data_h7x64.c
  src/filter/tests/data/iirfilt_crcf_data_h7x64.c
  src/filter/tests/data/iirfilt_cccf_data_h7x64.c
)

set(filter_AUTOTEST_BYPRODUCTS
  autotest/logs/dds_cccf_decim.m
  autotest/logs/dds_cccf_interp.m
  autotest/logs/dds_cccf_orig.m
  autotest/logs/firdes_doppler.m
  autotest/logs/firdes_prototype_arkaiser.m
  autotest/logs/firdes_prototype_farcsech.m
  autotest/logs/firdes_prototype_fexp.m
  autotest/logs/firdes_prototype_fsech.m
  autotest/logs/firdes_prototype_hm3.m
  autotest/logs/firdes_prototype_kaiser.m
  autotest/logs/firdes_prototype_pm.m
  autotest/logs/firdes_prototype_rcos.m
  autotest/logs/firdes_prototype_rfarcsech.m
  autotest/logs/firdes_prototype_rfexp.m
  autotest/logs/firdes_prototype_rfsech.m
  autotest/logs/firdes_prototype_rkaiser.m
  autotest/logs/firdes_prototype_rrcos.m
  autotest/logs/firdespm_callback.m
  autotest/logs/firdespm_halfband_m10_ft200.m
  autotest/logs/firdespm_halfband_m12_ft100.m
  autotest/logs/firdespm_halfband_m20_ft050.m
  autotest/logs/firdespm_halfband_m3_ft400.m
  autotest/logs/firdespm_halfband_m40_ft050.m
  autotest/logs/firdespm_halfband_m4_ft200.m
  autotest/logs/firdespm_halfband_m4_ft400.m
  autotest/logs/firdespm_halfband_m80_ft010.m
  autotest/logs/firdespm_lowpass.m
  autotest/logs/firfilt_cccf_notch.m
  autotest/logs/firfilt_crcf_firdespm.m
  autotest/logs/firfilt_crcf_kaiser.m
  autotest/logs/firfilt_crcf_notch.m
  autotest/logs/firfilt_crcf_rect.m
  autotest/logs/firhilbf_decim.m
  autotest/logs/firhilbf_interp.m
  autotest/logs/firhilbf_orig.m
  autotest/logs/iirdes_bessel.m
  autotest/logs/iirdes_butter_lowpass.m
  autotest/logs/iirdes_cheby1_lowpass.m
  autotest/logs/iirdes_cheby2_lowpass.m
  autotest/logs/iirdes_ellip_bandpass.m
  autotest/logs/iirdes_ellip_bandstop.m
  autotest/logs/iirdes_ellip_highpass.m
  autotest/logs/iirdes_ellip_lowpass.m
  autotest/logs/iirhilbf_decim.m
  autotest/logs/iirhilbf_filter_c2r.m
  autotest/logs/iirhilbf_filter_orig.m
  autotest/logs/iirhilbf_filter_r2c.m
  autotest/logs/iirhilbf_interp.m
  autotest/logs/iirhilbf_orig.m
  autotest/logs/msresamp2_crcf_interp_M16_f250_a60.m
  autotest/logs/msresamp2_crcf_interp_M16_f450_a60.m
  autotest/logs/msresamp2_crcf_interp_M2_f250_a60.m
  autotest/logs/msresamp2_crcf_interp_M2_f450_a60.m
  autotest/logs/msresamp2_crcf_interp_M4_f250_a60.m
  autotest/logs/msresamp2_crcf_interp_M4_f450_a60.m
  autotest/logs/msresamp2_crcf_interp_M8_f250_a60.m
  autotest/logs/msresamp2_crcf_interp_M8_f450_a100.m
  autotest/logs/msresamp2_crcf_interp_M8_f450_a60.m
  autotest/logs/msresamp2_crcf_interp_M8_f450_a80.m
  autotest/logs/msresamp2_crcf_interp_M8_f450_a90.m
  autotest/logs/msresamp_crcf_r127_a60_autotest.m
  autotest/logs/msresamp_crcf_r373_a60_autotest.m
  autotest/logs/msresamp_crcf_r676_a60_autotest.m
  autotest/logs/resamp2_crcf_filter_hi_m12_as60.m
  autotest/logs/resamp2_crcf_filter_hi_m15_as100.m
  autotest/logs/resamp2_crcf_filter_hi_m15_as120.m
  autotest/logs/resamp2_crcf_filter_hi_m15_as80.m
  autotest/logs/resamp2_crcf_filter_hi_m4_as60.m
  autotest/logs/resamp2_crcf_filter_hi_m7_as60.m
  autotest/logs/resamp2_crcf_filter_lo_m12_as60.m
  autotest/logs/resamp2_crcf_filter_lo_m15_as100.m
  autotest/logs/resamp2_crcf_filter_lo_m15_as120.m
  autotest/logs/resamp2_crcf_filter_lo_m15_as80.m
  autotest/logs/resamp2_crcf_filter_lo_m4_as60.m
  autotest/logs/resamp2_crcf_filter_lo_m7_as60.m
  autotest/logs/resamp_crcf_00.m
  autotest/logs/resamp_crcf_01.m
  autotest/logs/resamp_crcf_02.m
  autotest/logs/resamp_crcf_03.m
  autotest/logs/resamp_crcf_10.m
  autotest/logs/resamp_crcf_11.m
  autotest/logs/resamp_crcf_12.m
  autotest/logs/resamp_crcf_13.m
  autotest/logs/rresamp_crcf.m
)

set(filter_BENCHMARK_SOURCES
  src/filter/bench/fftfilt_crcf_benchmark.c
  src/filter/bench/firdecim_crcf_benchmark.c
  src/filter/bench/firhilb_benchmark.c
  src/filter/bench/firinterp_crcf_benchmark.c
  src/filter/bench/firfilt_crcf_benchmark.c
  src/filter/bench/iirdecim_crcf_benchmark.c
  src/filter/bench/iirfilt_crcf_benchmark.c
  src/filter/bench/iirinterp_crcf_benchmark.c
  src/filter/bench/rresamp_crcf_benchmark.c
  src/filter/bench/resamp_crcf_benchmark.c
  src/filter/bench/resamp2_crcf_benchmark.c
  src/filter/bench/symsync_crcf_benchmark.c
)

#
# MODULE : framing
#

set(framing_SOURCES
  src/framing/src/bpacketgen.c
  src/framing/src/bpacketsync.c
  src/framing/src/bpresync_cccf.c
  src/framing/src/bsync_rrrf.c
  src/framing/src/bsync_crcf.c
  src/framing/src/bsync_cccf.c
  src/framing/src/detector_cccf.c
  src/framing/src/dsssframegen.c
  src/framing/src/dsssframesync.c
  src/framing/src/framedatastats.c
  src/framing/src/framesyncstats.c
  src/framing/src/framegen64.c
  src/framing/src/framesync64.c
  src/framing/src/flexframegen.c
  src/framing/src/flexframesync.c
  src/framing/src/fskframegen.c
  src/framing/src/fskframesync.c
  src/framing/src/gmskframegen.c
  src/framing/src/gmskframesync.c
  src/framing/src/msourcecf.c
  src/framing/src/ofdmflexframegen.c
  src/framing/src/ofdmflexframesync.c
  src/framing/src/presync_cccf.c
  src/framing/src/symstreamcf.c
  src/framing/src/symstreamrcf.c
  src/framing/src/symtrack_cccf.c
  src/framing/src/qdetector_cccf.c
  src/framing/src/qpacketmodem.c
  src/framing/src/qpilotgen.c
  src/framing/src/qpilotsync.c
)

set(framing_AUTOTEST_SOURCES
  src/framing/tests/bpacketsync_autotest.c
  src/framing/tests/bsync_autotest.c
  src/framing/tests/detector_autotest.c
  src/framing/tests/dsssframesync_autotest.c
  src/framing/tests/flexframesync_autotest.c
  src/framing/tests/framesync64_autotest.c
  src/framing/tests/gmskframe_autotest.c
  src/framing/tests/msource_autotest.c
  src/framing/tests/ofdmflexframe_autotest.c
  src/framing/tests/qdetector_cccf_autotest.c
  src/framing/tests/qdetector_cccf_copy_autotest.c
  src/framing/tests/qpacketmodem_autotest.c
  src/framing/tests/qpilotsync_autotest.c
  src/framing/tests/qsource_autotest.c
  src/framing/tests/symstreamcf_autotest.c
  src/framing/tests/symstreamcf_delay_autotest.c
  src/framing/tests/symstreamrcf_autotest.c
  src/framing/tests/symstreamrcf_delay_autotest.c
  src/framing/tests/symtrack_cccf_autotest.c
)

set(framing_AUTOTEST_BYPRODUCTS
  autotest/logs/framesync64_n00000001.dat
  autotest/logs/framesync64_u00000001.dat
  autotest/logs/msourcecf_accessor_autotest.m
  autotest/logs/msourcecf_aggregate_autotest.m
  autotest/logs/msourcecf_chirp_autotest.m
  autotest/logs/msourcecf_copy.m
  autotest/logs/msourcecf_tone_autotest.m
  autotest/logs/symstreamcf_psd_k2_m12_b030_autotest.m
  autotest/logs/symstreamcf_psd_k4_m12_b030_autotest.m
  autotest/logs/symstreamcf_psd_k4_m25_b020_autotest.m
  autotest/logs/symstreamcf_psd_k7_m11_b035_autotest.m
  autotest/logs/symstreamrcf_psd_bw200_m12_b030_autotest.m
  autotest/logs/symstreamrcf_psd_bw400_m12_b030_autotest.m
  autotest/logs/symstreamrcf_psd_bw400_m25_b020_autotest.m
  autotest/logs/symstreamrcf_psd_bw700_m11_b035_autotest.m
)

# NOTE: most autotest/logs/framesync64_*.dat byproducts are non-deterministic
#       and require a wildcard to remove automatically
set(framing_AUTOTEST_GLOB_BYPRODUCTS autotest/logs/framesync64_*.dat)

set(framing_BENCHMARK_SOURCES
  src/framing/bench/presync_benchmark.c
  src/framing/bench/bpacketsync_benchmark.c
  src/framing/bench/bpresync_benchmark.c
  src/framing/bench/bsync_benchmark.c
  src/framing/bench/detector_benchmark.c
  src/framing/bench/flexframesync_benchmark.c
  src/framing/bench/framesync64_benchmark.c
  src/framing/bench/gmskframesync_benchmark.c
  src/framing/bench/qdetector_benchmark.c
)

#
# MODULE : math
#

set(math_SOURCES
  src/math/src/poly.c
  src/math/src/polyc.c
  src/math/src/polyf.c
  src/math/src/polycf.c
  src/math/src/math.c
  src/math/src/math.bessel.c
  src/math/src/math.gamma.c
  src/math/src/math.complex.c
  src/math/src/math.trig.c
  src/math/src/modular_arithmetic.c
  src/math/src/poly.findroots.c
  src/math/src/windows.c
)

set(math_AUTOTEST_SOURCES
  src/math/tests/gcd_autotest.c
  src/math/tests/window_autotest.c
  src/math/tests/math_autotest.c
  src/math/tests/math_bessel_autotest.c
  src/math/tests/math_gamma_autotest.c
  src/math/tests/math_complex_autotest.c
  src/math/tests/polynomial_autotest.c
  src/math/tests/polynomial_findroots_autotest.c
  src/math/tests/prime_autotest.c
)

set(math_BENCHMARK_SOURCES
  src/math/bench/polyfit_benchmark.c
)

#
# MODULE : matrix
#

set(matrix_SOURCES
  src/matrix/src/matrix.c
  src/matrix/src/matrixf.c
  src/matrix/src/matrixc.c
  src/matrix/src/matrixcf.c
  src/matrix/src/smatrix.common.c
  src/matrix/src/smatrixb.c
  src/matrix/src/smatrixf.c
  src/matrix/src/smatrixi.c
)

set(matrix_AUTOTEST_SOURCES
  src/matrix/tests/matrixcf_autotest.c
  src/matrix/tests/matrixf_autotest.c
  src/matrix/tests/smatrixb_autotest.c
  src/matrix/tests/smatrixf_autotest.c
  src/matrix/tests/smatrixi_autotest.c
)

list(APPEND autotest_EXTRA_SOURCES
  src/matrix/tests/data/matrixf_data_add.c
  src/matrix/tests/data/matrixf_data_aug.c
  src/matrix/tests/data/matrixf_data_cgsolve.c
  src/matrix/tests/data/matrixf_data_chol.c
  src/matrix/tests/data/matrixf_data_gramschmidt.c
  src/matrix/tests/data/matrixf_data_inv.c
  src/matrix/tests/data/matrixf_data_linsolve.c
  src/matrix/tests/data/matrixf_data_ludecomp.c
  src/matrix/tests/data/matrixf_data_mul.c
  src/matrix/tests/data/matrixf_data_qrdecomp.c
  src/matrix/tests/data/matrixf_data_transmul.c

  src/matrix/tests/data/matrixcf_data_add.c
  src/matrix/tests/data/matrixcf_data_aug.c
  src/matrix/tests/data/matrixcf_data_chol.c
  src/matrix/tests/data/matrixcf_data_inv.c
  src/matrix/tests/data/matrixcf_data_linsolve.c
  src/matrix/tests/data/matrixcf_data_ludecomp.c
  src/matrix/tests/data/matrixcf_data_mul.c
  src/matrix/tests/data/matrixcf_data_qrdecomp.c
  src/matrix/tests/data/matrixcf_data_transmul.c
)

set(matrix_BENCHMARK_SOURCES
  src/matrix/bench/matrixf_inv_benchmark.c
  src/matrix/bench/matrixf_linsolve_benchmark.c
  src/matrix/bench/matrixf_mul_benchmark.c
  src/matrix/bench/smatrixf_mul_benchmark.c
)

#
# MODULE : modem
#

set(modem_SOURCES
  src/modem/src/ampmodem.c
  src/modem/src/cpfskdem.c
  src/modem/src/cpfskmod.c
  src/modem/src/fskdem.c
  src/modem/src/fskmod.c
  src/modem/src/gmskdem.c
  src/modem/src/gmskmod.c
  src/modem/src/modem.shim.c
  src/modem/src/modemcf.c
  src/modem/src/modem_utilities.c
  src/modem/src/modem_apsk_const.c
  src/modem/src/modem_arb_const.c
)

set(modem_AUTOTEST_SOURCES
  src/modem/tests/ampmodem_autotest.c
  src/modem/tests/cpfskmodem_autotest.c
  src/modem/tests/freqmodem_autotest.c
  src/modem/tests/fskmodem_autotest.c
  src/modem/tests/gmskmodem_autotest.c
  src/modem/tests/modem_autotest.c
  src/modem/tests/modem_copy_autotest.c
  src/modem/tests/modem_demodsoft_autotest.c
  src/modem/tests/modem_demodstats_autotest.c
  src/modem/tests/modem_utilities_autotest.c
)

set(modem_BENCHMARK_SOURCES
  src/modem/bench/freqdem_benchmark.c
  src/modem/bench/freqmod_benchmark.c
  src/modem/bench/fskdem_benchmark.c
  src/modem/bench/fskmod_benchmark.c
  src/modem/bench/gmskmodem_benchmark.c
  src/modem/bench/modem_modulate_benchmark.c
  src/modem/bench/modem_demodulate_benchmark.c
  src/modem/bench/modem_demodsoft_benchmark.c
)

#
# MODULE : multichannel
#

set(multichannel_SOURCES
  src/multichannel/src/firpfbch_crcf.c
  src/multichannel/src/firpfbch_cccf.c
  src/multichannel/src/ofdmframe.common.c
  src/multichannel/src/ofdmframegen.c
  src/multichannel/src/ofdmframesync.c
)

set(multichannel_AUTOTEST_SOURCES
  src/multichannel/tests/firpfbch2_crcf_autotest.c
  src/multichannel/tests/firpfbch_crcf_synthesizer_autotest.c
  src/multichannel/tests/firpfbch_crcf_analyzer_autotest.c
  src/multichannel/tests/ofdmframesync_autotest.c
)

set(multichannel_BENCHMARK_SOURCES
  src/multichannel/bench/firpfbch_crcf_benchmark.c
  src/multichannel/bench/firpfbch2_crcf_benchmark.c
  src/multichannel/bench/firpfbchr_crcf_benchmark.c
  src/multichannel/bench/ofdmframesync_acquire_benchmark.c
  src/multichannel/bench/ofdmframesync_rxsymbol_benchmark.c
)

#
# MODULE : nco - numerically-controlled oscillator
#

set(nco_SOURCES
  src/nco/src/nco_crcf.c
  src/nco/src/nco.utilities.c
)

set(nco_AUTOTEST_SOURCES
  src/nco/tests/nco_crcf_copy_autotest.c
  src/nco/tests/nco_crcf_frequency_autotest.c
  src/nco/tests/nco_crcf_mix_autotest.c
  src/nco/tests/nco_crcf_phase_autotest.c
  src/nco/tests/nco_crcf_pll_autotest.c
  src/nco/tests/unwrap_phase_autotest.c
)

list(APPEND autotest_EXTRA_SOURCES
  src/nco/tests/data/nco_sincos_fsqrt1_2.c
  src/nco/tests/data/nco_sincos_fsqrt1_3.c
  src/nco/tests/data/nco_sincos_fsqrt1_5.c
  src/nco/tests/data/nco_sincos_fsqrt1_7.c
)

set(nco_BENCHMARK_SOURCES
  src/nco/bench/nco_benchmark.c
  src/nco/bench/vco_benchmark.c
)

#
# MODULE : optim - optimization
#

set(optim_SOURCES
  src/optim/src/chromosome.c
  src/optim/src/gasearch.c
  src/optim/src/gradsearch.c
  src/optim/src/optim.common.c
  src/optim/src/qnsearch.c
  src/optim/src/qs1dsearch.c
  src/optim/src/utilities.c
)

set(optim_AUTOTEST_SOURCES
  src/optim/tests/gradsearch_autotest.c
  src/optim/tests/qs1dsearch_autotest.c
)

set(optim_BENCHMARK_SOURCES)

#
# MODULE : quantization
#

set(quantization_SOURCES
  src/quantization/src/compand.c
  src/quantization/src/quantizercf.c
  src/quantization/src/quantizerf.c
  src/quantization/src/quantizer.inline.c
)

set(quantization_AUTOTEST_SOURCES
  src/quantization/tests/compand_autotest.c
  src/quantization/tests/quantize_autotest.c
)

set(quantization_BENCHMARK_SOURCES
  src/quantization/bench/quantizer_benchmark.c
  src/quantization/bench/compander_benchmark.c
)

#
# MODULE : random
#

set(random_SOURCES
  src/random/src/rand.c
  src/random/src/randn.c
  src/random/src/randexp.c
  src/random/src/randweib.c
  src/random/src/randgamma.c
  src/random/src/randnakm.c
  src/random/src/randricek.c
  src/random/src/scramble.c
)

set(random_AUTOTEST_SOURCES src/random/tests/scramble_autotest.c)

set(random_BENCHMARK_SOURCES
  src/random/bench/random_benchmark.c
)

#
# MODULE : sequence
#

set(sequence_SOURCES
  src/sequence/src/bsequence.c
  src/sequence/src/msequence.c
)

set(sequence_AUTOTEST_SOURCES
  src/sequence/tests/bsequence_autotest.c
  src/sequence/tests/complementary_codes_autotest.c
  src/sequence/tests/msequence_autotest.c
)

set(sequence_BENCHMARK_SOURCES
  src/sequence/bench/bsequence_benchmark.c
)

#
# MODULE : utility
#

set(utility_SOURCES
  src/utility/src/bshift_array.c
  src/utility/src/byte_utilities.c
  src/utility/src/memory.c
  src/utility/src/msb_index.c
  src/utility/src/pack_bytes.c
  src/utility/src/shift_array.c
  src/utility/src/utility.c
)

set(utility_AUTOTEST_SOURCES
  src/utility/tests/bshift_array_autotest.c
  src/utility/tests/count_bits_autotest.c
  src/utility/tests/pack_bytes_autotest.c
  src/utility/tests/shift_array_autotest.c
)

set(utility_BENCHMARK_SOURCES
  src/utility/bench/byte_utilities_benchmark.c
)

#
# MODULE : vector
#

set(vector_SOURCES
  src/vector/src/vectorf_add.port.c
  src/vector/src/vectorf_norm.port.c
  src/vector/src/vectorf_mul.port.c
  src/vector/src/vectorf_trig.port.c
  src/vector/src/vectorcf_add.port.c
  src/vector/src/vectorcf_norm.port.c
  src/vector/src/vectorcf_mul.port.c
  src/vector/src/vectorcf_trig.port.c
)

set(vector_AUTOTEST_SOURCES)

set(vector_BENCHMARK_SOURCES)

# Target collection
#
# Information about targets for each module is collected
# in these variables

set(liquid_SOURCES
  src/libliquid.c
  ${agc_SOURCES}
  ${audio_SOURCES}
  ${buffer_SOURCES}
  ${channel_SOURCES}
  ${dotprod_SOURCES}
  ${equalization_SOURCES}
  ${fec_SOURCES}
  ${fft_SOURCES}
  ${filter_SOURCES}
  ${framing_SOURCES}
  ${math_SOURCES}
  ${matrix_SOURCES}
  ${modem_SOURCES}
  ${multichannel_SOURCES}
  ${nco_SOURCES}
  ${optim_SOURCES}
  ${quantization_SOURCES}
  ${random_SOURCES}
  ${sequence_SOURCES}
  ${utility_SOURCES}
  ${vector_SOURCES}
)

set(autotest_SOURCES
  autotest/null_autotest.c
  autotest/libliquid_autotest.c
  ${agc_AUTOTEST_SOURCES}
  ${audio_AUTOTEST_SOURCES}
  ${buffer_AUTOTEST_SOURCES}
  ${channel_AUTOTEST_SOURCES}
  ${dotprod_AUTOTEST_SOURCES}
  ${equalization_AUTOTEST_SOURCES}
  ${fec_AUTOTEST_SOURCES}
  ${fft_AUTOTEST_SOURCES}
  ${filter_AUTOTEST_SOURCES}
  ${framing_AUTOTEST_SOURCES}
  ${math_AUTOTEST_SOURCES}
  ${matrix_AUTOTEST_SOURCES}
  ${modem_AUTOTEST_SOURCES}
  ${multichannel_AUTOTEST_SOURCES}
  ${nco_AUTOTEST_SOURCES}
  ${optim_AUTOTEST_SOURCES}
  ${quantization_AUTOTEST_SOURCES}
  ${random_AUTOTEST_SOURCES}
  ${sequence_AUTOTEST_SOURCES}
  ${utility_AUTOTEST_SOURCES}
  ${vector_AUTOTEST_SOURCES}
)

set(autotest_BYPRODUCTS
  ${fft_AUTOTEST_BYPRODUCTS}
  ${filter_AUTOTEST_BYPRODUCTS}
  ${framing_AUTOTEST_BYPRODUCTS}
)

set(benchmark_SOURCES
  bench/null_benchmark.c
  ${agc_BENCHMARK_SOURCES}
  ${audio_BENCHMARK_SOURCES}
  ${buffer_BENCHMARK_SOURCES}
  ${channel_BENCHMARK_SOURCES}
  ${dotprod_BENCHMARK_SOURCES}
  ${equalization_BENCHMARK_SOURCES}
  ${fec_BENCHMARK_SOURCES}
  ${fft_BENCHMARK_SOURCES}
  ${filter_BENCHMARK_SOURCES}
  ${framing_BENCHMARK_SOURCES}
  ${math_BENCHMARK_SOURCES}
  ${matrix_BENCHMARK_SOURCES}
  ${modem_BENCHMARK_SOURCES}
  ${multichannel_BENCHMARK_SOURCES}
  ${nco_BENCHMARK_SOURCES}
  ${optim_BENCHMARK_SOURCES}
  ${quantization_BENCHMARK_SOURCES}
  ${random_BENCHMARK_SOURCES}
  ${sequence_BENCHMARK_SOURCES}
  ${utility_BENCHMARK_SOURCES}
  ${vector_BENCHMARK_SOURCES}
)

##
## TARGET : all       - build shared library (default)
##

# liquid configure options

add_library(liquid_config INTERFACE)

target_include_directories(liquid_config INTERFACE . include)

target_compile_options(liquid_config INTERFACE
  -Wall -Wno-deprecated -Wno-deprecated-declarations
  $<$<COMPILE_LANG_AND_ID:CXX,AppleClang,Clang>:-Wno-return-type-c-linkage>
)

if(LIQUID_DEBUG_MESSAGES)
  target_compile_definitions(liquid_config INTERFACE DEBUG)
endif()

if(LIQUID_COVERAGE)
    target_compile_options(liquid_config INTERFACE --coverage)
    target_link_options(liquid_config INTERFACE --coverage)
endif()

target_link_libraries(liquid_config INTERFACE arch_option)

# liquid library

add_library(liquid OBJECT ${liquid_SOURCES})

target_link_libraries(liquid liquid_config)

if(HAVE_LIBC)
  target_link_libraries(liquid c)
endif()
if(HAVE_LIBM)
  target_link_libraries(liquid m)
endif()
if(fec_FOUND)
  target_link_libraries(liquid fec::fec)
endif()
if(NOT LIQUID_FFTOVERRIDE AND FFTW_FOUND)
  target_link_libraries(liquid FFTW::fftw3f)
endif()

add_library(liquid-shared SHARED)
target_link_libraries(liquid-shared liquid)

add_library(liquid-static STATIC)
target_link_libraries(liquid-static liquid)

set_target_properties(liquid-shared liquid-static PROPERTIES
  OUTPUT_NAME liquid
  SOVERSION ${PROJECT_VERSION_MAJOR}
  VERSION ${PROJECT_VERSION}
)

set_property(TARGET liquid-static PROPERTY
  SUFFIX ${CMAKE_STATIC_LIBRARY_SUFFIX}.${PROJECT_VERSION}
)

##
## TARGET : install   - installs the libraries and headers in the host system
##

install(TARGETS liquid-shared liquid-static
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/liquid
)

install(FILES include/liquid.h
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/liquid
)

install(CODE "
message([===[

---------------------------------------------------------
  liquid-dsp was successfully installed.

  On some machines (e.g. Linux) you should rebind your
  libraries by running 'ldconfig' to make the shared
  object available.  You might also need to modify your
  LD_LIBRARY_PATH environment variable to include the
  install directory (e.g. ${CMAKE_INSTALL_PREFIX})

  Please report bugs to ${PACKAGE_BUGREPORT}
---------------------------------------------------------

]===]
)
"
)

##
## TARGET : uninstall - uninstalls the libraries and headers in the host system
##

configure_file(cmake/uninstall.CMakeLists.txt.in cmake_uninstall.cmake @ONLY)

add_custom_target(uninstall
  COMMAND ${CMAKE_COMMAND}
          -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  COMMENT "Uninstall the project..."
)


##
## autoscript
##

add_executable(autoscript EXCLUDE_FROM_ALL scripts/autoscript.c scripts/main.c)
set_property(TARGET autoscript PROPERTY
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/scripts"
)

##
## TARGET : check     - build and run autotest scripts
##

# Autotests are used to check the validity and accuracy of the
# DSP libraries.

add_custom_command(
  OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/autotest_include.h"
  COMMAND scripts/autoscript
          $<SHELL_PATH:/> autotest
          ${autotest_SOURCES}
          > autotest_include.h
  DEPENDS autoscript
  COMMENT "run the autotest generator script to create autotest_include.h"
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

add_executable(xautotest EXCLUDE_FROM_ALL)
target_sources(xautotest PRIVATE
  autotest/xautotest.c
  autotest/autotest.c
  autotest_include.h
  ${autotest_SOURCES}
  ${autotest_EXTRA_SOURCES}
)
target_link_libraries(xautotest PRIVATE liquid-static)
set_property(TARGET xautotest PROPERTY
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

add_custom_command(
  OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/autotest.json"
  COMMAND xautotest -v -o autotest.json
  DEPENDS xautotest
  COMMENT "run the autotest program"
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

list(TRANSFORM autotest_BYPRODUCTS PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/"
  OUTPUT_VARIABLE autotest_FULL_BYPRODUCTS
)

add_custom_target(check
  COMMAND ""
  DEPENDS autotest.json
  BYPRODUCTS ${autotest_FULL_BYPRODUCTS}
  COMMENT "checking for autotest.json"
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

enable_testing()

add_test(NAME check
  COMMAND xautotest -v
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

##
## TARGET : doc-check - build and run basic documentation checks
##

file(READ "${CMAKE_CURRENT_SOURCE_DIR}/README.md" readme_md)
string(REPLACE ";" "\\\;" readme_md "${readme_md}")
string(REPLACE "\n" ";" readme_md "${readme_md}")

list(SUBLIST readme_md 23 20 readme_c_example)
list(JOIN readme_c_example "\n" readme_c_example)
string(REPLACE
  [[<liquid/liquid.h>]] [["liquid.h"]]
  readme_c_example "${readme_c_example}"
)
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/readme.c.example.c"
  "${readme_c_example}"
)

add_executable(readme.c.example EXCLUDE_FROM_ALL
  "${CMAKE_CURRENT_BINARY_DIR}/readme.c.example.c"
)
target_link_libraries(readme.c.example PRIVATE liquid-static)

if(CMAKE_CXX_COMPILER)
  list(SUBLIST readme_md 152 21 readme_cc_example)
  list(JOIN readme_cc_example "\n" readme_cc_example)
  string(REPLACE
    [[<liquid/liquid.h>]] [["liquid.h"]]
    readme_cc_example "${readme_cc_example}"
  )

  file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/readme.cc.example.cc"
    "${readme_cc_example}"
  )

  add_executable(readme.cc.example EXCLUDE_FROM_ALL
    "${CMAKE_CURRENT_BINARY_DIR}/readme.cc.example.cc"
  )
  target_link_libraries(readme.cc.example PRIVATE liquid-static)

  add_custom_target(doc-check
    COMMAND readme.c.example
    COMMAND readme.cc.example
    DEPENDS readme.c.example readme.cc.example
  )
else()
  add_custom_target(doc-check
    COMMAND readme.c.example
    DEPENDS readme.c.example
  )
endif()

##
## TARGET : coverage  - run coverage test and produce report
##

if(LIQUID_COVERAGE)
  add_custom_command(
    OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/coverage.out"
    COMMAND gcovr --print-summary
            --root=src "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/liquid.dir"
            > coverage.out
    DEPENDS check
    COMMENT "run coverage test and produce report"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
  )

  add_custom_target(coverage
    COMMAND tail -n5 coverage.out
    DEPENDS coverage.out
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
  )
else()
  add_custom_target(coverage
    COMMAND ""
    COMMENT "Enable with LIQUID_COVERAGE option. Requries gcovr."
  )
endif()

##
## TARGET : bench     - build and run all benchmarks
##

# Benchmarks measure the relative speed of the DSP algorithms running
# on the target platform.

add_custom_command(
  OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/benchmark_include.h"
  COMMAND scripts/autoscript
          $<SHELL_PATH:/> benchmark
          ${benchmark_SOURCES}
          > benchmark_include.h
  DEPENDS autoscript
  COMMENT "run the benchmark generator script to create benchmark_include.h"
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

add_executable(benchmark EXCLUDE_FROM_ALL)
target_sources(benchmark PRIVATE
  bench/bench.c
  benchmark_include.h
  ${benchmark_SOURCES}
  ${benchmark_EXTRA_SOURCES}
)
target_link_libraries(benchmark PRIVATE liquid-static)
set_property(TARGET benchmark PROPERTY
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

add_custom_command(
  OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/benchmark.json"
  COMMAND benchmark -o benchmark.json
  DEPENDS benchmark
  COMMENT "run the benchmark program"
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

add_custom_target(bench
  DEPENDS benchmark.json
  COMMENT "checking for benchmark.json"
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

##
## TARGET : examples  - build all examples binaries
##

set(examples_SOURCES
  examples/agc_crcf_example.c
  examples/agc_crcf_squelch_example.c
  examples/agc_crcf_qpsk_example.c
  examples/agc_rrrf_example.c
  examples/ampmodem_example.c
  examples/asgramcf_example.c
  examples/asgramf_example.c
  examples/autocorr_cccf_example.c
  examples/bpacketsync_example.c
  examples/bpresync_example.c
  examples/bsequence_example.c
  examples/cbufferf_example.c
  examples/cgsolve_example.c
  examples/channel_cccf_example.c
  examples/compand_example.c
  examples/compand_cf_example.c
  examples/complementary_codes_example.c
  examples/conversion_example.c
  examples/crc_example.c
  examples/cpfskmodem_example.c
  examples/cpfskmodem_psd_example.c
  examples/cvsd_example.c
  examples/detector_cccf_example.c
  examples/dds_cccf_example.c
  examples/dsssframesync_example.c
  examples/dotprod_rrrf_example.c
  examples/dotprod_cccf_example.c
  examples/eqlms_cccf_block_example.c
  examples/eqlms_cccf_blind_example.c
  examples/eqlms_cccf_decisiondirected_example.c
  examples/eqlms_cccf_example.c
  examples/eqrls_cccf_example.c
  examples/error_handling_example.c
  examples/fec_example.c
  examples/fec_soft_example.c
  examples/fdelay_rrrf_example.c
  examples/fft_example.c
  examples/fftfilt_crcf_example.c
  examples/firdecim_crcf_example.c
  examples/firfarrow_rrrf_example.c
  examples/firfilt_cccf_example.c
  examples/firfilt_cccf_notch_example.c
  examples/firfilt_crcf_copy_example.c
  examples/firfilt_crcf_example.c
  examples/firfilt_crcf_dcblocker_example.c
  examples/firfilt_rrrf_example.c
  examples/firdes_doppler_example.c
  examples/firdes_kaiser_example.c
  examples/firdespm_callback_example.c
  examples/firdespm_halfband_example.c
  examples/firdespm_example.c
  examples/firdespm_lowpass_example.c
  examples/firhilb_example.c
  examples/firhilb_decim_example.c
  examples/firhilb_filter_example.c
  examples/firhilb_interp_example.c
  examples/firpfb_rrrf_example.c
  examples/firpfbch2_crcf_example.c
  examples/firpfbch2_crcf_reconstruct_example.c
  examples/firpfbchr_crcf_example.c
  examples/firinterp_crcf_example.c
  examples/firinterp_firdecim_crcf_example.c
  examples/firinterp_rrrf_linear_example.c
  examples/firinterp_rrrf_window_example.c
  examples/firpfbch_crcf_example.c
  examples/firpfbch_crcf_analysis_example.c
  examples/firpfbch_crcf_msource_example.c
  examples/firpfbch_crcf_synthesis_example.c
  examples/flexframesync_example.c
  examples/flexframesync_reconfig_example.c
  examples/framesync64_example.c
  examples/framesync64_performance_example.c
  examples/freqmodem_example.c
  examples/fskframesync_example.c
  examples/fskmodem_example.c
  examples/fskmodem_waterfall_example.c
  examples/gasearch_example.c
  examples/gasearch_knapsack_example.c
  examples/gmskframesync_example.c
  examples/gmskmodem_example.c
  examples/gmsk_eqlms_example.c
  examples/gmsk_tracking_example.c
  examples/gradsearch_datafit_example.c
  examples/gradsearch_example.c
  examples/interleaver_example.c
  examples/interleaver_soft_example.c
  examples/interleaver_scatterplot_example.c
  examples/iirdes_example.c
  examples/iirdes_analog_example.c
  examples/iirdes_pll_example.c
  examples/iirdecim_crcf_example.c
  examples/iirfilt_cccf_example.c
  examples/iirfilt_crcf_example.c
  examples/iirfilt_crcf_dcblocker_example.c
  examples/iirhilb_example.c
  examples/iirhilb_filter_example.c
  examples/iirinterp_crcf_example.c
  examples/kaiser_window_example.c
  examples/kbd_window_example.c
  examples/lpc_example.c
  examples/libliquid_example.c
  examples/matched_filter_example.c
  examples/math_lngamma_example.c
  examples/math_primitive_root_example.c
  examples/modem_arb_example.c
  examples/modem_example.c
  examples/modem_pi4dqpsk_example.c
  examples/modem_soft_example.c
  examples/modular_arithmetic_example.c
  examples/msequence_generator_example.c
  examples/msequence_example.c
  examples/msourcecf_example.c
  examples/msresamp_crcf_example.c
  examples/msresamp_crcf_noise_example.c
  examples/msresamp2_crcf_example.c
  examples/nco_crcf_mix_example.c
  examples/nco_crcf_tone_example.c
  examples/nco_example.c
  examples/nco_pll_example.c
  examples/nco_pll_real_example.c
  examples/nco_pll_modem_example.c
  examples/nyquist_filter_example.c
  examples/ofdmflexframesync_example.c
  examples/ofdmframesync_example.c
  examples/ordfilt_rrrf_example.c
  examples/packetizer_example.c
  examples/packetizer_soft_example.c
  examples/pll_example.c
  examples/polyfit_comparison_example.c
  examples/polyfit_example.c
  examples/polyfit_lagrange_example.c
  examples/poly_findroots_example.c
  examples/qdetector_cccf_example.c
  examples/qpacketmodem_performance_example.c
  examples/qpacketmodem_example.c
  examples/qpilotsync_example.c
  examples/qnsearch_example.c
  examples/qs1dsearch_example.c
  examples/quantize_example.c
  examples/random_histogram_example.c
  examples/repack_bytes_example.c
  examples/rresamp_crcf_example.c
  examples/rresamp_crcf_partition_example.c
  examples/rresamp_crcf_rnyquist_example.c
  examples/rresamp_rrrf_example.c
  examples/resamp_crcf_example.c
  examples/resamp_crcf_noise_example.c
  examples/resamp_crcf_rate_match_example.c
  examples/resamp2_cccf_example.c
  examples/resamp2_crcf_example.c
  examples/resamp2_crcf_decim_example.c
  examples/resamp2_crcf_filter_example.c
  examples/resamp2_crcf_interp_example.c
  examples/ricek_channel_example.c
  examples/scramble_example.c
  examples/smatrix_example.c
  examples/spgramcf_example.c
  examples/spgramf_example.c
  examples/spwaterfallcf_example.c
  examples/symsync_crcf_example.c
  examples/symsync_crcf_full_example.c
  examples/symsync_crcf_kaiser_example.c
  examples/symstreamcf_delay_example.c
  examples/symstreamcf_example.c
  examples/symstreamrcf_delay_example.c
  examples/symstreamrcf_example.c
  examples/symtrack_cccf_example.c
  examples/wdelayf_example.c
  examples/windowf_example.c
  examples/window_enbw_example.c
  examples/windowing_example.c
)

foreach(example_source ${examples_SOURCES})
  get_filename_component(example ${example_source} NAME_WE)
  list(APPEND examples_TARGETS ${example})
  add_executable(${example} EXCLUDE_FROM_ALL ${example_source})
  target_link_libraries(${example} PRIVATE liquid-static)
  set_property(TARGET ${example} PROPERTY
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/examples"
  )
endforeach()

add_custom_target(examples)
add_dependencies(examples ${examples_TARGETS})

##
## TARGET : sandbox   - build all sandbox binaries
##

if (FFTW_FOUND)
  set(sandbox_SOURCES
    sandbox/am_demod_dsb_peak_detect_test.c
    sandbox/am_demod_dsb_pll_carrier_test.c
    sandbox/am_demod_dsb_pll_costas_test.c
    sandbox/am_demod_ssb_pll_carrier_test.c
    sandbox/bpresync_test.c
    sandbox/chromosome_test.c
    sandbox/cpmodem_test.c
    sandbox/count_ones_gentab.c
    sandbox/crc_gentab.c
    sandbox/ellip_func_test.c
    sandbox/ellip_test.c
    sandbox/eqlms_cccf_test.c
    sandbox/fecsoft_ber_test.c
    sandbox/fec_g2412product_test.c
    sandbox/fec_golay2412_test.c
    sandbox/fec_golay_test.c
    sandbox/fec_hamming3126_example.c
    sandbox/fec_hamming128_test.c
    sandbox/fec_hamming128_gentab.c
    sandbox/fec_hamming128_example.c
    sandbox/fec_hamming74_gentab.c
    sandbox/fec_hamming84_gentab.c
    sandbox/fec_hamming_test.c
    sandbox/fec_ldpc_test.c
    sandbox/fec_rep3_test.c
    sandbox/fec_rep5_test.c
    sandbox/fec_secded2216_test.c
    sandbox/fec_secded3932_test.c
    sandbox/fec_secded7264_test.c
    sandbox/fec_spc2216_test.c
    sandbox/fec_secded_punctured_test.c
    sandbox/fecsoft_conv_test.c
    sandbox/fecsoft_hamming128_gentab.c
    sandbox/fecsoft_ldpc_test.c
    sandbox/fec_sumproduct_test.c
    sandbox/fskcorr_test.c
    sandbox/fskmodem_test.c
    sandbox/fft_dual_radix_test.c
    sandbox/fft_mixed_radix_test.c
    sandbox/fft_recursive_plan_test.c
    sandbox/fft_recursive_test.c
    sandbox/fft_rader_prime_test.c
    sandbox/fft_rader_prime_radix2_test.c
    sandbox/fft_r2r_test.c
    sandbox/firdes_energy_test.c
    sandbox/firdes_fexp_test.c
    sandbox/firdes_gmskrx_test.c
    sandbox/firdes_group_delay_test.c
    sandbox/firdes_length_test.c
    sandbox/firdespm_halfband_test.c
    sandbox/firfarrow_rrrf_test.c
    sandbox/firfilt_dcblocker_test.c
    sandbox/firpfbch_analysis_alignment_test.c
    sandbox/firpfbch2_analysis_equivalence_test.c
    sandbox/firpfbch2_test.c
    sandbox/firpfbch2_flatness_test.c
    sandbox/firpfbch_analysis_equivalence_test.c
    sandbox/firpfbch_synthesis_equivalence_test.c
    sandbox/gmskmodem_test.c
    sandbox/gmskmodem_coherent_test.c
    sandbox/gmskmodem_equalizer_test.c
    sandbox/gmskmodem_psd_filter_compare_test.c
    sandbox/householder_test.c
    sandbox/iirdes_test.c
    sandbox/iirdes_gradsearch_test.c
    sandbox/iirfilt_intdiff_test.c
    sandbox/levinson_test.c
    sandbox/matched_filter_test.c
    sandbox/matched_filter_cfo_test.c
    sandbox/math_lngamma_test.c
    sandbox/math_cacosf_test.c
    sandbox/math_casinf_test.c
    sandbox/math_catanf_test.c
    sandbox/math_cexpf_test.c
    sandbox/math_clogf_test.c
    sandbox/math_csqrtf_test.c
    sandbox/matrix_test.c
    sandbox/minsearch_test.c
    sandbox/minsearch2_test.c
    sandbox/matrix_eig_test.c
    sandbox/modem_demodulate_arb_gentab.c
    sandbox/modem_demodulate_soft_test.c
    sandbox/modem_demodulate_soft_gentab.c
    sandbox/mskmodem_test.c
    sandbox/msresamp_crcf_test.c
    sandbox/ofdmoqam_firpfbch_test.c
    sandbox/ofdm_ber_test.c
    sandbox/ofdmframe_papr_test.c
    sandbox/ofdmframesync_cfo_test.c
    sandbox/pll_3rd_order_test.c
    sandbox/pll_design_test.c
    sandbox/predemod_sync_test.c
    sandbox/quasinewton_test.c
    sandbox/recursive_qpsk_test.c
    sandbox/resamp2_crcf_filterbank_test.c
    sandbox/resamp2_crcf_interp_recreate_test.c
    sandbox/reverse_byte_gentab.c
    sandbox/rhamming_test.c
    sandbox/rkaiser2_test.c
    sandbox/shadowing_test.c
    sandbox/simplex_test.c
    sandbox/symsync_crcf_test.c
    sandbox/symsync_eqlms_test.c
    sandbox/svd_test.c
    sandbox/thiran_allpass_iir_test.c
    sandbox/vectorcf_test.c
  )

  foreach(sandbox_source ${sandbox_SOURCES})
    get_filename_component(sandbox ${sandbox_source} NAME_WE)
    list(APPEND sandbox_TARGETS ${sandbox})
    add_executable(${sandbox} EXCLUDE_FROM_ALL ${sandbox_source})
    target_link_libraries(${sandbox} PRIVATE liquid-static FFTW::fftw3f)
    set_property(TARGET ${sandbox} PROPERTY
      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/sandbox"
    )
  endforeach()

  add_custom_target(sandbox)
  add_dependencies(sandbox ${sandbox_TARGETS})
else()
  add_custom_target(sandbox
    COMMAND ""
    COMMENT "sandbox requires fftw3 to build"
  )
endif()

##
## TARGET : check-link- test linking to installed library
##

add_executable(liquid_linker_test EXCLUDE_FROM_ALL scripts/liquid_linker_test.c)
target_compile_options(liquid_linker_test PRIVATE -Wall -O2)
target_link_libraries(liquid_linker_test PRIVATE -lliquid)
if(HAVE_LIBC)
  target_link_libraries(liquid_linker_test PRIVATE c)
endif()
if(HAVE_LIBM)
  target_link_libraries(liquid_linker_test PRIVATE m)
endif()
if(fec_FOUND)
  target_link_libraries(liquid_linker_test PRIVATE fec::fec)
endif()
if(NOT LIQUID_FFTOVERRIDE AND FFTW_FOUND)
  target_link_libraries(liquid_linker_test PRIVATE FFTW::fftw3f)
endif()

target_include_directories(liquid_linker_test PRIVATE
  "${CMAKE_INSTALL_FULL_INCLUDEDIR}"
)
target_link_directories(liquid_linker_test PRIVATE
  "${CMAKE_INSTALL_FULL_LIBDIR}"
)

add_custom_target(check-link
  COMMAND liquid_linker_test
  DEPENDS liquid_linker_test
)

##
## TARGET : programs  - build all programs, but don't run anything
##

add_custom_target(programs)
add_dependencies(programs xautotest benchmark examples sandbox)

##
## TARGET : world     - build absolutely everything
##

add_custom_target(world)
add_dependencies(world bench check doc-check examples sandbox)

##
## TARGET : clean     - clean build (objects, dependencies, libraries, etc.)
##

# NOTE: Most cleanup is already handled by the global clean target

##
## TARGET : distclean - removes everything except originally distributed files
##

list(TRANSFORM framing_AUTOTEST_GLOB_BYPRODUCTS
  PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/"
  OUTPUT_VARIABLE autotest_FULL_GLOB_BYPRODUCTS
)

add_custom_target(distclean
  COMMAND ${CMAKE_COMMAND} --build "${CMAKE_CURRENT_BINARY_DIR}" --target clean
  COMMAND rm -f ${autotest_FULL_GLOB_BYPRODUCTS}
  COMMAND rm -f "${CMAKE_CURRENT_SOURCE_DIR}/config.h"
  COMMENT "cleaning distribution..."
)


##
## TARGET : list      - print list of targets
##

# NOTE: The target name "help" is reserved

set(echo_command ${CMAKE_COMMAND} -E echo)

# look for all occurrences of '## TARGET : ' and print rest of line to screen
file(STRINGS "${CMAKE_CURRENT_LIST_FILE}" main_targets REGEX "^## TARGET : ")
list(TRANSFORM main_targets REPLACE "^## TARGET : (.*)" "\\1")
list(TRANSFORM main_targets PREPEND
  "COMMAND;${echo_command};... "
  OUTPUT_VARIABLE echo_main_target_commands
)

add_custom_target(list
  COMMAND ${echo_command} "Targets for liquid-dsp:"
  ${echo_main_target_commands}
  VERBATIM
)
