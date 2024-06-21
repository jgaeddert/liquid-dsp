#
# This collects all of the liquid-dsp source files for each module 
#

# MODULE - agc
set(agc_SOURCES
  src/agc/src/agc_crcf.c
  src/agc/src/agc_rrrf.c
)

# MODULE - audio
set(audio_SOURCES src/audio/src/cvsd.c)

# MODULE - buffer
set(buffer_SOURCES
  src/buffer/src/bufferf.c
  src/buffer/src/buffercf.c
)

# MODULE - channel
set(channel_SOURCES src/channel/src/channel_cccf.c)

# MODULE - dotprod
# AVX512 (x86_64)
set(dotprod_AVX512_SOURCES
  src/dotprod/src/dotprod_cccf.avx512f.c
  src/dotprod/src/dotprod_crcf.avavx512f.c
  src/dotprod/src/dotprod_rrrf.avavx512f.c
  src/dotprod/src/sumsq.avavx512f.c
)
# AVX (x86_64)
set(dotprod_AVX_SOURCES
  src/dotprod/src/dotprod_cccf.avx.c
  src/dotprod/src/dotprod_crcf.avx.c
  src/dotprod/src/dotprod_rrrf.avx.c
  src/dotprod/src/sumsq.avx.c
)
# SSE (x86_64)
set(dotprod_SSE_SOURCES
  src/dotprod/src/dotprod_cccf.sse.c
  src/dotprod/src/dotprod_crcf.sse.c
  src/dotprod/src/dotprod_rrrf.sse.c
  src/dotprod/src/sumsq.sse.c
)
# NEON (ARM)
set(dotprod_NEON_SOURCES
  src/dotprod/src/dotprod_cccf.neon.c
  src/dotprod/src/dotprod_crcf.neon.c
  src/dotprod/src/dotprod_rrrf.neon.c
  src/dotprod/src/sumsq.c
)
# AltiVec (PowerPC)
set(dotprod_ALTIVEC_SOURCES
  src/dotprod/src/dotprod_cccf.c
  src/dotprod/src/dotprod_crcf.av.c
  src/dotprod/src/dotprod_rrrf.av.c
  src/dotprod/src/sumsq.c
)
# Portable (Any architecture)
set(dotprod_C_SOURCES
  src/dotprod/src/dotprod_cccf.c
  src/dotprod/src/dotprod_crcf.c
  src/dotprod/src/dotprod_rrrf.c
  src/dotprod/src/sumsq.c
)

# If SIMD override is enabled, use portable C dotprod sources, otherwise try to use SIMD-optimized dotprod sources
if(LIQUID_SIMDOVERRIDE)
    set(dotprod_SOURCES ${dotprod_C_SOURCES})
    message("LIQUID_SIMDOVERRIDE is enabled. Using portable C instead of SIMD optimizations.")
else()
    if(HAVE_AVX512 AND HAVE_IMMINTRIN_H)
        set(dotprod_SOURCES ${dotprod_AVX512_SOURCES})
        message("Using AVX512 optimizations.")
    elseif(HAVE_AVX AND HAVE_IMMINTRIN_H)
        set(dotprod_SOURCES ${dotprod_AVX_SOURCES})
        message("Using AVX optimizations.")
    elseif(HAVE_SSE AND HAVE_XMMINTRIN_H)
        set(dotprod_SOURCES ${dotprod_SSE_SOURCES})
        message("Using SSE optimizations.")
    elseif(HAVE_NEON AND HAVE_ARM_NEON_H)
        set(dotprod_SOURCES ${dotprod_NEON_SOURCES})
        message("Using NEON optimizations.")
    elseif(HAVE_ALTIVEC)
        set(dotprod_SOURCES ${dotprod_ALTIVEC_SOURCES})
        message("Using ALTIVEC optimizations.")
    else()
        set(dotprod_SOURCES ${dotprod_C_SOURCES})
        message("No SIMD optimizations available, using portable.")
    endif()
endif()

# MODULE - equalization
set(equalization_SOURCES
  src/equalization/src/equalizer_cccf.c
  src/equalization/src/equalizer_rrrf.c
)

# MODULE - fec
set(fec_SOURCES
  src/fec/src/c_ones_mod2.c
  src/fec/src/crc.c
  src/fec/src/fec.c
  src/fec/src/fec_conv.c
  src/fec/src/fec_conv_poly.c
  src/fec/src/fec_conv_pmatrix.c
  src/fec/src/fec_conv_punctured.c
  src/fec/src/fec_golay2412.c
  src/fec/src/fec_hamming74.c
  src/fec/src/fec_hamming84.c
  src/fec/src/fec_hamming128_gentab.c
  src/fec/src/fec_hamming128.c
  src/fec/src/fec_hamming1511.c
  src/fec/src/fec_hamming3126.c
  src/fec/src/fec_pass.c
  src/fec/src/fec_rep3.c
  src/fec/src/fec_rep5.c
  src/fec/src/fec_rs.c
  src/fec/src/fec_secded2216.c
  src/fec/src/fec_secded3932.c
  src/fec/src/fec_secded7264.c
  src/fec/src/interleaver.c
  src/fec/src/packetizer.c
  src/fec/src/sumproduct.c
)

# MODULE - fft
set(fft_SOURCES
  src/fft/src/fft_utilities.c
  src/fft/src/fftf.c
  src/fft/src/spgramcf.c
  src/fft/src/spgramf.c
)

# MODULE - filter
set(filter_SOURCES
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
  src/filter/src/rrcos.c
)

# MODULE - framing
set(framing_SOURCES
  src/framing/src/bpacketgen.c
  src/framing/src/bpacketsync.c
  src/framing/src/detector_cccf.c
  src/framing/src/dsssframegen.c
  src/framing/src/dsssframesync.c
  src/framing/src/flexframegen.c
  src/framing/src/flexframesync.c
  src/framing/src/framedatastats.c
  src/framing/src/framegen64.c
  src/framing/src/framesync64.c
  src/framing/src/framesyncstats.c
  src/framing/src/framing_cccf.c
  src/framing/src/framing_crcf.c
  src/framing/src/framing_rrrf.c
  src/framing/src/framingcf.c
  src/framing/src/fskframegen.c
  src/framing/src/fskframesync.c
  src/framing/src/gmskframegen.c
  src/framing/src/gmskframesync.c
  src/framing/src/ofdmflexframegen.c
  src/framing/src/ofdmflexframesync.c
  src/framing/src/qpilotgen.c
  src/framing/src/qpilotsync.c
)

# MODULE - math
set(math_SOURCES
    src/math/src/math.c
    src/math/src/math.bessel.c
    src/math/src/math.complex.c
    src/math/src/math.gamma.c
    src/math/src/math.trig.c
    src/math/src/modular_arithmetic.c
    src/math/src/poly.c
    src/math/src/poly.findroots.c
    src/math/src/polyc.c
    src/math/src/polycf.c
    src/math/src/polyf.c
    src/math/src/windows.c
)

# MODULE - matrix
set(matrix_SOURCES
    src/matrix/src/matrix.c
    src/matrix/src/matrixc.c
    src/matrix/src/matrixcf.c
    src/matrix/src/matrixf.c
    src/matrix/src/smatrix.common.c
    src/matrix/src/smatrixb.c
    src/matrix/src/smatrixf.c
    src/matrix/src/smatrixi.c
)

# MODULE - modem
set(modem_SOURCES
    src/modem/src/ampmodem.c
    src/modem/src/cpfskdem.c
    src/modem/src/cpfskmod.c
    src/modem/src/fskdem.c
    src/modem/src/fskmod.c
    src/modem/src/gmskdem.c
    src/modem/src/gmskmod.c
    src/modem/src/modem_apsk_const.c
    src/modem/src/modem_arb_const.c
    src/modem/src/modem_utilities.c
    src/modem/src/modem.shim.c
    src/modem/src/modemcf.c
)

# MODULE - multichannel
set(multichannel_SOURCES
    src/multichannel/src/firpfbch_cccf.c
    src/multichannel/src/firpfbch_crcf.c
    src/multichannel/src/ofdmframe.common.c
    src/multichannel/src/ofdmframegen.c
    src/multichannel/src/ofdmframesync.c
)

# MODULE - nco
set(nco_SOURCES
    src/nco/src/nco_crcf.c
    src/nco/src/nco.utilities.c
)

# MODULE - optim (optimization)
set(optim_SOURCES
    src/optim/src/chromosome.c
    src/optim/src/gasearch.c
    src/optim/src/gradsearch.c
    src/optim/src/optim.common.c
    src/optim/src/qnsearch.c
    src/optim/src/qs1dsearch.c
    src/optim/src/utilities.c
)

# MODULE - quantization
set(quantization_SOURCES
    src/quantization/src/compand.c
    src/quantization/src/quantizer.inline.c
    src/quantization/src/quantizercf.c
    src/quantization/src/quantizerf.c
)

# MODULE - random
set(random_SOURCES
    src/random/src/rand.c
    src/random/src/randexp.c
    src/random/src/randgamma.c
    src/random/src/randn.c
    src/random/src/randnakm.c
    src/random/src/randricek.c
    src/random/src/randweib.c
    src/random/src/scramble.c
)

# MODULE - sequence
set(sequence_SOURCES
  src/sequence/src/bsequence.c
  src/sequence/src/msequence.c
)

# MODULE - utility
set(utility_SOURCES
  src/utility/src/bshift_array.c
  src/utility/src/byte_utilities.c
  src/utility/src/memory.c
  src/utility/src/msb_index.c
  src/utility/src/pack_bytes.c
  src/utility/src/shift_array.c
  src/utility/src/utility.c
)

# MODULE - vector
set(vector_SOURCES
  src/vector/src/vectorcf_add.port.c
  src/vector/src/vectorcf_mul.port.c
  src/vector/src/vectorcf_norm.port.c
  src/vector/src/vectorcf_trig.port.c
  src/vector/src/vectorf_add.port.c
  src/vector/src/vectorf_mul.port.c
  src/vector/src/vectorf_norm.port.c
  src/vector/src/vectorf_trig.port.c
)
