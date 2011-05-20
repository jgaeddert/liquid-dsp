=========================================================================

  liquid-dsp : Software-Defined Radio Digital Signal Processing Library

=========================================================================

liquid-dsp is a free and open-source digital signal processing (DSP)
library designed specifically for software-defined radios on embedded
platforms. The aim is to provide a lightweight DSP library that does not
rely on a myriad of external dependencies or proprietary and otherwise
cumbersome frameworks. All signal processing elements are designed to be
flexible, scalable, and dynamic, including filters, filter design,
oscillators, modems, synchronizers, and complex mathematical operations.

For more information, please refer to the full documentation directory
'doc/' or download the pre-compiled .pdf documentation file from
http://ganymede.ece.vt.edu/downloads/liquid.pdf

=========================================================================
 Installation and Dependencies
=========================================================================

liquid-dsp only relies on libc (standard C) and libm (math) libraries to
run, however takes advantage of other packages if they are available.

Mac OS X:
    $ sudo port install fftw-3-single

Ubuntu
    $ sudo apt-get install libfftw3-dev

BUILD:
    $ ./reconf
    $ ./configure
    $ make

INSTALL:
    # make install

UNINSTALL:
    # make uninstall

TEST:
    $ make check

EXAMPLES: Nearly all signal processing elements have a corresponding
          example in the 'examples/' directory.  Most example scripts
          generate an output .m file for plotting with GNU octave
          [octave:web].
    $ make examples
    -- or --
    $ make examples/specific_example

BENCHMARK:
    $ make bench

Modules: description
    agc: automatic gain control, squelch, rssi
    audio: source audio encoders/decoders: cvsd, filterbanks...
    buffer: internal buffering, circular/static, ports (threaded)
    dotprod: dot products (real, complex)
    equalization: adaptive equalizers: LMS, RLS, blind...
    fec: forward error correction (basic), checksum, crc, etc.
    fft: fast Fourier transform (basic), simple implementation
    filter: fir, iir, polyphase, hilbert, interp, decim, design,
      resampling, symbol timing recovery
    framing: packet framing, encoding, synchronization, interleaving
    math: transcendental functions not in the C standard library (gamma,
      besseli, etc.), polynomial operations (roots, etc.)
    matrix: basic math, lu_decomp, inv, gauss_elim, 
    modem: modulate, demodulate, psk, dpsk, qam, oqam, aqam, msk, fsk
    multicarrier: channelizers, OFDM/OQAM, OFDM...
    nco: numerically-controlled oscillator: mix, pll
    optim: newton_raphson, ga, gradient
    quantization: analog/digital converters, companding...
    random: random number generators
    sequence: lfsr, complementary_code
    utility: useful miscellany
    experimental: artificial neural networks, communications channel
      modeling, threaded ports, filterbank audio synthesizer,
      continuous-phase modulation, direct digital synthesis, quadrature
      mirror filterbanks, advanced symbol timing recovery

Additional:
    benchmarking tool
    automatic test scripts
    full documentation (NOT doxygen)

=========================================================================
 References
=========================================================================
    [octave:web] GNU Octave website, http://www.gnu.org/software/octave/

