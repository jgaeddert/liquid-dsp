project(liquid-dsp C)

cmake_minimum_required(VERSION 3.0)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules")

file(READ "${CMAKE_CURRENT_SOURCE_DIR}/include/liquid.h" liquid_h)
string(REGEX MATCH "\\#define LIQUID_VERSION[ ]+\"([0-9]+\\.[0-9]+\\.[0-9]+)\"" LIQUID_VERSION_MATCHES "${liquid_h}")
if(NOT LIQUID_VERSION_MATCHES)
    message(FATAL_ERROR "Failed to extract version number from liquid.h")
endif(NOT LIQUID_VERSION_MATCHES)
set(LIQUID_VERSION ${CMAKE_MATCH_1})

message("-- Configuring for liquid-dsp ${LIQUID_VERSION}")

if (CMAKE_CROSSCOMPILING AND NOT HOST_C_COMPILER)
    message(FATAL_ERROR "Please, define HOST_C_COMPILER to the path of the host C compiler.")
endif ()

include(CheckCPUID)
include(CheckIncludeFile)
include(CheckLibraryExists)
include(CheckRequiredFunction)
include(CheckTypeSize)
include(LiquidBuildSamples)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)

option(LIQUID_FFTOVERRIDE "Force internal FFT even if libfftw is available" OFF)
option(LIQUID_SIMDOVERRIDE "Force overriding of SIMD (use portable C code)" OFF)
option(LIQUID_BUILD_SHARED "Build shared library" ON)
option(LIQUID_BUILD_STATIC "Build static library" ON)
option(LIQUID_BUILD_EXAMPLES "Build examples" ON)
option(LIQUID_BUILD_SANDBOX "Build sandbox" ON)
option(LIQUID_FORCE_X86_MMX "Force MMX (useful for cross-compiling)" OFF)
option(LIQUID_FORCE_X86_SSE "Force SSE (useful for cross-compiling)" OFF)
option(LIQUID_FORCE_X86_SSE2 "Force SSE2 (useful for cross-compiling)" OFF)
option(LIQUID_FORCE_X86_SSE3 "Force SSE3 (useful for cross-compiling)" OFF)
option(LIQUID_FORCE_X86_SSSE3 "Force SSSE3 (useful for cross-compiling)" OFF)
option(LIQUID_FORCE_X86_SSE41 "Force SSE4.1 (useful for cross-compiling)" OFF)
option(LIQUID_FORCE_X86_SSE42 "Force SSE4.2 (useful for cross-compiling)" OFF)
option(LIQUID_FORCE_X86_AVX "Force AVX (useful for cross-compiling)" OFF)
option(LIQUID_FORCE_PPC_ALTIVEC "Force AltiVec (useful for cross-compiling)" OFF)
option(LIQUID_FORCE_ARM_NEON "Force NEON (useful for cross-compiling)" OFF)
option(LIQUID_FORCE_ARM64_NEON "Force AArch64 NEON (useful for cross-compiling)" OFF)

CHECK_INCLUDE_FILE(sys/resource.h HAVE_SYS_RESOURCE_H)

CHECK_INCLUDE_FILE(complex.h HAVE_COMPLEX_H)
CHECK_INCLUDE_FILE(float.h HAVE_FLOAT_H)
CHECK_INCLUDE_FILE(getopt.h HAVE_GETOPT_H)
CHECK_INCLUDE_FILE(inttypes.h HAVE_INTTYPES_H)
CHECK_INCLUDE_FILE(memory.h HAVE_MEMORY_H)
CHECK_INCLUDE_FILE(unistd.h HAVE_UNISTD_H)

CHECK_INCLUDE_FILE(mmintrin.h HAVE_MMINTRIN_H)
CHECK_INCLUDE_FILE(emmintrin.h HAVE_EMMINTRIN_H)
CHECK_INCLUDE_FILE(immintrin.h HAVE_IMMINTRIN_H)
CHECK_INCLUDE_FILE(pmmintrin.h HAVE_PMMINTRIN_H)
CHECK_INCLUDE_FILE(smmintrin.h HAVE_SMMINTRIN_H)
CHECK_INCLUDE_FILE(tmmintrin.h HAVE_TMMINTRIN_H)
CHECK_INCLUDE_FILE(xmmintrin.h HAVE_XMMINTRIN_H)

CHECK_INCLUDE_FILE(fec.h HAVE_FEC_H)
CHECK_INCLUDE_FILE(fftw3.h HAVE_FFTW3_H)

CHECK_TYPE_SIZE(int SIZEOF_INT)
CHECK_TYPE_SIZE("unsigned int" SIZEOF_UNSIGNED_INT)

CHECK_REQUIRED_FUNCTION(sinf m HAVE_SINF)
CHECK_REQUIRED_FUNCTION(cosf m HAVE_COSF)
CHECK_REQUIRED_FUNCTION(expf m HAVE_EXPF)
CHECK_REQUIRED_FUNCTION(cargf m HAVE_CARGF)
CHECK_REQUIRED_FUNCTION(cexpf m HAVE_CEXPF)
CHECK_REQUIRED_FUNCTION(crealf m HAVE_CREALF)
CHECK_REQUIRED_FUNCTION(cimagf m HAVE_CIMAGF)
CHECK_REQUIRED_FUNCTION(sqrtf m HAVE_SQRTF)

CHECK_LIBRARY_EXISTS(m sqrtf "" HAVE_LIBM)
if (HAVE_FEC_H)
    CHECK_LIBRARY_EXISTS(fec create_viterbi27 "" HAVE_LIBFEC)
    if (NOT HAVE_LIBFEC)
        unset(HAVE_FEC_H CACHE)
    endif ()
endif ()
if (HAVE_FFTW3_H)
    CHECK_LIBRARY_EXISTS(fftw3f fftwf_plan_dft_1d "" HAVE_LIBFFTW3F)
    if (NOT HAVE_LIBFFTW3F)
        unset(HAVE_FFTW3_H CACHE)
    endif ()
endif ()

CHECK_CPUID("mmx" HAVE_MMX ${LIQUID_FORCE_X86_MMX})
CHECK_CPUID("sse" HAVE_SSE ${LIQUID_FORCE_X86_SSE})
CHECK_CPUID("sse2" HAVE_SSE2 ${LIQUID_FORCE_X86_SSE2})
CHECK_CPUID("sse3" HAVE_SSE3 ${LIQUID_FORCE_X86_SSE3})
CHECK_CPUID("ssse3" HAVE_SSSE3 ${LIQUID_FORCE_X86_SSSE3})
CHECK_CPUID("sse41" HAVE_SSE41 ${LIQUID_FORCE_X86_SSE41})
CHECK_CPUID("sse42" HAVE_SSE42 ${LIQUID_FORCE_X86_SSE42})
CHECK_CPUID("avx" HAVE_AVX ${LIQUID_FORCE_X86_AVX})
CHECK_CPUID("vmx" HAVE_ALTIVEC ${LIQUID_FORCE_PPC_ALTIVEC})
CHECK_CPUID("neon" HAVE_NEON ${LIQUID_FORCE_ARM_NEON})
CHECK_CPUID("neon64" HAVE_NEON64 ${LIQUID_FORCE_ARM64_NEON})

#
# Build compilation extra flags
#
if (CMAKE_C_COMPILER_ID MATCHES "GNU" OR CMAKE_C_COMPILER_ID MATCHES "Clang")
    if (HAVE_AVX)
        set(_EXTRA_C_FLAGS "-mavx")
    elseif (HAVE_SSE42)
        set(_EXTRA_C_FLAGS "-msse4.2")
    elseif (HAVE_SSE41)
        set(_EXTRA_C_FLAGS "-msse4.1")
    elseif (HAVE_SSSE3)
        set(_EXTRA_C_FLAGS "-mssse3")
    elseif (HAVE_SSE3)
        set(_EXTRA_C_FLAGS "-msse3")
    elseif (HAVE_SSE2)
        set(_EXTRA_C_FLAGS "-msse2")
    elseif (HAVE_SSE)
        set(_EXTRA_C_FLAGS "-msse")
    elseif (HAVE_MMX)
        set(_EXTRA_C_FLAGS "-mmmx")
    elseif (HAVE_ALTIVEC)
        if (CMAKE_SYSTEM_NAME MATCHES "Darwin")
            set(_EXTRA_C_FLAGS "-fno-common -faltivec")
        else ()
            set(_EXTRA_C_FLAGS "-maltivec")
        endif ()
    elseif (HAVE_NEON)
        set(_EXTRA_C_FLAGS "-mfpu=neon-vfpv4")
    elseif (HAVE_NEON64)
        # No extra flags needed
    endif ()
    set(_EXTRA_C_FLAGS "${_EXTRA_C_FLAGS} -ffast-math")
endif ()

#
# MODULE : agc - automatic gain control
#

set(agc_SOURCES
    src/agc/src/agc_crcf.c
    src/agc/src/agc_rrrf.c
    )

set(agc_AUTOTESTS_SOURCES
    src/agc/tests/agc_crcf_autotest.c
    )

set(agc_BENCHMARKS_SOURCES
    src/agc/bench/agc_crcf_benchmark.c
    )

#
# MODULE : audio
#

set(audio_SOURCES
    src/audio/src/cvsd.c
    )

set(audio_AUTOTESTS_SOURCES
    src/audio/tests/cvsd_autotest.c
    )

set(audio_BENCHMARKS_SOURCES
    src/audio/bench/cvsd_benchmark.c
    )

#
# MODULE : buffer
#

set(buffer_SOURCES
    src/buffer/src/bufferf.c
    src/buffer/src/buffercf.c
    )

set(buffer_AUTOTESTS_SOURCES
    src/buffer/tests/cbuffer_autotest.c
    src/buffer/tests/wdelay_autotest.c
    src/buffer/tests/window_autotest.c
    #src/buffer/tests/sbuffer_autotest.c
    )

set(buffer_BENCHMARKS_SOURCES
    src/buffer/bench/cbuffercf_benchmark.c
    src/buffer/bench/window_push_benchmark.c
    src/buffer/bench/window_read_benchmark.c
    )

#
# MODULE : channel
#

set(channel_SOURCES
    src/channel/src/channel_cccf.c
    )

set(channel_AUTOTESTS_SOURCES
    )

set(channel_BENCHMARKS_SOURCES
    )

#
# MODULE : dotprod
#

set(dotprod_C_SOURCES
    src/dotprod/src/dotprod_cccf.c
    src/dotprod/src/dotprod_crcf.c
    src/dotprod/src/dotprod_rrrf.c
    src/dotprod/src/sumsq.c
    )

# PowerPC AltiVec
set(dotprod_ALTIVEC_SOURCES
    src/dotprod/src/dotprod_cccf.c
    src/dotprod/src/dotprod_crcf.av.c
    src/dotprod/src/dotprod_rrrf.av.c
    src/dotprod/src/sumsq.c
    )

# Intel MMX/SSE/AVX
set(dotprod_SSE_SOURCES
    src/dotprod/src/dotprod_cccf.mmx.c
    src/dotprod/src/dotprod_crcf.mmx.c
    src/dotprod/src/dotprod_rrrf.mmx.c
    src/dotprod/src/sumsq.mmx.c
    )

# ARM NEON
set(dotprod_NEON_SOURCES
    src/dotprod/src/dotprod_cccf.neon.c
    src/dotprod/src/dotprod_crcf.neon.c
    src/dotprod/src/dotprod_rrrf.neon.c
    src/dotprod/src/sumsq.c
    )

set(dotprod_AUTOTESTS_SOURCES
    src/dotprod/tests/dotprod_rrrf_autotest.c
    src/dotprod/tests/dotprod_crcf_autotest.c
    src/dotprod/tests/dotprod_cccf_autotest.c
    src/dotprod/tests/sumsqf_autotest.c
    src/dotprod/tests/sumsqcf_autotest.c
    )

set(dotprod_BENCHMARKS_SOURCES
    src/dotprod/bench/dotprod_cccf_benchmark.c
    src/dotprod/bench/dotprod_crcf_benchmark.c
    src/dotprod/bench/dotprod_rrrf_benchmark.c
    src/dotprod/bench/sumsqf_benchmark.c
    src/dotprod/bench/sumsqcf_benchmark.c
    )

if (LIQUID_SIMDOVERRIDE)
    set(dotprod_SOURCES ${dotprod_C_SOURCES})
elseif (HAVE_SSE41 AND HAVE_SMMINTRIN_H)
    set(dotprod_SOURCES ${dotprod_SSE_SOURCES})
elseif (HAVE_SSE3 AND HAVE_PMMINTRIN_H)
    set(dotprod_SOURCES ${dotprod_SSE_SOURCES})
elseif (HAVE_SSE2 AND HAVE_EMMINTRIN_H)
    unset(HAVE_PMMINTRIN_H CACHE) # Unset otherwise SSE3 code kicks in.
    set(dotprod_SOURCES ${dotprod_SSE_SOURCES})
elseif (HAVE_ALTIVEC)
    set(dotprod_SOURCES ${dotprod_ALTIVEC_SOURCES})
elseif (HAVE_NEON OR HAVE_NEON64)
    set(dotprod_SOURCES ${dotprod_NEON_SOURCES})
else ()
    set(dotprod_SOURCES ${dotprod_C_SOURCES})
endif ()

#
# MODULE : equalization
#

set(equalization_SOURCES
    src/equalization/src/equalizer_cccf.c
    src/equalization/src/equalizer_rrrf.c
    )

set(equalization_AUTOTESTS_SOURCES
    src/equalization/tests/eqlms_cccf_autotest.c
    src/equalization/tests/eqrls_rrrf_autotest.c
    )

set(equalization_BENCHMARKS_SOURCES
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

set(fec_AUTOTESTS_SOURCES
    src/fec/tests/crc_autotest.c
    src/fec/tests/fec_autotest.c
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
    src/fec/tests/packetizer_autotest.c
    )

set(fec_BENCHMARKS_SOURCES
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

set(fft_AUTOTESTS_SOURCES
    src/fft/tests/fft_small_autotest.c
    src/fft/tests/fft_radix2_autotest.c
    src/fft/tests/fft_composite_autotest.c
    src/fft/tests/fft_prime_autotest.c
    src/fft/tests/fft_r2r_autotest.c
    src/fft/tests/fft_shift_autotest.c
    )

set(autotest_EXTRA_SOURCES ${autotest_EXTRA_SOURCES}
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

set(fft_BENCHMARKS_SOURCES
    src/fft/bench/fft_composite_benchmark.c
    src/fft/bench/fft_prime_benchmark.c
    src/fft/bench/fft_radix2_benchmark.c
    src/fft/bench/fft_r2r_benchmark.c
    )

set(benchmark_EXTRA_SOURCES ${benchmark_EXTRA_SOURCES}
    src/fft/bench/fft_runbench.c
    )

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

set(filter_AUTOTESTS_SOURCES
	src/filter/tests/fftfilt_xxxf_autotest.c
	src/filter/tests/filter_crosscorr_autotest.c
	src/filter/tests/firdecim_xxxf_autotest.c
	src/filter/tests/firdes_autotest.c
	src/filter/tests/firdespm_autotest.c
	src/filter/tests/firfilt_xxxf_autotest.c
	src/filter/tests/firhilb_autotest.c
	src/filter/tests/firinterp_autotest.c
	src/filter/tests/firpfb_autotest.c
	src/filter/tests/groupdelay_autotest.c
	src/filter/tests/iirdes_autotest.c
	src/filter/tests/iirfilt_xxxf_autotest.c
	src/filter/tests/iirfiltsos_rrrf_autotest.c
	src/filter/tests/msresamp_crcf_autotest.c
	src/filter/tests/resamp_crcf_autotest.c
	src/filter/tests/resamp2_crcf_autotest.c
	src/filter/tests/symsync_crcf_autotest.c
	src/filter/tests/symsync_rrrf_autotest.c
    )

set(autotest_EXTRA_SOURCES ${autotest_EXTRA_SOURCES}
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

set(filter_BENCHMARKS_SOURCES
    src/filter/bench/fftfilt_crcf_benchmark.c
    src/filter/bench/firdecim_crcf_benchmark.c
    src/filter/bench/firhilb_benchmark.c
    src/filter/bench/firinterp_crcf_benchmark.c
    src/filter/bench/firfilt_crcf_benchmark.c
    src/filter/bench/iirdecim_crcf_benchmark.c
    src/filter/bench/iirfilt_crcf_benchmark.c
    src/filter/bench/iirinterp_crcf_benchmark.c
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
    src/framing/src/framedatastats.c
    src/framing/src/framesyncstats.c
    src/framing/src/framegen64.c
    src/framing/src/framesync64.c
    src/framing/src/flexframegen.c
    src/framing/src/flexframesync.c
    src/framing/src/gmskframegen.c
    src/framing/src/gmskframesync.c
    src/framing/src/msourcecf.c
    src/framing/src/ofdmflexframegen.c
    src/framing/src/ofdmflexframesync.c
    src/framing/src/presync_cccf.c
    src/framing/src/symstreamcf.c
    src/framing/src/symtrack_cccf.c
    src/framing/src/qdetector_cccf.c
    src/framing/src/qpacketmodem.c
    src/framing/src/qpilotgen.c
    src/framing/src/qpilotsync.c
    )

set(framing_AUTOTESTS_SOURCES
    src/framing/tests/bpacketsync_autotest.c
    src/framing/tests/bsync_autotest.c
    src/framing/tests/detector_autotest.c
    src/framing/tests/flexframesync_autotest.c
    src/framing/tests/framesync64_autotest.c
    src/framing/tests/qdetector_cccf_autotest.c
    src/framing/tests/qpacketmodem_autotest.c
    src/framing/tests/qpilotsync_autotest.c
    )

set(framing_BENCHMARKS_SOURCES
    src/framing/bench/presync_benchmark.c
    src/framing/bench/bpacketsync_benchmark.c
    src/framing/bench/bpresync_benchmark.c
    src/framing/bench/bsync_benchmark.c
    src/framing/bench/detector_benchmark.c
    src/framing/bench/flexframesync_benchmark.c
    src/framing/bench/framesync64_benchmark.c
    src/framing/bench/gmskframesync_benchmark.c
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
    )

set(math_AUTOTESTS_SOURCES
    src/math/tests/kbd_autotest.c
    src/math/tests/math_autotest.c
    src/math/tests/math_bessel_autotest.c
    src/math/tests/math_gamma_autotest.c
    src/math/tests/math_complex_autotest.c
    src/math/tests/polynomial_autotest.c
    )

set(math_BENCHMARKS_SOURCES
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

set(matrix_AUTOTESTS_SOURCES
    src/matrix/tests/matrixcf_autotest.c
    src/matrix/tests/matrixf_autotest.c
    src/matrix/tests/smatrixb_autotest.c
    src/matrix/tests/smatrixf_autotest.c
    src/matrix/tests/smatrixi_autotest.c
    )

set(autotest_EXTRA_SOURCES ${autotest_EXTRA_SOURCES}
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

set(matrix_BENCHMARKS_SOURCES
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
    src/modem/src/modemf.c
    src/modem/src/modem_utilities.c
    src/modem/src/modem_apsk_const.c
    src/modem/src/modem_arb_const.c
    )

set(modem_AUTOTESTS_SOURCES
    src/modem/tests/cpfskmodem_autotest.c
    src/modem/tests/freqmodem_autotest.c
    src/modem/tests/fskmodem_autotest.c
    src/modem/tests/modem_autotest.c
    src/modem/tests/modem_demodsoft_autotest.c
    src/modem/tests/modem_demodstats_autotest.c
    )

set(modem_BENCHMARKS_SOURCES
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

set(multichannel_AUTOTESTS_SOURCES
    src/multichannel/tests/firpfbch2_crcf_autotest.c
    src/multichannel/tests/firpfbch_crcf_synthesizer_autotest.c
    src/multichannel/tests/firpfbch_crcf_analyzer_autotest.c
    src/multichannel/tests/ofdmframesync_autotest.c
    )

set(multichannel_BENCHMARKS_SOURCES
    src/multichannel/bench/firpfbch_crcf_benchmark.c
    src/multichannel/bench/firpfbch2_crcf_benchmark.c
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

set(nco_AUTOTESTS_SOURCES
    src/nco/tests/nco_crcf_frequency_autotest.c
    src/nco/tests/nco_crcf_phase_autotest.c
    src/nco/tests/nco_crcf_pll_autotest.c
    src/nco/tests/unwrap_phase_autotest.c
    )

set(autotest_EXTRA_SOURCES ${autotest_EXTRA_SOURCES}
    src/nco/tests/data/nco_sincos_fsqrt1_2.c
    src/nco/tests/data/nco_sincos_fsqrt1_3.c
    src/nco/tests/data/nco_sincos_fsqrt1_5.c
    src/nco/tests/data/nco_sincos_fsqrt1_7.c
    )

set(nco_BENCHMARKS_SOURCES
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
    src/optim/src/utilities.c
    )

set(optim_AUTOTESTS_SOURCES
    src/optim/tests/gradsearch_autotest.c
    )

set(optim_BENCHMARKS_SOURCES )

#
# MODULE : quantization
#
set(quantization_SOURCES
    src/quantization/src/compand.c
    src/quantization/src/quantizercf.c
    src/quantization/src/quantizerf.c
    src/quantization/src/quantizer.inline.c
    )

set(quantization_AUTOTESTS_SOURCES
    src/quantization/tests/compand_autotest.c
    src/quantization/tests/quantize_autotest.c
    )

set(quantization_BENCHMARKS_SOURCES
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

set(random_AUTOTESTS_SOURCES
    src/random/tests/scramble_autotest.c
    #src/random/tests/random_autotest.c
    )

set(random_BENCHMARKS_SOURCES
    src/random/bench/random_benchmark.c
    )

#
# MODULE : sequence
#
set(sequence_SOURCES
    src/sequence/src/bsequence.c
    src/sequence/src/msequence.c
    )

set(sequence_AUTOTESTS_SOURCES
    src/sequence/tests/bsequence_autotest.c
    src/sequence/tests/complementary_codes_autotest.c
    src/sequence/tests/msequence_autotest.c
    )

set(sequence_BENCHMARKS_SOURCES
    src/sequence/bench/bsequence_benchmark.c
    )

#
# MODULE : utility
#
set(utility_SOURCES
    src/utility/src/bshift_array.c
    src/utility/src/byte_utilities.c
    src/utility/src/msb_index.c
    src/utility/src/pack_bytes.c
    src/utility/src/shift_array.c
    )

set(utility_AUTOTESTS_SOURCES
    src/utility/tests/bshift_array_autotest.c
    src/utility/tests/count_bits_autotest.c
    src/utility/tests/pack_bytes_autotest.c
    src/utility/tests/shift_array_autotest.c
    )

set(utility_BENCHMARKS_SOURCES )

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

set(vector_AUTOTESTS_SOURCES )

set(vector_BENCHMARKS_SOURCES )

#
# Generate config.h file.
#
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/include/config.h)

#
# Library
#
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

if (NOT LIQUID_BUILD_SHARED AND NOT LIQUID_BUILD_STATIC)
    message(FATAL_ERROR "Please, define LIQUID_BUILD_SHARED and/or LIQUID_BUILD_STATIC.")
endif ()

if (LIQUID_BUILD_SHARED)
    add_library(liquid-shared SHARED ${liquid_SOURCES})
    if (HAVE_LIBM)
        target_link_libraries(liquid-shared m)
    endif ()
    if (HAVE_LIBFEC)
        target_link_libraries(liquid-shared fec)
    endif ()
    if (NOT LIQUID_FFTOVERRIDE AND HAVE_LIBFFTW3F)
        target_link_libraries(liquid-shared fftw3f)
    endif ()
    set_property(TARGET liquid-shared PROPERTY OUTPUT_NAME "liquid")
    set_property(TARGET liquid-shared PROPERTY SOVERSION "${LIQUID_VERSION}")
    set_property(TARGET liquid-shared PROPERTY COMPILE_FLAGS "${_EXTRA_C_FLAGS}")
    set(LIQUID_LIBRARY liquid-shared)
    set(LIQUID_SHARED_LIBRARY liquid-shared)
endif ()

if (LIQUID_BUILD_STATIC)
    add_library(liquid-static STATIC ${liquid_SOURCES})
    if (HAVE_LIBM)
        target_link_libraries(liquid-static m)
    endif ()
    if (HAVE_LIBFEC)
        target_link_libraries(liquid-static fec)
    endif ()
    if (NOT LIQUID_FFTOVERRIDE AND HAVE_LIBFFTW3F)
        target_link_libraries(liquid-static fftw3f)
    endif ()
    set_property(TARGET liquid-static PROPERTY OUTPUT_NAME "liquid")
    set_property(TARGET liquid-static PROPERTY COMPILE_FLAGS "${_EXTRA_C_FLAGS}")

    if (NOT LIQUID_LIBRARY)
        set(LIQUID_LIBRARY liquid-static)
    endif ()
    set(LIQUID_STATIC_LIBRARY liquid-static)
endif ()

#
# Build the autoscript tool, needed by autotest and benchmark.
#
if (CMAKE_CROSSCOMPILING)
    #
    # Cross-compile the tool with the host compiler, the CMake Toolchain file shall
    # define this variable to point to the host compiler.
    #
    add_custom_target(autoscript
                      COMMAND ${HOST_C_COMPILER} -o autoscript "${CMAKE_CURRENT_SOURCE_DIR}/scripts/autoscript.c" "${CMAKE_CURRENT_SOURCE_DIR}/scripts/main.c")
else ()
    add_executable(autoscript scripts/autoscript.c scripts/main.c)
endif ()

#
# Autotest
#
set(autotest_ALL_SOURCES
    ${agc_AUTOTESTS_SOURCES}
    ${audio_AUTOTESTS_SOURCES}
    ${buffer_AUTOTESTS_SOURCES}
    ${channel_AUTOTESTS_SOURCES}
    ${dotprod_AUTOTESTS_SOURCES}
    ${equalization_AUTOTESTS_SOURCES}
    ${fec_AUTOTESTS_SOURCES}
    ${fft_AUTOTESTS_SOURCES}
    ${filter_AUTOTESTS_SOURCES}
    ${framing_AUTOTESTS_SOURCES}
    ${math_AUTOTESTS_SOURCES}
    ${matrix_AUTOTESTS_SOURCES}
    ${modem_AUTOTESTS_SOURCES}
    ${multichannel_AUTOTESTS_SOURCES}
    ${nco_AUTOTESTS_SOURCES}
    ${optim_AUTOTESTS_SOURCES}
    ${quantization_AUTOTESTS_SOURCES}
    ${random_AUTOTESTS_SOURCES}
    ${sequence_AUTOTESTS_SOURCES}
    ${utility_AUTOTESTS_SOURCES}
    ${vector_AUTOTESTS_SOURCES}
    )

set(autotest_SOURCES
    autotest/autotest.c
    autotest/autotestlib.c
    ${autotest_ALL_SOURCES}
    ${autotest_EXTRA_SOURCES}
    )

add_custom_target(autotest_include
                  COMMAND "${CMAKE_CURRENT_BINARY_DIR}/autoscript" "/" "autotest" ${autotest_ALL_SOURCES} > "${CMAKE_CURRENT_BINARY_DIR}/autotest_include.h"
                  DEPENDS autoscript
                  BYPRODUCTS "${CMAKE_CURRENT_BINARY_DIR}/autotest_include.h"
                  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                  )

add_executable(autotest ${autotest_SOURCES})
add_dependencies(autotest autotest_include)
target_link_libraries(autotest ${LIQUID_LIBRARY})

#
# Benchmark
#
# Benchmark can be built only if sys/resource.h has been found.
if (HAVE_SYS_RESOURCE_H)
    set(benchmark_ALL_SOURCES
        ${agc_BENCHMARKS_SOURCES}
        ${audio_BENCHMARKS_SOURCES}
        ${buffer_BENCHMARKS_SOURCES}
        ${channel_BENCHMARKS_SOURCES}
        ${dotprod_BENCHMARKS_SOURCES}
        ${equalization_BENCHMARKS_SOURCES}
        ${fec_BENCHMARKS_SOURCES}
        ${fft_BENCHMARKS_SOURCES}
        ${filter_BENCHMARKS_SOURCES}
        ${framing_BENCHMARKS_SOURCES}
        ${math_BENCHMARKS_SOURCES}
        ${matrix_BENCHMARKS_SOURCES}
        ${modem_BENCHMARKS_SOURCES}
        ${multichannel_BENCHMARKS_SOURCES}
        ${nco_BENCHMARKS_SOURCES}
        ${optim_BENCHMARKS_SOURCES}
        ${quantization_BENCHMARKS_SOURCES}
        ${random_BENCHMARKS_SOURCES}
        ${sequence_BENCHMARKS_SOURCES}
        ${utility_BENCHMARKS_SOURCES}
        ${vector_BENCHMARKS_SOURCES}
        )

    set(benchmark_SOURCES
        bench/bench.c
        ${benchmark_ALL_SOURCES}
        ${benchmark_EXTRA_SOURCES}
        )

    add_custom_target(benchmark_include
                      COMMAND "${CMAKE_CURRENT_BINARY_DIR}/autoscript" "/" "benchmark" ${benchmark_ALL_SOURCES} > "${CMAKE_CURRENT_BINARY_DIR}/benchmark_include.h"
                      DEPENDS autoscript
                      BYPRODUCTS "${CMAKE_CURRENT_BINARY_DIR}/benchmark_include.h"
                      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                      )

    add_executable(benchmark ${benchmark_SOURCES})
    add_dependencies(benchmark benchmark_include)
    target_link_libraries(benchmark ${LIQUID_LIBRARY})
else ()
    message(WARNING "Benchmark suite cannot be compiled because sys/resource.h header hasn't been found.")
endif ()

#
# Examples
#
if (LIQUID_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif ()

#
# Sandbox
#
if (LIQUID_BUILD_SANDBOX)
    add_subdirectory(sandbox)
endif ()

#
# Installation
#
install(TARGETS ${LIQUID_STATIC_LIBRARY} ${LIQUID_SHARED_LIBRARY}
        RUNTIME DESTINATION bin
        ARCHIVE DESTINATION lib${LIB_SUFFIX}
        LIBRARY DESTINATION lib${LIB_SUFFIX})
install(FILES include/liquid.h
        DESTINATION include/liquid)
