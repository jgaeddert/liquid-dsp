# ===========================================================================
#          http://www.gnu.org/software/autoconf-archive/ax_ext.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_EXT
#
# DESCRIPTION
#
#   Find supported SIMD extensions by requesting cpuid. When an SIMD
#   extension is found, the -m"simdextensionname" is added to SIMD_FLAGS
#   (only if compilator support it) (ie : if "sse2" is available "-msse2" is
#   added to SIMD_FLAGS)
#
#   This macro calls:
#
#     AC_SUBST(SIMD_FLAGS)
#
#   And defines:
#
#     HAVE_MMX / HAVE_SSE / HAVE_SSE2 / HAVE_SSE3 / HAVE_SSSE3 /
#     HAVE_SSE41 / HAVE_SSE42 / HAVE_AVX
#
# LICENSE
#
#   Copyright (c) 2008 Christophe Tournayre <turn3r@users.sourceforge.net>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 9

AC_DEFUN([AX_EXT],
[
  AC_REQUIRE([AX_GCC_X86_CPUID])

  AX_GCC_X86_CPUID(0x00000001)
  ecx=`echo $ax_cv_gcc_x86_cpuid_0x00000001 | cut -d ":" -f 3`
  edx=`echo $ax_cv_gcc_x86_cpuid_0x00000001 | cut -d ":" -f 4`

 AC_CACHE_CHECK([whether mmx is supported], [ax_cv_have_mmx_ext],
  [
    ax_cv_have_mmx_ext=no
    if test "$((0x$edx>>23&0x01))" = 1; then
      ax_cv_have_mmx_ext=yes
    fi
  ])

 AC_CACHE_CHECK([whether sse is supported], [ax_cv_have_sse_ext],
  [
    ax_cv_have_sse_ext=no
    if test "$((0x$edx>>25&0x01))" = 1; then
      ax_cv_have_sse_ext=yes
    fi
  ])

 AC_CACHE_CHECK([whether sse2 is supported], [ax_cv_have_sse2_ext],
  [
    ax_cv_have_sse2_ext=no
    if test "$((0x$edx>>26&0x01))" = 1; then
      ax_cv_have_sse2_ext=yes
    fi
  ])

 AC_CACHE_CHECK([whether sse3 is supported], [ax_cv_have_sse3_ext],
  [
    ax_cv_have_sse3_ext=no
    if test "$((0x$ecx&0x01))" = 1; then
      ax_cv_have_sse3_ext=yes
    fi
  ])

 AC_CACHE_CHECK([whether ssse3 is supported], [ax_cv_have_ssse3_ext],
  [
    ax_cv_have_ssse3_ext=no
    if test "$((0x$ecx>>9&0x01))" = 1; then
      ax_cv_have_ssse3_ext=yes
    fi
  ])

 AC_CACHE_CHECK([whether sse4.1 is supported], [ax_cv_have_sse41_ext],
  [
    ax_cv_have_sse41_ext=no
    if test "$((0x$ecx>>19&0x01))" = 1; then
      ax_cv_have_sse41_ext=yes
    fi
  ])

 AC_CACHE_CHECK([whether sse4.2 is supported], [ax_cv_have_sse42_ext],
  [
    ax_cv_have_sse42_ext=no
    if test "$((0x$ecx>>20&0x01))" = 1; then
      ax_cv_have_sse42_ext=yes
    fi
  ])

 AC_CACHE_CHECK([whether avx is supported], [ax_cv_have_avx_ext],
  [
    ax_cv_have_avx_ext=no
    if test "$((0x$ecx>>28&0x01))" = 1; then
      ax_cv_have_avx_ext=yes
    fi
  ])

  if test "$ax_cv_have_mmx_ext" = yes; then
    AC_DEFINE(HAVE_MMX,, [Support MMX instructions])
    AX_CHECK_COMPILE_FLAG(-mmmx, SIMD_FLAGS="$SIMD_FLAGS -mmmx", [])
  fi

  if test "$ax_cv_have_sse_ext" = yes; then
    AC_DEFINE(HAVE_SSE,, [Support SSE (Streaming SIMD Extensions) instructions])
    AX_CHECK_COMPILE_FLAG(-msse, SIMD_FLAGS="$SIMD_FLAGS -msse", [])
  fi

  if test "$ax_cv_have_sse2_ext" = yes; then
    AC_DEFINE(HAVE_SSE2,, [Support SSE2 (Streaming SIMD Extensions 2) instructions])
    AX_CHECK_COMPILE_FLAG(-msse2, SIMD_FLAGS="$SIMD_FLAGS -msse2", [])
  fi

  if test "$ax_cv_have_sse3_ext" = yes; then
    AC_DEFINE(HAVE_SSE3,, [Support SSE3 (Streaming SIMD Extensions 3) instructions])
    AX_CHECK_COMPILE_FLAG(-msse3, SIMD_FLAGS="$SIMD_FLAGS -msse3", [])
  fi

  if test "$ax_cv_have_ssse3_ext" = yes; then
    AC_DEFINE(HAVE_SSSE3,, [Support SSSE3 (Supplemental Streaming SIMD Extensions 3) instructions])
    AX_CHECK_COMPILE_FLAG(-mssse3, SIMD_FLAGS="$SIMD_FLAGS -mssse3", [])
  fi

  if test "$ax_cv_have_sse41_ext" = yes; then
    AC_DEFINE(HAVE_SSE41,, [Support SSE4.1 (Streaming SIMD Extensions 4.1) instructions])
    AX_CHECK_COMPILE_FLAG(-msse4.1, SIMD_FLAGS="$SIMD_FLAGS -msse4.1", [])
  fi

  if test "$ax_cv_have_sse42_ext" = yes; then
    AC_DEFINE(HAVE_SSE42, 1,[Support SSE4.2 (Streaming SIMD Extensions 4.2) instructions])
    AX_CHECK_COMPILE_FLAG(-msse4.2, SIMD_FLAGS="$SIMD_FLAGS -msse4.2", [])
  fi

  if test "$ax_cv_have_avx_ext" = yes; then
    AC_DEFINE(HAVE_AVX,,[Support AVX (Advanced Vector Extensions) instructions])
    AX_CHECK_COMPILE_FLAG(-mavx, SIMD_FLAGS="$SIMD_FLAGS -mavx", [])
  fi

  AC_SUBST(SIMD_FLAGS)
])
