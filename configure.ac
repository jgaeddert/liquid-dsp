## 
## Copyright (c) 2007, 2008, 2009, 2010 Joseph Gaeddert
## Copyright (c) 2007, 2008, 2009, 2010 Virginia Polytechnic
##                                      Institute & State University
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
# Liquid SDR library configure
# Process with autoconf to generate configure script
#

AC_INIT([liquid-dsp],[1.0.0],[liquid@vt.edu])
AC_CONFIG_SRCDIR([libliquid.c])

# Specify 'C' language
AC_LANG(C)

# Autoheader
AH_TEMPLATE([LIQUID_EXPERIMENTAL], [Enable building of experimental modules and objects.])

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

AC_ARG_ENABLE(profile,
    AS_HELP_STRING([--enable-profile],[profile]),
    [PROFILE_OPTION="-pg"; CLIB="-lc_p"],
    [PROFILE_OPTION="";    CLIB="-lc"],
)

# Option to enable/disable experimental modules and objects
AC_ARG_ENABLE(experimental,
    AS_HELP_STRING([--enable-experimental],[enable experimental modules, objects]),
    [   # experimental option enabled
        MODULE_EXPERIMENTAL="experimental"          # add 'experimental' module directory to list
        HEADER_EXPERIMENTAL="liquid.experimental.h" # add 'experimental' header to list
        AC_DEFINE(LIQUID_EXPERIMENTAL)
        AC_CHECK_HEADERS(pthread.h)
        AC_CHECK_LIB([pthread], [pthread_mutex_init], [],
                     [AC_MSG_ERROR(Need POSIX thread library for experimental build!)],
                     [])
    ],
    [   # experimental option disabled
        MODULE_EXPERIMENTAL=""
        HEADER_EXPERIMENTAL=""
    ],
)

# Check for necessary programs
AC_PROG_CC
AC_PROG_SED
AC_PROG_INSTALL
AC_PROG_RANLIB

# Check for necessary libraries, library functions
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_FUNC_REALLOC
# AC_CHECK_LIB (library, function, [action-if-found], [action-if-not-found], [other-libraries])
AC_CHECK_LIB([c], [malloc, realloc, free, memset, memmove], [],
             [AC_MSG_ERROR(Need standard c library!)],
             [])
AC_CHECK_LIB([m], [sinf, cosf, expf, cargf, cexpf, crealf, cimagf, sqrtf], [],
             [AC_MSG_ERROR(Need standard (complex) math library!)],
             [])

# Check for necessary header files
AC_CHECK_HEADERS([stdio.h stdlib.h stdbool.h complex.h string.h getopt.h sys/resource.h float.h inttypes.h limits.h stdlib.h string.h unistd.h])
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
#AC_CHECK_LIB([liquidfpm], [q32_mul], [],
#             [AC_MSG_WARN(fixed-point math library useful but not required)],
#             [])

# check for programs (documentation)
AC_CHECK_PROG(PDFLATEX, pdflatex, pdflatex, no)
if test "x$PDFLATEX" == "xno"; then
    AC_MSG_WARN(pdflatex required to build documentation)
fi
AC_CHECK_PROG(BIBTEX, bibtex, bibtex, no)
if test "x$BIBTEX" == "xno"; then
    AC_MSG_WARN(bibtex required to build documentation)
fi
AC_CHECK_PROG(EPSTOPDF, epstopdf, epstopdf, no)
if test "x$EPSTOPDF" == "xno"; then
    AC_MSG_WARN(epstopdf required to build documentation)
fi
AC_CHECK_PROG(GNUPLOT, gnuplot, gnuplot, no)
if test "x$GNUPLOT" == "xno"; then
    AC_MSG_WARN(gnuplot required to build documentation)
fi
AC_CHECK_PROG(PYGMENTIZE, pygmentize, pygmentize, no)
if test "x$PYGMENTIZE" == "xno"; then
    AC_MSG_WARN(pygmentize required to build documentation)
fi


# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Check size of certain variables
AC_CHECK_SIZEOF(int)
AC_CHECK_SIZEOF(unsigned int)

# Check canonical system
AC_CANONICAL_TARGET
case $target_cpu in
i386|i486|i586|i686|x86|x86_64)
    MLIBS_DOTPROD="dotprod_cccf.c dotprod_crcf.c dotprod_rrrf.mmx.c"
    case $target_os in
    darwin*)
        ARCH_OPTION="-march=core2";;
    *)
        ARCH_OPTION="";;
    esac;;
powerpc*)
    MLIBS_DOTPROD="dotprod_cccf.c dotprod_crcf.c dotprod_rrrf.av.c"
    ARCH_OPTION="-fno-common -faltivec";;
*)
    # unknown architecture : use portable C version
    MLIBS_DOTPROD="dotprod_cccf.c dotprod_crcf.c dotprod_rrrf.mmx.c"
    ARCH_OPTION="";;
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

#
# autoconf variable substitutions
#
AC_SUBST(LIBS)              # shared libraries (-lc, -lm, etc.)
AC_SUBST(MLIBS_DOTPROD)     # 
AC_SUBST(MODULE_EXPERIMENTAL)# build experimental module?
AC_SUBST(HEADER_EXPERIMENTAL)# experimental header

AC_SUBST(SH_LIB)            # output shared library target
AC_SUBST(REBIND)            # rebinding tool (e.g. ldconfig)
AC_SUBST(ARCH_OPTION)       # compiler architecture option

AC_SUBST(DEBUG_OPTION)      # debug option
AC_SUBST(PROFILE_OPTION)    # profile symbols
AC_SUBST(CLIB)              # 

# documentation programs
AC_SUBST(PDFLATEX)          # pdflatex
AC_SUBST(BIBTEX)            # bibtex
AC_SUBST(EPSTOPDF)          # epstopdf
AC_SUBST(GNUPLOT)           # gnuplot
AC_SUBST(PYGMENTIZE)        # pygmentize

AC_CONFIG_FILES([makefile doc/makefile])
AC_OUTPUT
