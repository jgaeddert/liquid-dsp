
liquid-dsp
==========
Software-Defined Radio Digital Signal Processing Library

liquid-dsp is a free and open-source digital signal processing (DSP)
library designed specifically for software-defined radios on embedded
platforms. The aim is to provide a lightweight DSP library that does not
rely on a myriad of external dependencies or proprietary and otherwise
cumbersome frameworks. All signal processing elements are designed to be
flexible, scalable, and dynamic, including filters, filter design,
oscillators, modems, synchronizers, and complex mathematical operations.

For more information, please refer to the full documentation directory
`doc/` or download the pre-compiled `.pdf` documentation file from
<http://ganymede.ece.vt.edu/downloads/liquid.pdf> (4.4 MB).

Installation and Dependencies
-----------------------------

liquid-dsp only relies on `libc` and `libm` (standard C and math)
libraries to run; however liquid will take advantage of other packages
(such as [FFTW][]) if they are available.

### Getting the source code

There are two primary ways of obtaining the source code:

1. Download the tarball from
   <http://ganymede.ece.vt.edu/downloads/liquid-dsp-1.2.0.tar.gz> (2.6 MB),
   validate the checksum, and unpack 

        $ wget http://ganymede.ece.vt.edu/downloads/liquid-dsp-1.2.0.tar.gz
        $ wget http://ganymede.ece.vt.edu/downloads/liquid-dsp.md5
        $ md5sum --check liquid-dsp.md5
        $ tar -xf liquid-dsp-1.2.0.tar.gz

2. Clone the entire [repository][liquid-github]
        
        $ git clone git://github.com/jgaeddert/liquid-dsp.git

### Installation

Once you have obtained a copy of the source code, you can now build the
DSP library (NOTE: if you chose to clone the repository, you will need
to also run the additional `./reconf` bootstrapping script before
configuring):

    $ ./configure
    $ make
    $ sudo make install

If you are installing on Linux for the first time, you will also need
to rebind your dynamic libraries with `sudo ldconfig` to make the
shared object available.

If you decide that you want to remove the installed DSP library, simply
run

    $ sudo make uninstall

### Run all test scripts

Source code validation is a critical step in any software library,
particulary for verifying the portability of code to different
processors and platforms. Packaged with liquid-dsp are a number of
automatic test scripts to validate the correctness of the source code.
The test scripts are located under each module's `tests/` directory and
take the form of a C source file. liquid includes a framework for
compiling, linking, and running the tests, and can be invoked with the
make target `check`, viz.

    $ make check

### Examples
Nearly all signal processing elements have a corresponding example in
the `examples/` directory.  Most example scripts generate an output
`.m` file for plotting with [GNU octave][octave]. All examples are
built as stand-alone programs and can be compiled with the make target
`examples`:

    $ make examples

Sometimes, however, it is useful to build one example individually.
This can be accomplished by directly targeting its binary
(e.g. `make examples/modem_example`). The example then can be run at the
command line, viz. `./examples/modem_example`.

### Benchmarking tool

Packaged with liquid are benchmarks to determine the speed each signal
processing element can run on your machine. Initially the tool provides
an estimate of the processor's clock frequency and will then estimate
the number of trials so that each benchmark will take between 50 and
500 ms to run. You can build and run the benchmark program with the
following command:

    $ make bench

Available Modules
-----------------
  * _agc_: automatic gain control, squelch, received signal strength
  * _audio_: source audio encoders/decoders: cvsd, filterbanks
  * _buffer_: internal buffering, circular/static, ports (threaded)
  * _dotprod_: inner dot products (real, complex)
  * _equalization_: adaptive equalizers: least mean-squares, recursive
        least squares, blind
  * _fec_: basic forward error correction codes including several
        Hamming codes, single error correction/double error detection,
        Golay block code, as well as several checksums and cyclic
        redundancy checks
  * _fft_: fast Fourier transforms (arbitrary length), discrete sin/cos
        transforms
  * _filter_: finite/infinite impulse response, polyphase, hilbert,
        interpolation, decimation, filter design, resampling, symbol
        timing recovery
  * _framing_: packet framing, encoding, synchronization, interleaving
  * _math_: transcendental functions not in the C standard library
        (gamma, besseli, etc.), polynomial operations (curve-fitting,
        root-finding, etc.)
  * _matrix_: basic math, LU/QR/Cholesky factorization, inversion,
        Gauss elimination, Gram-Schmidt decomposition, linear solver,
        sparse matrix representation
  * _modem_: modulate, demodulate, PSK, differential PSK, QAM, optimal
        QAM, as well as analog and non-linear digital modulations (FSK)
  * _multicarrier_: filterbank channelizers, OFDM/OQAM, OFDM
  * _nco_: numerically-controlled oscillator: mixing, frequency
        synthesis, phase-locked loops
  * _optim_: (non-linear optimization) Newton-Raphson, evoluationary
        algorithms, gradient descent
  * _quantization_: analog/digital converters, compression/expansion
  * _random_: (random number generators) uniform, exponential, gamma,
        Nakagami-m, Gauss, Rice-K, Weibull
  * _sequence_: linear feedback shift registers, complementary codes
  * _utility_: useful miscellany, mostly bit manipulation (shifting,
        packing, and unpacking of arrays)
  * _experimental_: artificial neural networks, communications channel
        modeling, threaded ports, filterbank audio synthesizer,
        continuous-phase modulation, direct digital synthesis,
        quadrature mirror filterbanks, advanced symbol timing recovery

Additional
----------
  * benchmarking tool (`make bench`) for testing execution speed of most
    algorithms
  * automatic test scripts (`make check`) for validating the code's
    functionality
  * full documentation (`make doc`) complete with tutorials, code
    examples, and interface descriptions. Requires the following programs
    and packages:
      - [pdflatex][ctan]    LaTeX compiler
      - [bibtex][ctan]      LaTeX bibliography generator
      - [pgf][] (v2.0)      LaTeX figures
      - epstopdf            convert .eps to .pdf (necessary for figures)
      - [gnuplot][]         plotting program
      - [pygments][]        pretty syntax hilighting program

### Additional notes
Splatter graphics were created using [GIMP][] with two
[splatter][corelila] [brush][hawksmont] plug-ins.


[FFTW]:         http://www.fftw.org/
[octave]:       http://www.gnu.org/software/octave/
[ctan]:         http://www.ctan.org/
[pygments]:     http://pygments.org/
[GIMP]:         http://www.gimp.org/
[gnuplot]:      http://www.gnuplot.info/
[pgf]:          http://sourceforge.net/projects/pgf/
[macports]:     http://www.macports.org/
[liquid.tar.gz]:http://ganymede.ece.vt.edu/
[liquid-github]:http://github.com/jgaeddert/liquid-dsp
[corelila]:     http://corelila.deviantart.com/art/Splatter-Brushes-60718934
[hawksmont]:    http://hawksmont.com/blog/gimp-brushes-splatters/

