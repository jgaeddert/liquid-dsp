======================================================================================

  Liquid Software Radio

======================================================================================

Modules:
    agc: automatic gain control
    ann: artificial neural networks
    buffer: internal buffering, circular/static
    fec: forward error correction (basic), checksum, etc.
    fft: fast Fourier transform (basic), simple implementation
    filters: fir, iir, polyphase, design, remez, etc.
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
    README
    configure
    Makefile
    doc/
        Makefile
        liquid.tex
    include/
        header1.h           <-- external header, defines
                                all APIsfor external use
        header2.h
    src/
        include/            <-- initially empty directory
        module1/
            configure
            Makefile
            README
            doc/
                figures/
                refman.tex
            examples/
                example1.c
                example2.c
                Makefile
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

Documentation organization
    Introduction
    Usage
        #include "liquid/module1.h"
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


