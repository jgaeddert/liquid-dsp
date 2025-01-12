INCLUDE(CheckCSourceRuns)
INCLUDE(CheckCSourceCompiles)
INCLUDE(CheckCXXSourceRuns)

SET(AVX_CODE "
  #include <immintrin.h>

  int main()
  {
    __m256 a;
    a = _mm256_set1_ps(0);
    return 0;
  }
")

SET(AVX512_CODE "
  #include <immintrin.h>
  int main()
  {
    float _v[4] = {1.0f, 2.0f, 3.0f, 4.0f};
    float _h[4] = {2.0f, 4.0f, 6.0f, 8.0f};
    __m512f v = _mm512_loadu_ps(_v);
    __m512f h = _mm512_loadu_ps(_h);
    __m512i s = _mm512_mul_ps(v, h);
    // unload packed array
    float w[4];
    _mm512_storeu_ps(w, s);
    return (w[0] == 2.0f && w[1] == 8.0f && w[2] == 18.0f && w[3] == 32.0f) ? 0 : -1;
  }
")

SET(AVX2_CODE "
  #include <immintrin.h>

  int main()
  {
    __m256i a = {0};
    a = _mm256_abs_epi16(a);
    __m256i x;
    _mm256_extract_epi64(x, 0); // we rely on this in our AVX2 code
    return 0;
  }
")

SET(NEON_CODE "
  #include <arm_neon.h>
  int main()
  {
    float _v[4] = {1.0f, 2.0f, 3.0f, 4.0f};
    float _h[4] = {2.0f, 4.0f, 6.0f, 8.0f};
    float32x4_t v = vld1q_f32(_v);
    float32x4_t h = vld1q_f32(_h);
    float32x4_t s = vmulq_f32(h,v);
       
    // unload packed array
    float w[4];
    vst1q_f32(w, s);
    return (w[0] == 2.0f && w[1] == 8.0f && w[2] == 18.0f && w[3] == 32.0f) ? 0 : -1;
  }
")

MACRO(CHECK_SIMD lang type flags)
  SET(__FLAG_I 1)
  SET(CMAKE_REQUIRED_FLAGS_SAVE ${CMAKE_REQUIRED_FLAGS})
  FOREACH(__FLAG ${flags})
    #message("testing ${lang} ${type} ${__FLAG}")
    IF(NOT ${lang}_${type}_FOUND)
      SET(CMAKE_REQUIRED_FLAGS ${__FLAG})
      # TODO: check that program runs and returns proper exit code 0
      IF(lang STREQUAL "CXX")
        CHECK_CXX_SOURCE_COMPILES("${${type}_CODE}" ${lang}_HAS_${type}_${__FLAG_I})
      ELSE()
        CHECK_C_SOURCE_COMPILES("${${type}_CODE}" ${lang}_HAS_${type}_${__FLAG_I})
      ENDIF()
      IF(${lang}_HAS_${type}_${__FLAG_I})
        SET(${lang}_${type}_FOUND TRUE CACHE BOOL "${lang} ${type} support")
        SET(${lang}_${type}_FLAGS "${__FLAG}" CACHE STRING "${lang} ${type} flags")
      ENDIF()
      MATH(EXPR __FLAG_I "${__FLAG_I}+1")
    ENDIF()
  ENDFOREACH()
  SET(CMAKE_REQUIRED_FLAGS ${CMAKE_REQUIRED_FLAGS_SAVE})

  IF(NOT ${lang}_${type}_FOUND)
    SET(${lang}_${type}_FOUND FALSE CACHE BOOL "${lang} ${type} support")
    SET(${lang}_${type}_FLAGS "" CACHE STRING "${lang} ${type} flags")
  ENDIF()

  SEPARATE_ARGUMENTS(${lang}_${type}_FLAGS)
  MARK_AS_ADVANCED(${lang}_${type}_FOUND ${lang}_${type}_FLAGS)

ENDMACRO()

# TODO: check msvc arch flags

CHECK_SIMD(C "AVX" " ;-mavx;/arch:AVX")
CHECK_SIMD(C "AVX2" " ;-mavx2 -mfma -mf16c;/arch:AVX2")
CHECK_SIMD(C "AVX512" " ;-mavx512f -mavx512dq -mavx512vl -mavx512bw -mfma;/arch:AVX512")
CHECK_SIMD(C "NEON" " ;-ffast-math;/arch:armv8.0;/arch:armv9.0")

CHECK_SIMD(CXX "AVX" " ;-mavx;/arch:AVX")
CHECK_SIMD(CXX "AVX2" " ;-mavx2 -mfma -mf16c;/arch:AVX2")
CHECK_SIMD(CXX "AVX512" " ;-mavx512f -mavx512dq -mavx512vl -mavx512bw -mfma;/arch:AVX512")
CHECK_SIMD(CXX "NEON" " ;-ffast-math;/arch:armv8.0;/arch:armv9.0")
