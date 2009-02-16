======================================================================================

  Liquid : Software-Defined Radio Digital Signal Processing Library

======================================================================================

BUILD:
    $ ./reconf
    $ ./configure
    $ make

INSTALL:
    # make install

TEST: (requires python)
    $ make check

BENCHMARK: (requires python)
    $ make bench
    

Modules: description
    agc: automatic gain control
    ann: artificial neural networks
    buffer: internal buffering, circular/static, ports
    dotprod: dot products (real, complex)
    fec: forward error correction (basic), checksum, etc.
    fft: fast Fourier transform (basic), simple implementation
    filters: fir, iir, polyphase, hilbert, interp, decim, design, remez, etc.
    interleaver: bit- and symbol-level
    math: transcendental functions (sin,cos,tan,atan), others: gamma, besseli
    modem: modulate, demodulate, psk, dpsk, qam, oqam, aqam, msk, fsk
    matrix: lu_decomp, inv, gauss_elim, 
    nco: mix, pll
    optim: newton_raphson, ga, gradient
    polynomial: roots, etc.
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
    configure               <-- configuration shell script
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
        #include "liquid.h"
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


