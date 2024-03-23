# Available at setup time due to pyproject.toml
from pybind11.setup_helpers import Pybind11Extension, build_ext
from setuptools import setup

__version__ = "0.0.1"

# The main interface is through Pybind11Extension.
# * You can add cxx_std=11/14/17, and then build_ext can be removed.
# * You can set include_pybind11=false to add the include directory yourself,
#   say from a submodule.
#
# Note:
#   Sort input source files if you glob sources to ensure bit-for-bit
#   reproducible builds (https://github.com/pybind/python_example/pull/53)

obj = [
    "src/libliquid.o",
    "src/agc/src/agc_crcf.o",
    "src/agc/src/agc_rrrf.o",
    "src/audio/src/cvsd.o",
    "src/buffer/src/bufferf.o",
    "src/buffer/src/buffercf.o",
    "src/channel/src/channel_cccf.o",
    "src/dotprod/src/dotprod_cccf.o",
    "src/dotprod/src/dotprod_crcf.o",
    "src/dotprod/src/dotprod_rrrf.o",
    "src/dotprod/src/sumsq.o",
    "src/equalization/src/equalizer_cccf.o",
    "src/equalization/src/equalizer_rrrf.o",
    "src/fec/src/crc.o",
    "src/fec/src/fec.o",
    "src/fec/src/fec_conv.o",
    "src/fec/src/fec_conv_poly.o",
    "src/fec/src/fec_conv_pmatrix.o",
    "src/fec/src/fec_conv_punctured.o",
    "src/fec/src/fec_golay2412.o",
    "src/fec/src/fec_hamming74.o",
    "src/fec/src/fec_hamming84.o",
    "src/fec/src/fec_hamming128.o",
    "src/fec/src/fec_hamming1511.o",
    "src/fec/src/fec_hamming3126.o",
    "src/fec/src/fec_hamming128_gentab.o",
    "src/fec/src/fec_pass.o",
    "src/fec/src/fec_rep3.o",
    "src/fec/src/fec_rep5.o",
    "src/fec/src/fec_rs.o",
    "src/fec/src/fec_secded2216.o",
    "src/fec/src/fec_secded3932.o",
    "src/fec/src/fec_secded7264.o",
    "src/fec/src/interleaver.o",
    "src/fec/src/packetizer.o",
    "src/fec/src/sumproduct.o",
    "src/fft/src/fftf.o",
    "src/fft/src/spgramcf.o",
    "src/fft/src/spgramf.o",
    "src/fft/src/fft_utilities.o",
    "src/filter/src/bessel.o",
    "src/filter/src/butter.o",
    "src/filter/src/cheby1.o",
    "src/filter/src/cheby2.o",
    "src/filter/src/ellip.o",
    "src/filter/src/filter_rrrf.o",
    "src/filter/src/filter_crcf.o",
    "src/filter/src/filter_cccf.o",
    "src/filter/src/firdes.o",
    "src/filter/src/firdespm.o",
    "src/filter/src/firdespm_halfband.o",
    "src/filter/src/fnyquist.o",
    "src/filter/src/gmsk.o",
    "src/filter/src/group_delay.o",
    "src/filter/src/hM3.o",
    "src/filter/src/iirdes.pll.o",
    "src/filter/src/iirdes.o",
    "src/filter/src/lpc.o",
    "src/filter/src/rcos.o",
    "src/filter/src/rkaiser.o",
    "src/filter/src/rrcos.o",
    "src/framing/src/bpacketgen.o",
    "src/framing/src/bpacketsync.o",
    "src/framing/src/detector_cccf.o",
    "src/framing/src/dsssframegen.o",
    "src/framing/src/dsssframesync.o",
    "src/framing/src/framedatastats.o",
    "src/framing/src/framesyncstats.o",
    "src/framing/src/framegen64.o",
    "src/framing/src/framesync64.o",
    "src/framing/src/framingcf.o",
    "src/framing/src/framing_rrrf.o",
    "src/framing/src/framing_crcf.o",
    "src/framing/src/framing_cccf.o",
    "src/framing/src/flexframegen.o",
    "src/framing/src/flexframesync.o",
    "src/framing/src/fskframegen.o",
    "src/framing/src/fskframesync.o",
    "src/framing/src/gmskframegen.o",
    "src/framing/src/gmskframesync.o",
    "src/framing/src/ofdmflexframegen.o",
    "src/framing/src/ofdmflexframesync.o",
    "src/framing/src/qpilotgen.o",
    "src/framing/src/qpilotsync.o",
    "src/math/src/poly.o",
    "src/math/src/polyc.o",
    "src/math/src/polyf.o",
    "src/math/src/polycf.o",
    "src/math/src/math.o",
    "src/math/src/math.bessel.o",
    "src/math/src/math.gamma.o",
    "src/math/src/math.complex.o",
    "src/math/src/math.trig.o",
    "src/math/src/modular_arithmetic.o",
    "src/math/src/poly.findroots.o",
    "src/math/src/windows.o",
    "src/matrix/src/matrix.o",
    "src/matrix/src/matrixf.o",
    "src/matrix/src/matrixc.o",
    "src/matrix/src/matrixcf.o",
    "src/matrix/src/smatrix.common.o",
    "src/matrix/src/smatrixb.o",
    "src/matrix/src/smatrixf.o",
    "src/matrix/src/smatrixi.o",
    "src/modem/src/ampmodem.o",
    "src/modem/src/cpfskdem.o",
    "src/modem/src/cpfskmod.o",
    "src/modem/src/fskdem.o",
    "src/modem/src/fskmod.o",
    "src/modem/src/gmskdem.o",
    "src/modem/src/gmskmod.o",
    "src/modem/src/modem.shim.o",
    "src/modem/src/modemcf.o",
    "src/modem/src/modem_utilities.o",
    "src/modem/src/modem_apsk_const.o",
    "src/modem/src/modem_arb_const.o",
    "src/multichannel/src/firpfbch_crcf.o",
    "src/multichannel/src/firpfbch_cccf.o",
    "src/multichannel/src/ofdmframe.common.o",
    "src/multichannel/src/ofdmframegen.o",
    "src/multichannel/src/ofdmframesync.o",
    "src/nco/src/nco_crcf.o",
    "src/nco/src/nco.utilities.o",
    "src/optim/src/chromosome.o",
    "src/optim/src/gasearch.o",
    "src/optim/src/gradsearch.o",
    "src/optim/src/optim.common.o",
    "src/optim/src/qnsearch.o",
    "src/optim/src/qs1dsearch.o",
    "src/optim/src/utilities.o",
    "src/quantization/src/compand.o",
    "src/quantization/src/quantizercf.o",
    "src/quantization/src/quantizerf.o",
    "src/quantization/src/quantizer.inline.o",
    "src/random/src/rand.o",
    "src/random/src/randn.o",
    "src/random/src/randexp.o",
    "src/random/src/randweib.o",
    "src/random/src/randgamma.o",
    "src/random/src/randnakm.o",
    "src/random/src/randricek.o",
    "src/random/src/scramble.o",
    "src/sequence/src/bsequence.o",
    "src/sequence/src/msequence.o",
    "src/utility/src/bshift_array.o",
    "src/utility/src/byte_utilities.o",
    "src/utility/src/memory.o",
    "src/utility/src/msb_index.o",
    "src/utility/src/pack_bytes.o",
    "src/utility/src/shift_array.o",
    "src/utility/src/utility.o",
    "src/vector/src/vectorf_add.port.o",
    "src/vector/src/vectorf_norm.port.o",
    "src/vector/src/vectorf_mul.port.o",
    "src/vector/src/vectorf_trig.port.o",
    "src/vector/src/vectorcf_add.port.o",
    "src/vector/src/vectorcf_norm.port.o",
    "src/vector/src/vectorcf_mul.port.o",
    "src/vector/src/vectorcf_trig.port.o",
]

ext_modules = [
    Pybind11Extension(
        name="liquid",
        sources=["bind/liquid.python.cc", ],
        libraries=["m","c","fftw3f"],
        include_dirs=['.','./include','./bind'],
        # Example: passing in the version to the compiled code
        define_macros=[("VERSION_INFO", __version__),
                       ("LIQUID_PYTHONLIB", 1)],
        extra_objects=obj,
    ),
]

setup(
    name="liquid",
    version=__version__,
    author="Joseph D. Gaeddert",
    author_email="joseph@liquidsdr.org",
    url="https://liquidsdr.org",
    description="Software-Defined Radio Digital Signal Processing Library",
    long_description="",
    ext_modules=ext_modules,
    extras_require={"test": "pytest"},
    # Currently, build_ext only provides an optional "highest supported C++
    # level" feature, but in the future it may provide more features.
    cmdclass={"build_ext": build_ext},
    zip_safe=False,
    python_requires=">=3.7",
)

# bind/liquid.python.cc
# bind/main.cc
# bind/fdelay.hh
# bind/fg64.hh
# bind/firfilt.hh
# bind/firhilb.hh
# bind/firinterp.hh
# bind/firpfbch2.hh
# bind/fs64.hh
# bind/liquid.hh
# bind/liquid.python.hh
# bind/modem.hh
# bind/msresamp.hh
# bind/nco.hh
# bind/ofdmflexframerx.hh
# bind/ofdmflexframetx.hh
# bind/rresamp.hh
# bind/spgram.hh
# bind/spwaterfall.hh
# bind/symstreamr.hh
# bind/symtrack.hh

#g++ -g -O2 -Wall -fPIC -Wno-deprecated -Wno-deprecated-declarations -std=c++14 -Wno-return-type-c-linkage 
#  -D LIQUID_PYTHONLIB -I/opt/homebrew/opt/python@3.12/Frameworks/Python.framework/Versions/3.12/include/python3.12 -I/opt/homebrew/Cellar/pybind11/2.11.1/libexec/lib/python3.12/site-packages/pybind11/include
#  -I./include -I./bind/ -c -o bind/liquid.python.o",
#bind/liquid.python.cc

