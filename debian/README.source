
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

For more information, please refer to the documentation:

  * online HTML version: [http://liquidsdr.org/doc](http://liquidsdr.org/doc)
  * pre-compiled `.pdf` version: [liquid.pdf](http://liquidsdr.org/downloads/liquid.pdf) (4.4 MB).

Installation and Dependencies
-----------------------------

liquid-dsp only relies on `libc` and `libm` (standard C and math)
libraries to run; however liquid will take advantage of other packages
(such as [FFTW](http://www.fftw.org)) if they are available.

### Getting the source code ###

There are two primary ways of obtaining the source code:

1. Clone the entire [repository](http://github.com/jgaeddert/liquid-dsp)
   (recommended)
        
        $ git clone git://github.com/jgaeddert/liquid-dsp.git

2. or download the [tarball](http://liquidsdr.org/downloads/liquid-dsp-1.2.0.tar.gz)
   (2.6 MB), validate the checksum, and unpack 

        $ wget http://liquidsdr.org/downloads/liquid-dsp-1.2.0.tar.gz
        $ wget http://liquidsdr.org/downloads/liquid-dsp.md5
        $ md5sum --check liquid-dsp.md5
        $ tar -xf liquid-dsp-1.2.0.tar.gz

### Installation ###

Once you have obtained a copy of the source code, you can now build the
DSP library (NOTE: if you chose to clone the repository, you will need
to also run the additional `./bootstrap.sh` script before configuring):

    $ ./bootstrap.sh     # <- only if you cloned the Git repo
    $ ./configure
    $ make
    $ sudo make install

If you are installing on Linux for the first time, you will also need
to rebind your dynamic libraries with `sudo ldconfig` to make the
shared object available.

If you decide that you want to remove the installed DSP library, simply
run

    $ sudo make uninstall

### Run all test scripts ###

Source code validation is a critical step in any software library,
particulary for verifying the portability of code to different
processors and platforms. Packaged with liquid-dsp are a number of
automatic test scripts to validate the correctness of the source code.
The test scripts are located under each module's `tests/` directory and
take the form of a C source file. liquid includes a framework for
compiling, linking, and running the tests, and can be invoked with the
make target `check`, viz.

    $ make check

### Examples ###

Nearly all signal processing elements have a corresponding example in
the `examples/` directory.  Most example scripts generate an output
`.m` file for plotting with [GNU octave](http://www.gnu.org/software/octave/)
All examples are built as stand-alone programs and can be compiled with
the make target `examples`:

    $ make examples

Sometimes, however, it is useful to build one example individually.
This can be accomplished by directly targeting its binary
(e.g. `make examples/modem_example`). The example then can be run at the
command line, viz. `./examples/modem_example`.

### Benchmarking tool ###

Packaged with liquid are benchmarks to determine the speed each signal
processing element can run on your machine. Initially the tool provides
an estimate of the processor's clock frequency and will then estimate
the number of trials so that each benchmark will take between 50 and
500 ms to run. You can build and run the benchmark program with the
following command:

    $ make bench

Available Modules
-----------------

  * _agc_: automatic gain control, received signal strength
  * _audio_: source audio encoders/decoders: cvsd, filterbanks
  * _buffer_: internal buffering, circular/static, ports (threaded)
  * _channel_: additive noise, multi-path fading, carrier phase/frequency
        offsets, timing phase/rate offsets
  * _dotprod_: inner dot products (real, complex), vector sum of squares
  * _equalization_: adaptive equalizers: least mean-squares, recursive
        least squares, semi-blind
  * _fec_: basic forward error correction codes including several
        Hamming codes, single error correction/double error detection,
        Golay block code, as well as several checksums and cyclic
        redundancy checks, interleaving, soft decoding
  * _fft_: fast Fourier transforms (arbitrary length), discrete sin/cos
        transforms
  * _filter_: finite/infinite impulse response, polyphase, hilbert,
        interpolation, decimation, filter design, resampling, symbol
        timing recovery
  * _framing_: flexible framing structures for amazingly easy packet
        software radio; dynamically adjust modulation and coding on the
        fly with single- and multi-carrier framing structures
  * _math_: transcendental functions not in the C standard library
        (gamma, besseli, etc.), polynomial operations (curve-fitting,
        root-finding, etc.)
  * _matrix_: basic math, LU/QR/Cholesky factorization, inversion,
        Gauss elimination, Gram-Schmidt decomposition, linear solver,
        sparse matrix representation
  * _modem_: modulate, demodulate, PSK, differential PSK, QAM, optimal
        QAM, as well as analog and non-linear digital modulations GMSK)
  * _multichannel_: filterbank channelizers, OFDM
  * _nco_: numerically-controlled oscillator: mixing, frequency
        synthesis, phase-locked loops
  * _optim_: (non-linear optimization) Newton-Raphson, evoluationary
        algorithms, gradient descent, line search
  * _quantization_: analog/digital converters, compression/expansion
  * _random_: (random number generators) uniform, exponential, gamma,
        Nakagami-m, Gauss, Rice-K, Weibull
  * _sequence_: linear feedback shift registers, complementary codes,
        maximal-length sequences
  * _utility_: useful miscellany, mostly bit manipulation (shifting,
        packing, and unpacking of arrays)
  * _vector_: generic vector operations

### License ###

liquid projects are released under the X11/MIT license.
Short version: this code is copyrighted to me (Joseph D. Gaeddert),
I give you permission to do wantever you want with it except remove my name
from the credits. See the LICENSE file or
[http://opensource.org/licenses/MIT](http://opensource.org/licenses/MIT)
for specific terms.

