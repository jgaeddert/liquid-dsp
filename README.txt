======================================================================================

  Liquid Software Radio

======================================================================================

Modules:
    math: transcendental functions (sin,cos,tan,atan), others: gamma, besseli
    filters: fir, iir, polyphase, design, remez, etc.
    matrix: lu_decomp, inv, gauss_elim, 
    polynomial: roots, etc.
    optim: newton_raphson, ga, gradient
    modem: modulate, demodulate, psk, dpsk, qam, oqam, aqam, msk, fsk
    nco: mix, pll
    buffers: circular
    sequence: lfsr, complementary_code

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
    src/
        include/            <-- initially empty directory
        module1/
            configure
            Makefile
            README
            api/
                module1.h   <-- external header, defines
                                all APIs for external use
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


