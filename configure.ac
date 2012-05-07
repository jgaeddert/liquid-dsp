## 
## Copyright (c) 2007, 2008, 2009, 2010, 2011 Joseph Gaeddert
## Copyright (c) 2007, 2008, 2009, 2010, 2011 Virginia Polytechnic
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

AC_INIT([liquid-dsp],[1.2.0],[liquid@vt.edu])
AC_CONFIG_SRCDIR([src/libliquid.c])

# permit auxiliary scripts directory (e.g. config.sub, config.guess, install-sh)
AC_CONFIG_AUX_DIR(scripts/)

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
        EXPERIMENTAL_HEADER="liquid.experimental.h" # add 'experimental' header to list
        EXPERIMENTAL_MLIBS="src/experimental/src/ann.nodes.o \
            src/experimental/src/activation_functions.o \
            src/experimental/src/cpmodem.o \
            src/experimental/src/kmeans.o \
            src/experimental/src/layer.o \
            src/experimental/src/maxnet.o \
            src/experimental/src/node.o \
            src/experimental/src/fbasc.o \
            src/experimental/src/gport.o \
            src/experimental/src/filter_rrrf.o \
            src/experimental/src/filter_crcf.o \
            src/experimental/src/filter_cccf.o \
            src/experimental/src/packetizer.o \
            src/experimental/src/patternset.o \
            src/experimental/src/awgn_channel.o \
            src/experimental/src/lognorm_channel.o \
            src/experimental/src/channel.o \
            src/experimental/src/pamodel.o \
            src/experimental/src/ofdmoqam.o \
            src/experimental/src/ofdmoqamframe.common.o \
            src/experimental/src/ofdmoqamframegen.o \
            src/experimental/src/ofdmoqamframesync.o \
            src/experimental/src/smatrix.o \
            src/experimental/src/mdctch.o \
            src/experimental/src/ricek_channel.o"
        EXPERIMENTAL_AUTOTESTS="src/experimental/tests/ann_autotest.c \
            src/experimental/tests/gport_autotest.c \
            src/experimental/tests/smatrix_autotest.c \
            src/experimental/tests/channel_autotest.c"
#           src/experimental/tests/ofdmoqam_autotest.c
        EXPERIMENTAL_BENCHMARKS="src/experimental/bench/fbasc_benchmark.c \
            src/experimental/bench/gport_dma_benchmark.c \
            src/experimental/bench/gport_dma_threaded_benchmark.c \
            src/experimental/bench/gport_ima_benchmark.c \
            src/experimental/bench/gport_ima_threaded_benchmark.c \
            src/experimental/bench/dds_benchmark.c \
            src/experimental/bench/ofdmoqam_benchmark.c \
            src/experimental/bench/qmfb_benchmark.c"
        EXPERIMENTAL_EXAMPLES="examples/ann_bitpattern_example \
            examples/ann_example \
            examples/ann_layer_example \
            examples/ann_maxnet_example \
            examples/ann_node_example \
            examples/ann_xor_example \
            examples/dds_cccf_example \
            examples/fbasc_example \
            examples/gport_dma_example \
            examples/gport_dma_threaded_example \
            examples/gport_ima_example \
            examples/gport_ima_threaded_example \
            examples/gport_mma_threaded_example \
            examples/iirqmfb_crcf_example \
            examples/itqmfb_crcf_example \
            examples/itqmfb_rrrf_example \
            examples/kmeans_example \
            examples/ofdmoqamframesync_example \
            examples/ofdmoqam_example \
            examples/ofdmoqam_firpfbch_example \
            examples/patternset_example \
            examples/prqmfb_crcf_example \
            examples/qmfb_crcf_analysis_example \
            examples/qmfb_crcf_synthesis_example \
            examples/ricek_channel_example \
            examples/smatrix_example \
            examples/symsync2_crcf_example \
            examples/symsynclp_crcf_example"
#           examples/ofdmoqamframe64gen_example
#           examples/ofdmoqamframe64sync_example
        AC_DEFINE(LIQUID_EXPERIMENTAL)
        AC_CHECK_HEADERS(pthread.h)
        AC_CHECK_LIB([pthread], [pthread_mutex_init], [],
                     [AC_MSG_ERROR(Need POSIX thread library for experimental build!)],
                     [])
    ],
    [   # experimental option disabled
        EXPERIMENTAL_HEADER=""
        EXPERIMENTAL_MLIBS=""
        EXPERIMENTAL_AUTOTESTS=""
        EXPERIMENTAL_BENCHMARKS=""
        EXPERIMENTAL_EXAMPLES=""
    ],
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
AC_CHECK_LIB([c], [malloc, realloc, free, memset, memmove], [],
             [AC_MSG_ERROR(Need standard c library!)],
             [])
AC_CHECK_LIB([m], [sinf, cosf, expf, cargf, cexpf, crealf, cimagf, sqrtf], [],
             [AC_MSG_ERROR(Need standard (complex) math library!)],
             [])

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
    # check for MMX/SSE/AVX CPU extensions and intrinsics headers
    #   MMX     :   mmintrin.h
    #   SSE     :   xmmintrin.h
    #   SSE2    :   emmintrin.h
    #   SSE3    :   pmmintrin.h
    #   SSSE3   :   tmmintrin.h
    #   SSE4.1/2:   smmintrin.h
    #   AVX     :   immintrin.h
    AX_EXT

    # need at least SSE2
    if [ test "$ax_cv_have_sse2_ext" = yes && test "$ac_cv_header_emmintrin_h" = yes ]; then
        MLIBS_DOTPROD="src/dotprod/src/dotprod_cccf.mmx.o \
                       src/dotprod/src/dotprod_crcf.mmx.o \
                       src/dotprod/src/dotprod_rrrf.mmx.o"
    else
        MLIBS_DOTPROD="src/dotprod/src/dotprod_cccf.o \
                       src/dotprod/src/dotprod_crcf.o \
                       src/dotprod/src/dotprod_rrrf.o"
    fi
    case $target_os in
    darwin*)
        ARCH_OPTION="-march=core2";;
    *)
        ARCH_OPTION="";;
    esac;;
powerpc*)
    MLIBS_DOTPROD="src/dotprod/src/dotprod_cccf.o \
                   src/dotprod/src/dotprod_rrrf.av.o \
                   src/dotprod/src/dotprod_crcf.av.o"
    ARCH_OPTION="-fno-common -faltivec";;
arm*)
    # ARM architecture : use portable C version
    MLIBS_DOTPROD="src/dotprod/src/dotprod_cccf.o \
                   src/dotprod/src/dotprod_crcf.o \
                   src/dotprod/src/dotprod_rrrf.o"
    ARCH_OPTION="-marm -mfloat-abi=softfp";;
*)
    # unknown architecture : use portable C version
    MLIBS_DOTPROD="src/dotprod/src/dotprod_cccf.o \
                   src/dotprod/src/dotprod_crcf.o \
                   src/dotprod/src/dotprod_rrrf.mmx.o"
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
AC_SUBST(LIBS)                      # shared libraries (-lc, -lm, etc.)
AC_SUBST(MLIBS_DOTPROD)             # 
AC_SUBST(EXPERIMENTAL_HEADER)       # experimental header
AC_SUBST(EXPERIMENTAL_MLIBS)        # experimental library objects
AC_SUBST(EXPERIMENTAL_AUTOTESTS)    # experimental autotests
AC_SUBST(EXPERIMENTAL_BENCHMARKS)   # experimental benchmarks
AC_SUBST(EXPERIMENTAL_EXAMPLES)     # experimental examples

AC_SUBST(SH_LIB)                    # output shared library target
AC_SUBST(REBIND)                    # rebinding tool (e.g. ldconfig)
AC_SUBST(ARCH_OPTION)               # compiler architecture option

AC_SUBST(DEBUG_OPTION)              # debug option
AC_SUBST(PROFILE_OPTION)            # profile symbols
AC_SUBST(CLIB)                      # C library linkage (e.g. '-lc')

# documentation programs
AC_SUBST(PDFLATEX)                  # pdflatex
AC_SUBST(BIBTEX)                    # bibtex
AC_SUBST(EPSTOPDF)                  # epstopdf
AC_SUBST(GNUPLOT)                   # gnuplot
AC_SUBST(PYGMENTIZE)                # pygmentize

AC_CONFIG_FILES([makefile doc/makefile])
AC_OUTPUT
