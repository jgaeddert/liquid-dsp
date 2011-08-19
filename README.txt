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

liquid-dsp only relies on libc (standard C) and libm (standard math)
libraries to run, however takes advantage of other packages (such as
FFTW [fftw:web]) if they are available.
  * Mac OS X: sudo port install fftw-3-single
  * Ubuntu:   sudo apt-get install libfftw3-dev

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
    $ make examples                         # build all examples
    $ make examples/iirfilt_crcf_example    # build a specific example

BENCHMARK:
    $ make bench

=========================================================================
 Available Modules
=========================================================================
  * agc: automatic gain control, squelch, rssi
  * audio: source audio encoders/decoders: cvsd, filterbanks...
  * buffer: internal buffering, circular/static, ports (threaded)
  * dotprod: dot products (real, complex)
  * equalization: adaptive equalizers: LMS, RLS, blind...
  * fec: forward error correction (basic), checksum, crc, etc.
  * fft: fast Fourier transform (basic), discrete sine/cosine
    transforms, simple implementations
  * filter: finite/infinite impulse response, polyphase, hilbert,
    interpolation, decimation, filter design, resampling, symbol timing
    recovery
  * framing: packet framing, encoding, synchronization, interleaving
  * math: transcendental functions not in the C standard library (gamma,
    besseli, etc.), polynomial operations (curve-fitting, root-finding,
    etc.)
  * matrix: basic math, LU/QR/Cholesky factorization, matrix inversion,
    Gauss elimination, Gram-Schmidt decomposition, linear solver
  * modem: modulate, demodulate, psk, dpsk, qam, oqam, aqam, msk, fsk
  * multicarrier: filterbank channelizers, OFDM/OQAM, OFDM
  * nco: numerically-controlled oscillator: mixing, frequency synthesis,
    phase-locked loops
  * optim (non-linear optimization): Newton-Raphson, evoluationary
    algorithms, gradient descent
  * quantization: analog/digital converters, compression/expansion
  * random (random number generators): uniform, exponential, gamma,
    Nakagami-m, Gauss, Rice-K, Weibull
  * sequence: linear feedback shift registers, complementary codes
  * utility: useful miscellany, mostly bit manipulation (shifting,
    packing, and unpacking of arrays)
  * experimental: artificial neural networks, communications channel
    modeling, threaded ports, filterbank audio synthesizer,
    continuous-phase modulation, direct digital synthesis, quadrature
    mirror filterbanks, advanced symbol timing recovery

Additional:
  * benchmarking tool ('make bench') for testing execution speed of most
    algorithms
  * automatic test scripts ('make check') for validating the code's
    functionality
  * full documentation ('make doc') complete with tutorials, code
    examples, and interface descriptions. Requires the following programs
    and packages:
      - pdflatex    LaTeX compiler [http://www.ctan.org]
      - bibtex      LaTeX bibliography generator [http://www.ctan.org]
      - pgf (v2.0)  LaTeX figures [http://sourceforge.net/projects/pgf/]
      - epstopdf    convert .eps to .pdf (necessary for figures)
      - gnuplot     plotting program [http://www.gnuplot.info]
      - pygmentize  pretty syntax hilighting program [http://pygments.org]

=========================================================================
 References
=========================================================================
    [fftw:web] FFTW website, http://www.fftw.org/
    [octave:web] GNU Octave website, http://www.gnu.org/software/octave/
    [ctan:web] The Comprehensive TeX Archive Network, Online:
        http://www.ctan.org/
    [pygments:web] Python Syntax Highlighter, Online: http://pygments.org/
    [gnuplot:web] Gnuplot homepage, Online: http://www.gnuplot.info/
    [pgf:web] PGF and TikZ, Graphics systems for TeX, Online:
        http://sourceforge.net/projects/pgf/

