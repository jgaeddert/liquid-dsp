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

TEST: (requires python to generate header file)
    $ make check

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
    ann: artificial neural networks
    audio: source audio encoders/decoders: cvsd, filterbanks...
    buffer: internal buffering, circular/static, ports (threaded)
    channel: communications channel modeling: additive noise, multipath fading...
    dotprod: dot products (real, complex)
    equalization: adaptive equalizers: LMS, RLS, blind...
    estimation: parametric estimation: frequency offset, timing offset, fading...
    fec: forward error correction (basic), checksum, crc, etc.
    fft: fast Fourier transform (basic), simple implementation
    filter: fir, iir, polyphase, hilbert, interp, decim, design, remez, etc.
    interleaver: bit- and symbol-level
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

Additional:
    benchmarking tool
    automatic test scripts
    full documentation (NOT doxygen)

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
    estimation      []
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

