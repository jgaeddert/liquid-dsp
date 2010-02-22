dnl
dnl  Copyright (c) 2007, 2008, 2009, 2010 Joseph Gaeddert
dnl  Copyright (c) 2007, 2008, 2009, 2010 Virginia Polytechnic
dnl                                       Institute & State University
dnl
dnl  This file is part of liquid.
dnl
dnl  liquid is free software: you can redistribute it and/or modify
dnl  it under the terms of the GNU General Public License as published by
dnl  the Free Software Foundation, either version 3 of the License, or
dnl  (at your option) any later version.
dnl
dnl  liquid is distributed in the hope that it will be useful,
dnl  but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl  GNU General Public License for more details.
dnl
dnl  You should have received a copy of the GNU General Public License
dnl  along with liquid.  If not, see <http://www.gnu.org/licenses/>.

dnl 
dnl Liquid SDR library configure
dnl Process with autoconf to generate configure script
dnl

AC_PREREQ([2.65])
AC_INIT([liquid-dsp],[0.1.0],[liquid@vt.edu])
AC_CONFIG_SRCDIR([libliquid.c])

dnl Autoheader
AC_CONFIG_HEADER(config.h)
AH_TOP([
#ifndef __LIQUID_CONFIG_H__
#define __LIQUID_CONFIG_H__
])
AH_BOTTOM([
#endif // __LIQUID_CONFIG_H__
])

dnl Configure options

AC_ARG_ENABLE(debug,
    AS_HELP_STRING([--enable-debug],[debug]),
    [DEBUG_OPTION="-DDEBUG"],
    [DEBUG_OPTION=""]
)

AC_ARG_ENABLE(profile,
    AS_HELP_STRING([--enable-profile],[profile]),
    [PROFILE_OPTION="-pg"; CLIB="-lc_p"],
    [PROFILE_OPTION="";    CLIB="-lc"],
)

dnl Check for necessary programs
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_RANLIB

dnl Check for necessary libraries, library functions
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_FUNC_REALLOC
dnl AC_CHECK_LIB (library, function, [action-if-found], [action-if-not-found], [other-libraries])
AC_CHECK_LIB([c], [malloc, realloc, free, memset, memmove], [],
             [AC_MSG_ERROR(Need standard c library!)],
             [])
AC_CHECK_LIB([m], [sinf, cosf, expf, cargf, cexpf, crealf, cimagf, sqrtf], [],
             [AC_MSG_ERROR(Need standard math library!)],
             [])

dnl Check for necessary header files
AC_CHECK_HEADERS([stdio.h stdlib.h stdbool.h complex.h string.h getopt.h sys/resource.h float.h inttypes.h limits.h stdlib.h string.h unistd.h])
if test -z "$HAVE_stdio.h"
then
    AC_MSG_ERROR([Need stdio.h!])
fi

dnl Check for optional header files, libraries
AC_CHECK_HEADERS(pthread.h fec.h fftw3.h)
AC_CHECK_LIB([pthread], [pthread_mutex_init], [],
             [AC_MSG_WARN(Need POSIX thread library!)],
             [])
AC_CHECK_LIB([fftw3f], [fftwf_plan_dft_1d], [],
             [AC_MSG_WARN(fftw3 library useful but not required)],
             [])
AC_CHECK_LIB([fec], [create_viterbi27], [],
             [AC_MSG_WARN(fec library useful but not required)],
             [])
AC_CHECK_LIB([liquidfpm], [q32_mul], [],
             [AC_MSG_WARN(fixed-point math library useful but not required)],
             [])

dnl Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

dnl Check size of certain variables
AC_CHECK_SIZEOF(int)
AC_CHECK_SIZEOF(unsigned int)

dnl Check canonical system
AC_CANONICAL_TARGET
case $target_cpu in
i386|i486|i586|i686)
    MLIBS_DOTPROD="dotprod_cccf.c dotprod_crcf.c dotprod_rrrf.mmx.c"
    case $target_os in
    darwin*)
        ARCH_OPTION="-march=core2";;
    *)
        ARCH_OPTION="-march=$target_cpu";;
    esac;;
powerpc*)
    MLIBS_DOTPROD="dotprod_cccf.c dotprod_crcf.c dotprod_rrrf.av.c"
    ARCH_OPTION="-fno-common -faltivec";;
*)
esac

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

dnl
dnl autoconf variable substitutions
dnl
AC_SUBST(LIBS)              dnl shared libraries (-lc, -lm, etc.)
AC_SUBST(MLIBS_DOTPROD)     dnl 

AC_SUBST(SH_LIB)            dnl output shared library target
AC_SUBST(REBIND)            dnl rebinding tool (e.g. ldconfig)
AC_SUBST(ARCH_OPTION)       dnl compiler architecture option

AC_SUBST(DEBUG_OPTION)      dnl debug option
AC_SUBST(PROFILE_OPTION)    dnl profile symbols
AC_SUBST(CLIB)              dnl 

AC_CONFIG_FILES([makefile])
AC_OUTPUT
