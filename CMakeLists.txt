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

# cmake debugging
#variable_watch(HAVE_SINF)
#set(CMAKE_VERBOSE_MAKEFILE ON)


# This is the CMake file used for generating build files
cmake_minimum_required(VERSION 3.9...3.19)
project(liquid C ASM)

# require a C99 compiler for all targets
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

#target_include_directories(test PRIVATE include)
set(INCLUDE include)
set(HEADER_FILES ${INCLUDE}/liquid.h ${INCLUDE}/liquid.internal.h)

set(LIBSRC src/libliquid.c)

# NOTE: if set to "liquid" it will unfortunately link to installed version... grumble...
set(LIBNAME liquidtmp)
add_library(${LIBNAME} SHARED ${LIBSRC} ${HEADER_FILES})
target_include_directories(${LIBNAME} PRIVATE ${INCLUDE})

add_executable(test_mylib examples/libliquid_example.c ${HEADER_FILES})
target_include_directories(test_mylib PRIVATE ${INCLUDE})

target_link_libraries(test_mylib ${LIBNAME})

# see: https://cmake.org/cmake/help/latest/command/install.html#examples
# TODO: install headers
# TODO: uninstall?
#install(TARGETS mylib)

# enable testing
enable_testing()
add_test(NAME test COMMAND test_mylib)


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
check_include_file      (assert.h         HAVE_ASSERT_H)
check_include_file      (complex.h        HAVE_COMPLEX_H)
check_include_file      (float.h          HAVE_FLOAT_H)
check_include_file      (getopt.h         HAVE_GETOPT_H)
check_include_file      (inttypes.h       HAVE_INTTYPES_H)
check_include_file      (limits.h         HAVE_LIMITS_H)
check_include_file      (math.h           HAVE_MATH_H)
check_include_file      (stdio.h          HAVE_STDIO_H)
check_include_file      (stdint.h         HAVE_STDINT_H)
check_include_file      (stdlib.h         HAVE_STDLIB_H)
check_include_file      (string.h         HAVE_STRING_H)
check_include_file      (sys/resource.h   HAVE_SYS_RESOURCE_H)
check_include_file      (time.h           HAVE_TIME_H)
check_include_file      (unistd.h         HAVE_UNISTD_H)

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

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

################################################################################
# Targets
#  
#  include_directories (${CMAKE_CURRENT_SOURCE_DIR})
#  include_directories (${CMAKE_CURRENT_BINARY_DIR})

