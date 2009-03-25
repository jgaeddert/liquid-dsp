======================================================================================

  Liquid : Software-Defined Radio Digital Signal Processing Library

======================================================================================

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

Using oprofile (http://...) with the benchmark tool to profile liquid
    # opcontrol --setup --no-vmlinux
    # opcontrol --reset
    # opcontrol --start
    $ ./benchmark -p0 -c2.0e9 -n1000000
    # opcontrol --shutdown
    # opannotate --source | vim -

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
    README.txt              <-- this file
    common.mk               <-- common makefile for building each module
    configure.in            <-- configuration shell script prototype
    makefile.in             <-- top-level makefile prototype
    reconf                  <-- bootstrapping shell script
    doc/
        makefile
        liquid.tex
    include/
        liquid.h            <-- external header, defines
                                all APIsfor external use
        liquid.internal.h   <-- internal header
    src/
        include/            <-- initially empty directory
        module1/
            module.mk       <-- top-level included makefile for
                                building this specific module
            README
            src/
                object1.h   <-- internal header
                object1.1.c <-- internal source
                object1.2.c <-- internal source
                ...
            tests/
                autotest1.1.h
                autotest1.2.h
                ...
            bench/
                benchmark1.h
                benchmark2.h
                ...
        module2/
            ...
    examples/
        example.mk          <-- top-level included makefile
        example1.c
        example2.c

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

Repository organization
/svn/liquid/
    liquid/
        trunk/
        branches/
        tags/
    people/
        jgaeddert/
    framework/              <-- framework for connecting modules
    modules/                <-- processing blocks
        interpolator/


