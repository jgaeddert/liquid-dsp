======================================================================================

  liquid-dsp : Software-Defined Radio Digital Signal Processing Library

======================================================================================

liquid-dsp is a free and open-source digital signal processing (DSP) library
designed specifically for software-defined radios on embedded platforms. The
aim is to provide a lightweight DSP library that does not rely on a myriad of
external dependencies or proprietary and otherwise cumbersome frameworks.
All signal processing elements are designed to be flexible, scalable, and
dynamic, including filters, filter design, oscillators, modems, synchronizers,
and complex mathematical operations.

For more information, please refer to the full documentation directory under
doc/.

BUILD:
    $ ./reconf
    $ ./configure
    $ make

INSTALL:
    # make install

UNINSTALL:
    # make uninstall

TEST: (requires python to generate header file)
    $ make check

EXAMPLES: Nearly all signal processing elements have a corresponding
          example in the 'examples/' directory.  Most example scripts
          generate an output .m file for plotting with GNU octave
          [octave:web].
    $ make examples
    -- or --
    $ make examples/specific_example

BENCHMARK: (requires python to generate header file)
    $ make bench

Using oprofile (http://oprofile.sourceforge.net/) with the benchmark tool to
profile liquid
    # opcontrol --setup --no-vmlinux
    # opcontrol --reset
    # opcontrol --start
    $ ./benchmark -p0 -c2.0e9 -n1000000
    # opcontrol --shutdown
    # opannotate --source | vim -
        or
    # opreport --callgraph -t 0.01 | vim -

Modules: description
    agc: automatic gain control
    audio: source audio encoders/decoders: cvsd, filterbanks...
    buffer: internal buffering, circular/static, ports (threaded)
    dotprod: dot products (real, complex)
    equalization: adaptive equalizers: LMS, RLS, blind...
    fec: forward error correction (basic), checksum, crc, etc.
    fft: fast Fourier transform (basic), simple implementation
    filter: fir, iir, polyphase, hilbert, interp, decim, design, remez, etc.
    framing: packet framing, encoding, synchronization, interleaving
    math: transcendental functions not in the C standard library (gamma, besseli, etc.)
    matrix: basic math, lu_decomp, inv, gauss_elim, 
    modem: modulate, demodulate, psk, dpsk, qam, oqam, aqam, msk, fsk
    multicarrier: OFDM/OQAM, OFDM...
    nco: numerically-controlled oscillator: mix, pll
    optim: newton_raphson, ga, gradient
    [polynomial]: roots, etc.
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

Dependencies
    liquid-dsp only relies on libc (standard C) and libm (math)
    libraries to run, however takes advantage of other packages if they
    are available.

    Mac OS X:
        $ sudo port install fftw-3-single

    Ubuntu
        $ sudo apt-get install libfftw3-dev

Source code organization
liquid/
    README.txt                  <-- this file
    common.mk                   <-- common makefile for building each module
    configure.ac                <-- configuration shell script prototype (autoconf)
    makefile.in                 <-- top-level makefile prototype
    reconf                      <-- bootstrapping shell script
    doc/
        makefile
        liquid.tex
        sections/
        src/
        ...
    include/
        liquid.h                <-- external header, defines
                                    all APIsfor external use
        liquid.internal.h       <-- internal header
    src/
        module1/
            module.mk           <-- top-level included makefile for
                                    building this specific module
            README
            src/
                object_a.c      <-- internal source
                object_b.c      <-- internal source
                ...
            tests/
                autotest_a.h    <-- autotest script
                autotest_b.h    <-- autotest script
                ...
            bench/
                benchmark_a.h   <-- benchmark script
                benchmark_b.h   <-- benchmark script
                ...
        module2/
            ...
    examples/
        example.mk              <-- top-level included makefile
        README                  <-- description of examples
        example1.c              <-- example program source
        example2.c              <-- example program source

Documentation organization
    Introduction
    Usage
        #include <liquid/liquid.h>
    ...
    Module1
        Object1.1
            Interface
            Description of operation
            Other (Accuracy, etc.)
            Bugs
        Object1.2
            ...
    Module2
        ...

======================================================================================
 Misc
======================================================================================

RELEASE:
To build a distribution, move to directory above this one and make a
compressed archive.  Notice that an exclude file is conveniently
included within this directory.

  $ cd ..
  $ tar -czf liquid-dsp-VERSION.tar.gz -X liquid-dsp/.tarignore liquid-dsp/

Replace 'VERSION' with the appropriate version number


======================================================================================
 DSP module dependency tree
======================================================================================
Listed below is the dependency tree each module has on each other.  The aim is
to provide a framework for building modules separately without generating the
entire library if it is either not needed, or consumes too much memory.  This
list might not be complete, and should probably be automatically generated.

    agc             []
    ann             []
    audio           [filter,fft,multicarrier]
    buffer          []
    channel         [random]
    dotprod         []
    equalization    [buffer,dotprod,matrix]
    fec             [utility]
    fft             []
    filter          [buffer,dotprod]
    framing         [agc,fec,filter,nco,sequence]
    interleaver     []
    math            []
    matrix          []
    modem           []
    multicarrier    [filter,fft,nco]
    nco             [filter]
    optim           []
    quantization    []
    random          [math]
    sequence        []
    utility         []

======================================================================================
 References
======================================================================================
    [octave:web] GNU Octave website, http://www.gnu.org/software/octave/

