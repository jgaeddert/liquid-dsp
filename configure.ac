## 
## Copyright (c) 2007 - 2014 Joseph Gaeddert
##
## This file is part of liquid.
##
## liquid is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## liquid is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with liquid.  If not, see <http://www.gnu.org/licenses/>.

# 
# liquid-dsp library configure
# Process with autoconf to generate configure script
#

AC_INIT([liquid-dsp],[1.2.0],[support@liquidsdr.org])
AC_CONFIG_SRCDIR([src/libliquid.c])

# permit auxiliary scripts directory (e.g. config.sub, config.guess, install-sh)
AC_CONFIG_AUX_DIR(scripts/)

# Specify 'C' language
AC_LANG(C)

# Autoheader
AH_TEMPLATE([LIQUID_FFTOVERRIDE],  [Force internal FFT even if libfftw is available])

AC_CONFIG_HEADER(config.h)
AH_TOP([
#ifndef __LIQUID_CONFIG_H__
#define __LIQUID_CONFIG_H__
])
AH_BOTTOM([
#endif // __LIQUID_CONFIG_H__
])

# Configure options

AC_ARG_ENABLE(debug,
    AS_HELP_STRING([--enable-debug],[debug]),
    [DEBUG_OPTION="-DDEBUG"],
    [DEBUG_OPTION=""]
)

AC_ARG_ENABLE(fftoverride,
    AS_HELP_STRING([--enable-fftoverride],[use internal fft even if libfftw is available]),
    [AC_DEFINE(LIQUID_FFTOVERRIDE)],
    [],
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
AC_CHECK_LIB([c],[main], [],[AC_MSG_ERROR(Could not use standard C library)],   [])
AC_CHECK_LIB([m],[main], [],[AC_MSG_ERROR(Could not use standard math library)],[])

# AC_CHECK_FUNC(function, [action-if-found], [action-if-not-found])
AC_CHECK_FUNC([malloc],  [],[AC_MSG_ERROR(Could not use malloc())])
AC_CHECK_FUNC([realloc], [],[AC_MSG_ERROR(Could not use realloc())],)
AC_CHECK_FUNC([free],    [],[AC_MSG_ERROR(Could not use free())],)
AC_CHECK_FUNC([memset],  [],[AC_MSG_ERROR(Could not use memset())],)
AC_CHECK_FUNC([memmove], [],[AC_MSG_ERROR(Could not use memove())],)

AC_CHECK_FUNC([sinf],    [],[AC_MSG_ERROR(Could not use sinf())],)
AC_CHECK_FUNC([cosf],    [],[AC_MSG_ERROR(Could not use cosf())],)
AC_CHECK_FUNC([expf],    [],[AC_MSG_ERROR(Could not use expf())],)
AC_CHECK_FUNC([cargf],   [],[AC_MSG_ERROR(Could not use cargf())],)
AC_CHECK_FUNC([cexpf],   [],[AC_MSG_ERROR(Could not use cexpf())],)
AC_CHECK_FUNC([crealf],  [],[AC_MSG_ERROR(Could not use crealf())],)
AC_CHECK_FUNC([cimagf],  [],[AC_MSG_ERROR(Could not use cimagf())],)
AC_CHECK_FUNC([sqrtf],   [],[AC_MSG_ERROR(Could not use sqrtf())],)

# Check for necessary header files
AC_CHECK_HEADERS([stdio.h stdlib.h complex.h string.h getopt.h sys/resource.h float.h inttypes.h limits.h stdlib.h string.h unistd.h])
if test -z "$HAVE_stdio.h"
then
    AC_MSG_ERROR([Need stdio.h!])
fi

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
AC_CHECK_SIZEOF(int)
AC_CHECK_SIZEOF(unsigned int)

# AX_GCC_ARCHFLAG([PORTABLE?], [ACTION-SUCCESS], [ACTION-FAILURE])
# Try to guess the "native" architecture of the target to use with gcc's
# -march or -mtune flags. Default success action adds $ax_cv_gcc_archflag to
# $CFLAGS (sets to "unknown" on failure).
AX_GCC_ARCHFLAG([no],[],[])

# Check canonical system
AC_CANONICAL_TARGET
case $target_cpu in
i386|i486|i586|i686|x86|x86_64)
    # check for MMX/SSE/AVX CPU extensions and intrinsics headers
    #   MMX     :   mmintrin.h
    #   SSE     :   xmmintrin.h
    #   SSE2    :   emmintrin.h
    #   SSE3    :   pmmintrin.h
    #   SSSE3   :   tmmintrin.h
    #   SSE4.1/2:   smmintrin.h
    #   AVX     :   immintrin.h
    AX_EXT

    if [ test "$ax_cv_have_sse41_ext" = yes && test "$ac_cv_header_smmintrin_h" = yes ]; then
        # SSE4.1/2 extensions
        MLIBS_DOTPROD="src/dotprod/src/dotprod_cccf.mmx.o \
                       src/dotprod/src/dotprod_crcf.mmx.o \
                       src/dotprod/src/dotprod_rrrf.mmx.o \
                       src/dotprod/src/sumsq.mmx.o"
    elif [ test "$ax_cv_have_sse2_ext" = yes && test "$ac_cv_header_emmintrin_h" = yes ]; then
        # SSE2 extensions
        MLIBS_DOTPROD="src/dotprod/src/dotprod_cccf.mmx.o \
                       src/dotprod/src/dotprod_crcf.mmx.o \
                       src/dotprod/src/dotprod_rrrf.mmx.o \
                       src/dotprod/src/sumsq.mmx.o"
    else
        # portable C version
        MLIBS_DOTPROD="src/dotprod/src/dotprod_cccf.o \
                       src/dotprod/src/dotprod_crcf.o \
                       src/dotprod/src/dotprod_rrrf.o \
                       src/dotprod/src/sumsq.o"
    fi;;
powerpc*)
    MLIBS_DOTPROD="src/dotprod/src/dotprod_cccf.o \
                   src/dotprod/src/dotprod_rrrf.av.o \
                   src/dotprod/src/dotprod_crcf.av.o \
                   src/dotprod/src/sumsq.o"
    ARCH_OPTION="-fno-common -faltivec";;
arm*)
    # TODO: check for Neon availability

    # ARM architecture : use neon extensions
    MLIBS_DOTPROD="src/dotprod/src/dotprod_cccf.neon.o \
                   src/dotprod/src/dotprod_crcf.neon.o \
                   src/dotprod/src/dotprod_rrrf.neon.o \
                   src/dotprod/src/sumsq.o"
    # TODO: check these flags
    ARCH_OPTION="-ffast-math -mcpu=cortex-a8 -mfloat-abi=softfp -mfpu=neon";;
*)
    # unknown architecture : use portable C version
    MLIBS_DOTPROD="src/dotprod/src/dotprod_cccf.o \
                   src/dotprod/src/dotprod_crcf.o \
                   src/dotprod/src/dotprod_rrrf.o \
                   src/dotprod/src/sumsq.o"
    ARCH_OPTION="";;
esac

# for now all vector operations are portable C versions
MLIBS_VECTOR="src/vector/src/vectorf_add.port.o   \
              src/vector/src/vectorf_norm.port.o  \
              src/vector/src/vectorf_mul.port.o   \
              src/vector/src/vectorf_trig.port.o  \
              src/vector/src/vectorcf_add.port.o  \
              src/vector/src/vectorcf_norm.port.o \
              src/vector/src/vectorcf_mul.port.o  \
              src/vector/src/vectorcf_trig.port.o"

case $target_os in
darwin*)
    SH_LIB=libliquid.dylib
    REBIND=""
    ;;
*)
    SH_LIB=libliquid.so
    REBIND=ldconfig
    ;;
esac

#
# autoconf variable substitutions
#
AC_SUBST(LIBS)                      # shared libraries (-lc, -lm, etc.)
AC_SUBST(MLIBS_DOTPROD)             # 
AC_SUBST(MLIBS_VECTOR)              #

AC_SUBST(SH_LIB)                    # output shared library target
AC_SUBST(REBIND)                    # rebinding tool (e.g. ldconfig)
AC_SUBST(ARCH_OPTION)               # compiler architecture option

AC_SUBST(DEBUG_OPTION)              # debug option
AC_SUBST(CLIB)                      # C library linkage (e.g. '-lc')

AC_CONFIG_FILES([makefile])
AC_OUTPUT
