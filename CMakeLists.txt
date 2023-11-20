# Copyright (c) 2007 - 2023 Joseph Gaeddert
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
# CMake build file for liquid-dsp
#   $ mkdir build && cd build
#   $ cmake ..
#   $ make
#
# run autotest scripts
#   $ make check
#
# install
#   $ sudo make install
#

cmake_minimum_required(VERSION 3.10)
project(liquid VERSION 1.6.0 LANGUAGES C ASM)

# require a C99 compiler for all targets
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# cmake debugging
#variable_watch(HAVE_SINF)
#set(CMAKE_VERBOSE_MAKEFILE ON)

#cmake_host_system_information(RESULT info QUERY NUMBER_OF_LOGICAL_CORES)
#message(${info})

include (CheckIncludeFile)
include (CheckFunctionExists)
include (CheckLibraryExists)
include (CheckSymbolExists)

# global flags
add_definitions ("-Wall -fPIC -O3 -Wno-deprecated -Wno-deprecated-declarations")

# include files
# mmintrin.h
# xmmintrin.h
# emmintrin.h
# pmmintrin.h
# smmintrin.h
# arm_neon.h
# fftw3.h (bench/fftbench.c)
# configu.guess:
#   features.h
#   signal.h
#   stdarg.h
#   sys/stdarg.h
#   sys/param.h
#   sys/utsname.h
#   sys/types.h
#check_include_file      (assert.h         HAVE_ASSERT_H)
#check_include_file      (complex.h        HAVE_COMPLEX_H)
#check_include_file      (float.h          HAVE_FLOAT_H)
#check_include_file      (getopt.h         HAVE_GETOPT_H)
#check_include_file      (inttypes.h       HAVE_INTTYPES_H)
#check_include_file      (limits.h         HAVE_LIMITS_H)
#check_include_file      (math.h           HAVE_MATH_H)
#check_include_file      (stdio.h          HAVE_STDIO_H)
#check_include_file      (stdint.h         HAVE_STDINT_H)
#check_include_file      (stdlib.h         HAVE_STDLIB_H)
#check_include_file      (string.h         HAVE_STRING_H)
#check_include_file      (sys/resource.h   HAVE_SYS_RESOURCE_H)
#check_include_file      (time.h           HAVE_TIME_H)
#check_include_file      (unistd.h         HAVE_UNISTD_H)

# check for necessary functions/symbols
#check_symbol_exists     (malloc,    "stlib.h"   HAVE_MALLOC)
#check_symbol_exists     (realloc,   "stlib.h"   HAVE_REALLOC)
#check_symbol_exists     (free,      "stlib.h"   HAVE_FREE)
#check_symbol_exists     (memset,    "string.h"  HAVE_MEMSET)
#check_symbol_exists     (memmove,   "string.h"  HAVE_MEMMOVE)
#check_symbol_exists     (sinf,      "math.h"    HAVE_SINF)
#check_symbol_exists     (cosf,      "math.h"    HAVE_COSF)
#check_symbol_exists     (expf,      "math.h"    HAVE_EXPF)
#check_symbol_exists     (sqrtf      "math.h"    HAVE_SQRTF)
#check_symbol_exists     (cargf,     "complex.h" HAVE_CARGF)
#check_symbol_exists     (cexpf,     "complex.h" HAVE_CEXPF)
#check_symbol_exists     (crealf,    "complex.h" HAVE_CREALF)
#check_symbol_exists     (cimagf,    "complex.h" HAVE_CIMAGF)


include (CheckTypeSize)
check_type_size(int SIZEOF_INT LANGUAGE C)

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

################################################################################
# Targets
#  
#  include_directories (${CMAKE_CURRENT_SOURCE_DIR})
#  include_directories (${CMAKE_CURRENT_BINARY_DIR})

#target_include_directories(test PRIVATE include)
include_directories(include .)

# libraries

add_library(agc OBJECT
    src/agc/src/agc_crcf.c
    src/agc/src/agc_rrrf.c)

add_library(audio OBJECT
    src/audio/src/cvsd.c)

add_library(buffer OBJECT
    src/buffer/src/bufferf.c
    src/buffer/src/buffercf.c)

add_library(channel OBJECT
    src/channel/src/channel_cccf.c)

add_library(dotprod OBJECT
    src/dotprod/src/dotprod_cccf.c
    src/dotprod/src/dotprod_crcf.c
    src/dotprod/src/dotprod_rrrf.c
    src/dotprod/src/sumsq.c)

add_library(equalization OBJECT
    src/equalization/src/equalizer_cccf.c
    src/equalization/src/equalizer_rrrf.c)

add_library(fec OBJECT
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
    src/fec/src/sumproduct.c)

add_library(fft OBJECT
    src/fft/src/fftf.c
    src/fft/src/spgramcf.c
    src/fft/src/spgramf.c
    src/fft/src/fft_utilities.c)

add_library(filter OBJECT
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
    src/filter/src/rrcos.c)

add_library(framing OBJECT
    src/framing/src/bpacketgen.c
    src/framing/src/bpacketsync.c
    src/framing/src/detector_cccf.c
    src/framing/src/dsssframegen.c
    src/framing/src/dsssframesync.c
    src/framing/src/framedatastats.c
    src/framing/src/framesyncstats.c
    src/framing/src/framegen64.c
    src/framing/src/framesync64.c
    src/framing/src/framingcf.c
    src/framing/src/framing_rrrf.c
    src/framing/src/framing_crcf.c
    src/framing/src/framing_cccf.c
    src/framing/src/flexframegen.c
    src/framing/src/flexframesync.c
    src/framing/src/fskframegen.c
    src/framing/src/fskframesync.c
    src/framing/src/gmskframegen.c
    src/framing/src/gmskframesync.c
    src/framing/src/ofdmflexframegen.c
    src/framing/src/ofdmflexframesync.c
    src/framing/src/qpilotgen.c
    src/framing/src/qpilotsync.c)

add_library(math OBJECT
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
    src/math/src/windows.c)

add_library(matrix OBJECT
    src/matrix/src/matrix.c
    src/matrix/src/matrixf.c
    src/matrix/src/matrixc.c
    src/matrix/src/matrixcf.c
    src/matrix/src/smatrix.common.c
    src/matrix/src/smatrixb.c
    src/matrix/src/smatrixf.c
    src/matrix/src/smatrixi.c)

add_library(modem OBJECT
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
    src/modem/src/modem_arb_const.c)

add_library(multichannel OBJECT
    src/multichannel/src/firpfbch_crcf.c
    src/multichannel/src/firpfbch_cccf.c
    src/multichannel/src/ofdmframe.common.c
    src/multichannel/src/ofdmframegen.c
    src/multichannel/src/ofdmframesync.c)

add_library(nco OBJECT
    src/nco/src/nco_crcf.c
    src/nco/src/nco.utilities.c)

add_library(optim OBJECT
    src/optim/src/chromosome.c
    src/optim/src/gasearch.c
    src/optim/src/gradsearch.c
    src/optim/src/optim.common.c
    src/optim/src/qnsearch.c
    src/optim/src/qs1dsearch.c
    src/optim/src/utilities.c)

add_library(quantization OBJECT
    src/quantization/src/compand.c
    src/quantization/src/quantizercf.c
    src/quantization/src/quantizerf.c
    src/quantization/src/quantizer.inline.c)

add_library(random OBJECT
    src/random/src/rand.c
    src/random/src/randn.c
    src/random/src/randexp.c
    src/random/src/randweib.c
    src/random/src/randgamma.c
    src/random/src/randnakm.c
    src/random/src/randricek.c
    src/random/src/scramble.c)

add_library(sequence OBJECT
    src/sequence/src/bsequence.c
    src/sequence/src/msequence.c)

add_library(utility OBJECT
    src/utility/src/bshift_array.c
    src/utility/src/byte_utilities.c
    src/utility/src/memory.c
    src/utility/src/msb_index.c
    src/utility/src/pack_bytes.c
    src/utility/src/shift_array.c
    src/utility/src/utility.c)

add_library(vector OBJECT
    src/vector/src/vectorf_add.port.c
    src/vector/src/vectorf_norm.port.c
    src/vector/src/vectorf_mul.port.c
    src/vector/src/vectorf_trig.port.c
    src/vector/src/vectorcf_add.port.c
    src/vector/src/vectorcf_norm.port.c
    src/vector/src/vectorcf_mul.port.c
    src/vector/src/vectorcf_trig.port.c)



#
# main library
#

set(LIBSRC src/libliquid.c)

# NOTE: if set to "liquid" it will unfortunately link to installed version... grumble...
set(LIBNAME liquidtmp)
add_library(${LIBNAME} SHARED
    ${LIBSRC}
    $<TARGET_OBJECTS:agc>
    $<TARGET_OBJECTS:audio>
    $<TARGET_OBJECTS:buffer>
    $<TARGET_OBJECTS:channel>
    $<TARGET_OBJECTS:dotprod>
    $<TARGET_OBJECTS:equalization>
    $<TARGET_OBJECTS:fec>
    $<TARGET_OBJECTS:fft>
    $<TARGET_OBJECTS:filter>
    $<TARGET_OBJECTS:framing>
    $<TARGET_OBJECTS:math>
    $<TARGET_OBJECTS:matrix>
    $<TARGET_OBJECTS:modem>
    $<TARGET_OBJECTS:multichannel>
    $<TARGET_OBJECTS:nco>
    $<TARGET_OBJECTS:optim>
    $<TARGET_OBJECTS:quantization>
    $<TARGET_OBJECTS:random>
    $<TARGET_OBJECTS:sequence>
    $<TARGET_OBJECTS:utility>
    $<TARGET_OBJECTS:vector>)

target_include_directories(${LIBNAME} PRIVATE ${INCLUDE})

add_executable(test_libliquid examples/libliquid_example.c)
target_include_directories(test_libliquid PRIVATE ${INCLUDE})

target_link_libraries(test_libliquid ${LIBNAME})

# see: https://cmake.org/cmake/help/latest/command/install.html#examples
# TODO: install headers
# TODO: uninstall?
#install(TARGETS mylib)

#
# examples
#
#set_source_files_properties(DIRECTORY examples TARGET_DIRECTORY examples)

#
# autotest
#

set(AUTOTEST_SOURCES ${PROJECT_SOURCE_DIR}/src/agc/tests/agc_crcf_autotest.c)
add_executable(autoscript scripts/autoscript.c scripts/main.c scripts/autoscript.h)
add_custom_command(
    OUTPUT autotest_include.h
    DEPENDS ${PROJECT_BINARY_DIR}/autoscript
    COMMAND ${PROJECT_BINARY_DIR}/autoscript / autotest ${AUTOTEST_SOURCES} > autotest_include.h)

#add_custom_target(xautotest ALL
#    DEPENDS autotest_include.h)

#add_executable(xautotest autotest/xautotest.c autotest/autotest.h autotest_include.h)
#target_include_directories(xautotest PRIVATE ${INCLUDE})

#
# testing
#

# enable testing
enable_testing()
add_test(NAME test COMMAND test_libliquid)


# packaging
set(CPACK_PACKAGE_NAME liquid)
set(CPACK_PACKAGE_VENDOR liquidsdr)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Software-Defined Radio Digital Signal Processing Library")
set(CPACK_PACKAGE_INSTALL_DIRECTORY ${CPACK_PACKAGE_NAME})
set(CPACK_VERBATIM_VARIABLES TRUE)
include(CPack)

#message(${AGC_SRC})
#message("top: CMAKE_SOURCE_DIR         = ${CMAKE_SOURCE_DIR}")
#message("top: CMAKE_BINARY_DIR         = ${CMAKE_BINARY_DIR}")
#message("top: CMAKE_CURRENT_SOURCE_DIR = ${CMAKE_CURRENT_SOURCE_DIR}")
#message("top: CMAKE_CURRENT_BINARY_DIR = ${CMAKE_CURRENT_BINARY_DIR}")

# this file can only use the @VARIABLE@ syntax
configure_file(src/version.c.in ${PROJECT_BINARY_DIR}/src/version.c @ONLY)

