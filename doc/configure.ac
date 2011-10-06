## 
## Copyright (c) 2007-2010 Joseph Gaeddert
## Copyright (c) 2007-2010 Virginia Polytechnic Institute &
##                             State University
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
# liquid documentation configure
# Process with autoconf to generate configure script
#

AC_PREREQ(2.61)
AC_INIT([liquid-doc],[0.1.0],[liquid@vt.edu])
AC_CONFIG_SRCDIR([liquid.tex])

##
## Configure options
##

# (none)

##
## Check for necessary programs
##
AC_PROG_CC

# check for pdflatex
AC_CHECK_PROG(HAVE_PDFLATEX, pdflatex, yes, no)
if test "x$HAVE_PDFLATEX" = "xyes"; then
    PDFLATEX=pdflatex
else
    AC_MSG_ERROR(Need pdflatex)
fi

# check for bibtex
AC_CHECK_PROG(HAVE_BIBTEX, bibtex, yes)
if test "x$HAVE_BIBTEX" = "xyes"; then
    BIBTEX=bibtex
else
    AC_MSG_ERROR(Need bibtex)
fi

# check for epstopdf
AC_CHECK_PROG(HAVE_EPSTOPDF, epstopdf, yes)
if test "x$HAVE_EPSTOPDF" = "xyes"; then
    EPSTOPDF=epstopdf
else
    AC_MSG_ERROR(Need epstopdf)
fi

# check for gnuplot
AC_CHECK_PROG(HAVE_GNUPLOT, gnuplot, yes)
if test "x$HAVE_GNUPLOT" = "xyes"; then
    GNUPLOT=gnuplot
else
    AC_MSG_ERROR(Need gnuplot)
fi

# check for pygmentize
AC_CHECK_PROG(HAVE_PYGMENTIZE, pygmentize, yes)
if test "x$HAVE_PYGMENTIZE" = "xyes"; then
    PYGMENTIZE=pygmentize
else
    AC_MSG_ERROR(Need pygmentize)
fi

##
## Check for necessary header files
##

# AC_CHECK_HEADERS (header-files, [action-if-found], [action-if-not-found], [includes])
AC_CHECK_HEADERS([stdio.h stdlib.h complex.h string.h getopt.h],
                 [],
                 [AC_MSG_ERROR(missing header files)],
                 [])

##
## Check for necessary libraries, library functions
##
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_C_CONST
AC_C_INLINE
# AC_CHECK_LIB (library, function, [action-if-found], [action-if-not-found], [other-libraries])
AC_CHECK_LIB([c], [malloc, realloc, free, memset, memmove, strcspn, strstr], [],
             [AC_MSG_ERROR(Need standard c library!)],
             [])
AC_CHECK_LIB([m], [sinf, cosf, expf, cargf, cexpf, crealf, cimagf, sqrtf], [],
             [AC_MSG_ERROR(Need standard math library!)],
             [])

##
## Check for optional header files, libraries
##
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

##
## Checks for typedefs, structures, and compiler characteristics.
##
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

##
## Check size of certain variables
##
AC_CHECK_SIZEOF(int)
AC_CHECK_SIZEOF(unsigned int)

##
## Check canonical system
## 

##
## autoconf variable substitutions
##
AC_SUBST(LIBS)              # shared libraries (-lc, -lm, etc.)
AC_SUBST(PDFLATEX)          # pdflatex
AC_SUBST(BIBTEX)            # bibtex
AC_SUBST(EPSTOPDF)          # epstopdf
AC_SUBST(GNUPLOT)           # gnuplot
AC_SUBST(PYGMENTIZE)        # pygmentize

##
## export output
##
AC_CONFIG_FILES([makefile])
AC_OUTPUT
