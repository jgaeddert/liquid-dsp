# Copyright (c) 2007 - 2026 Joseph Gaeddert
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
# liquid-dsp library configure
# Process with autoconf to generate configure script
#

AC_INIT([liquid-dsp],[1.8.2],[joseph@liquidsdr.org])
AC_CONFIG_SRCDIR([src/libliquid.c])
AC_CONFIG_MACRO_DIR([scripts])

PROJECT_COPYRIGHT="Copyright (c) 2007 - 2026 Joseph D. Gaeddert <joseph@liquidsdr.org>"
PROJECT_LICENSE="MIT"
PROJECT_AUTHOR="Joseph D. Gaeddert <joseph@liquidsdr.org>"
PROJECT_HOMEPAGE="https://liquidsdr.org"
PROJECT_DESCRIPTION="Software-defined radio digital signal processing library"

# permit auxiliary scripts directory (e.g. config.sub, config.guess, install-sh)
AC_CONFIG_AUX_DIR(scripts/)

# Specify 'C' language
AC_LANG(C)

# uncomment the following line to reset "CFLAGS" variable; it defaults to '-g -O2'
#: ${CFLAGS=""}

# Autoheader
AH_TEMPLATE([LIQUID_FFTOVERRIDE],  [Force internal FFT even if libfftw is available])
AH_TEMPLATE([LIQUID_SIMDOVERRIDE], [Force overriding of SIMD (use portable C code)])
AH_TEMPLATE([LIQUID_SUPPRESS_ERROR_OUTPUT],[Suppress printing errors to stderr])
AH_TEMPLATE([LIQUID_STRICT_EXIT],  [Enable strict program exit on error])
AH_TEMPLATE([LIQUID_COLORS],       [Enable color terminal output])

AC_CONFIG_HEADERS([config.h])
AH_TOP([
#ifndef __LIQUID_CONFIG_H__
#define __LIQUID_CONFIG_H__
])
AH_BOTTOM([
#endif // __LIQUID_CONFIG_H__
])

# Configure options

AC_ARG_ENABLE(debug-messages,
    AS_HELP_STRING([--enable-debug-messages],[enable verbose debug messages (-DDEBUG)]),
    [DEBUG_MSG_OPTION="-DDEBUG"],
    [DEBUG_MSG_OPTION=""]
)

AC_ARG_ENABLE(simdoverride,
    AS_HELP_STRING([--enable-simdoverride],[use portable C code for dotprod, etc. even if SIMD extensions are available]),
    [AC_DEFINE(LIQUID_SIMDOVERRIDE)],
    [],
)

AC_ARG_ENABLE(fftoverride,
    AS_HELP_STRING([--enable-fftoverride],[use internal fft even if libfftw is available]),
    [AC_DEFINE(LIQUID_FFTOVERRIDE)],
    [],
)

# code coverage
AC_ARG_ENABLE(coverage,
    AS_HELP_STRING([--enable-coverage],[enable flags to test code coverage]),
    [   # check that gcovr program is installed
        AC_CHECK_PROG(HAVE_GCOVR,gcovr,yes)
        AS_IF([test x"$HAVE_GCOVR" != x"yes"],
              [AC_MSG_ERROR(Need gcovr with coverage option)],
              [COVERAGE_OPTION="--coverage"])],
    [COVERAGE_OPTION=""]
)

AC_ARG_ENABLE(suppress-errors,
    AS_HELP_STRING([--enable-suppress-errors],[suppress printing errors to stderr]),
    [AC_DEFINE(LIQUID_SUPPRESS_ERROR_OUTPUT)],
    [],
)

AC_ARG_ENABLE(strict,
    AS_HELP_STRING([--enable-strict],[use strict error handling at runtime]),
    [AC_DEFINE(LIQUID_STRICT_EXIT)],
    [],
)

AC_ARG_ENABLE(colors,
    AS_HELP_STRING([--disable-colors],[disable color terminal]),
    [],
    [AC_DEFINE(LIQUID_COLORS)],
)

# Check for necessary programs
AC_PROG_CC
AC_PROG_SED
AC_PROG_GREP
AC_PROG_INSTALL
AC_PROG_RANLIB

# Check for necessary libraries, library functions
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_FUNC_REALLOC

# AC_CHECK_LIB (library, function, [action-if-found], [action-if-not-found], [other-libraries])
AC_CHECK_LIB([c],[main],[],[AC_MSG_ERROR(Could not use standard C library)],   [])
AC_CHECK_LIB([m],[main],[],[AC_MSG_ERROR(Could not use standard math library)],[])

# AC_CHECK_FUNC(function, [action-if-found], [action-if-not-found])
AC_CHECK_FUNCS([malloc realloc free memset memmove],[],
               [AC_MSG_ERROR(Could not use memory allocation functions)])
AC_CHECK_FUNCS([sinf cosf expf cargf cexpf crealf cimagf sqrtf],[],
               [AC_MSG_ERROR(Could not use special math functions)],)

# Check for necessary header files
AC_CHECK_HEADERS([stdio.h stdlib.h complex.h string.h getopt.h sys/resource.h float.h inttypes.h limits.h stdlib.h string.h unistd.h],[],
                 [AC_MSG_ERROR(Could not use standard headers)])

# Check for optional header files, libraries, programs
AC_CHECK_HEADERS(fec.h fftw3.h)
AC_CHECK_LIB([fftw3f], [fftwf_plan_dft_1d], [],
             [AC_MSG_WARN(fftw3 library useful but not required)],
             [])
AC_CHECK_LIB([fec], [create_viterbi27], [],
             [AC_MSG_WARN(fec library useful but not required)],
             [])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Check size of certain variables
AC_CHECK_SIZEOF(short int)
AC_CHECK_SIZEOF(int)
AC_CHECK_SIZEOF(long int)
AC_CHECK_SIZEOF(long long int)

# AX_GCC_ARCHFLAG([PORTABLE?], [ACTION-SUCCESS], [ACTION-FAILURE])
# Try to guess the "native" architecture of the target to use with gcc's
# -march or -mtune flags. Default success action adds $ax_cv_gcc_archflag to
# $CFLAGS (sets to "unknown" on failure).
#AX_GCC_ARCHFLAG([no],[],[])

# get canonical target architecture
AC_CANONICAL_TARGET

# SIMD build flags (set to 0 by default, may be set to 1 by architecture detection)
BUILD_ALTIVEC="0"
BUILD_NEON="0"
BUILD_MMX="0"
BUILD_SSE="0"
BUILD_SSE2="0"
BUILD_SSE3="0"
BUILD_SSSE3="0"
BUILD_SSE41="0"
BUILD_SSE42="0"
BUILD_AVX="0"
BUILD_FMA3="0"
BUILD_AVX2="0"
BUILD_AVX512="0"
BUILD_AMX="0"
BUILD_AMX101="0"
BUILD_AMX102="0"

# default portable version
ARCH_OPTION=""

# override SIMD
if test "${enable_simdoverride+set}" = set; then
    # portable C version (all BUILD_* remain 0)
    ARCH_OPTION=""
else
    # Check canonical system
    case $target_cpu in
    powerpc*)
        BUILD_ALTIVEC="1"
        ARCH_OPTION="-fno-common -faltivec";;
    armv1*|armv2*|armv3*|armv4*|armv5*|armv6*)
        # assume neon instructions are NOT available
        ARCH_OPTION="";;
    arm|armv7*|armv8*|aarch64)
        # assume neon instructions are available
        BUILD_NEON="1"
        case $target_os in
        darwin*)
            # M1 mac, ARM architecture : use neon extensions
            ARCH_OPTION="-ffast-math";;
        *)
            ARCH_OPTION="";; # -ffast-math causes tests to fail on pi4 due to imprecision
        esac;;
    i386|i486|i586|i686|x86|x86_64)
        # check for MMX/SSE/AVX CPU extensions and intrinsics headers
        #   MMX     :   mmintrin.h
        #   SSE     :   xmmintrin.h
        #   SSE2    :   emmintrin.h
        #   SSE3    :   pmmintrin.h
        #   SSSE3   :   tmmintrin.h
        #   SSE4.1/2:   smmintrin.h
        #   AVX     :   immintrin.h
        #   AVX2    :   immintrin.h
        #   AVX512  :   immintrin.h
        AX_EXT

        # check for SSE4.1
        if [ test "$ax_cv_have_sse41_ext" = yes ]; then
            BUILD_SSE="1"
            BUILD_SSE2="1"
            BUILD_SSE3="1"
            BUILD_SSSE3="1"
            BUILD_SSE41="1"
            ARCH_OPTION='-msse4.1'
        fi;

        # check for AVX
        if [ test "$ax_cv_have_avx_ext" = yes ]; then
            BUILD_SSE42="1"
            BUILD_AVX="1"
            ARCH_OPTION='-mavx'
        fi;

        # check for AVX2
        if [ test "$ax_cv_have_avx2_ext" = yes ]; then
            BUILD_AVX2="1"
            ARCH_OPTION='-mavx2 -mfma -mf16c'
        fi;

        # check for AVX512
        if [ test "$ax_cv_have_avx512f_ext" = yes ]; then
            BUILD_AVX512="1"
            ARCH_OPTION='-mavx512f -mavx512dq -mavx512vl -mavx512bw -mfma'
        fi;;
    *)
        # unknown architecture : use portable C version
        ARCH_OPTION="";;
    esac
fi


case $target_os in
darwin*)
    AN_MAKEVAR([LIBTOOL], [AC_PROG_LIBTOOL])
    AN_PROGRAM([libtool], [AC_PROG_LIBTOOL])
    AC_DEFUN([AC_PROG_LIBTOOL], [AC_CHECK_TOOL(LIBTOOL, libtool, :)])
    AC_PROG_LIBTOOL

    AR_LIB=ar	    # archive extension
    SH_LIB=dylib    # shared library extension
    REBIND=""
    ;;
*)
    AN_MAKEVAR([AR], [AC_PROG_AR])
    AN_PROGRAM([ar], [AC_PROG_AR])
    AC_DEFUN([AC_PROG_AR], [AC_CHECK_TOOL(AR, ar, :)])
    AC_PROG_AR

    AR_LIB=a	    # archive extension
    SH_LIB=so	    # shared library extension
    REBIND=ldconfig
    ;;
esac

#
# autoconf variable substitutions
#
AC_SUBST(LIBS)                      # shared libraries (-lc, -lm, etc.)

AC_SUBST(AR_LIB)                    # archive library
AC_SUBST(SH_LIB)                    # output shared library target
AC_SUBST(REBIND)                    # rebinding tool (e.g. ldconfig)
AC_SUBST(ARCH_OPTION)               # compiler architecture option

AC_SUBST([LIBNAME], [liquid])        # library name for pkg-config

AC_SUBST(DEBUG_MSG_OPTION)          # debug messages option (.e.g -DDEBUG)
AC_SUBST(COVERAGE_OPTION)           # source code coverage option (e.g. -fprofile-arcs -ftest-coverage)
AC_SUBST(CLIB)                      # C library linkage (e.g. '-lc')

# gather metadata
# NOTE: many of these macros use a 'CMAKE' prefix so that a common build_info
#       template file may be used between cmake and legacy autotools
AC_SUBST([PROJECT_COPYRIGHT])
AC_SUBST([PROJECT_LICENSE])
AC_SUBST([PROJECT_AUTHOR])
AC_SUBST([PROJECT_HOMEPAGE])
AC_SUBST([PROJECT_DESCRIPTION])

# git hash
BUILD_GITHASH=$(git rev-parse --short=16 HEAD 2>/dev/null || echo "unknown")
AC_SUBST([BUILD_GITHASH])

# build_datetime
BUILD_DATETIME=$(date -u +"%Y-%m-%dT%H:%M:%SZ")
AC_SUBST([BUILD_DATETIME])

# build_hostname
BUILD_HOSTNAME=$(hostname)
AC_SUBST([BUILD_HOSTNAME])

# build_os
CMAKE_HOST_SYSTEM_NAME=$(uname -s)
AC_SUBST([CMAKE_HOST_SYSTEM_NAME])

# build_arch
CMAKE_HOST_SYSTEM_PROCESSOR=$(uname -m)
AC_SUBST([CMAKE_HOST_SYSTEM_PROCESSOR])

# build_toolchain
CMAKE_C_COMPILER_INFO=$($CC --version | head -n1)
AC_SUBST([CMAKE_C_COMPILER_INFO])

# build_type
CMAKE_BUILD_TYPE="Release"
AC_SUBST([CMAKE_BUILD_TYPE])

# build_env
BUILD_ENV="autotools"
AC_SUBST([BUILD_ENV])

# target_os
CMAKE_SYSTEM_NAME="${target_os}"
AC_SUBST([CMAKE_SYSTEM_NAME])

# target_arch
CMAKE_SYSTEM_PROCESSOR="${target_cpu}"
AC_SUBST([CMAKE_SYSTEM_PROCESSOR])

# logging enabled or not
ENABLE_LOGGING="1"
AC_SUBST([ENABLE_LOGGING])

# logging level threshold
LIQUID_LOG_LEVEL_COMPILE="0"
AC_SUBST([LIQUID_LOG_LEVEL_COMPILE])

# color enabled or not
ENABLE_COLOR="1"
AC_SUBST([ENABLE_COLOR])

# SIMD
AC_SUBST([BUILD_ALTIVEC])
AC_SUBST([BUILD_NEON])
AC_SUBST([BUILD_MMX])
AC_SUBST([BUILD_SSE])
AC_SUBST([BUILD_SSE2])
AC_SUBST([BUILD_SSE3])
AC_SUBST([BUILD_SSSE3])
AC_SUBST([BUILD_SSE41])
AC_SUBST([BUILD_SSE42])
AC_SUBST([BUILD_AVX])
AC_SUBST([BUILD_FMA3])
AC_SUBST([BUILD_AVX2])
AC_SUBST([BUILD_AVX512])
AC_SUBST([BUILD_AMX])
AC_SUBST([BUILD_AMX101])
AC_SUBST([BUILD_AMX102])


# pkg-config: map autotools variables to cmake-style placeholders in liquid-dsp.pc.in
PROJECT_VERSION="${PACKAGE_VERSION}"
AC_SUBST([PROJECT_VERSION])
CMAKE_INSTALL_PREFIX='${prefix}'
AC_SUBST([CMAKE_INSTALL_PREFIX])
libdir_for_pc_file='${prefix}/lib'
AC_SUBST([libdir_for_pc_file])
includedir_for_pc_file='${prefix}/include'
AC_SUBST([includedir_for_pc_file])

# build private libraries list for pkg-config
LIQUID_PC_LIBS_PRIVATE=""
if test "${ac_cv_lib_fftw3f_fftwf_plan_dft_1d}" = "yes" ; then
    LIQUID_PC_LIBS_PRIVATE="${LIQUID_PC_LIBS_PRIVATE} -lfftw3f"
fi
if test "${ac_cv_lib_fec_create_viterbi27}" = "yes" ; then
    LIQUID_PC_LIBS_PRIVATE="${LIQUID_PC_LIBS_PRIVATE} -lfec"
fi
LIQUID_PC_LIBS_PRIVATE="${LIQUID_PC_LIBS_PRIVATE} -lm"
AC_SUBST([LIQUID_PC_LIBS_PRIVATE])

AC_CONFIG_FILES([liquid.config.h:cmake/liquid.config.h.in])
AC_CONFIG_FILES([makefile])
AC_CONFIG_FILES([liquid-dsp.pc:cmake/liquid-dsp.pc.in])

AC_OUTPUT

