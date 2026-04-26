
Autotests
=========

.. author   jgaeddert Joseph D. Gaeddert <joseph@liquidsdr.org>
.. date     January 1, 1975
.. location Boston, MA
.. header   /doc/datastructures/banner.png
.. keywords data structures, rrrf, crcf, cccf

Source code validation is a critical step in any software library,
particularly for verifying the portability of code to different processors and
platforms.
Packaged with |liquid| are a number of automatic test scripts to validate the
correctness of the source code.
The test scripts are located under each module's `tests` directory and
take the form of a C source file.
The testing framework operates similarly to 
`cppunit <https://sourceforge.net/projects/cppunit/>`_ and
`cxxtest <https://cxxtest.com/>`_,
however it is written in C.
The generator script `scripts/autoscript` parses these header files looking for
the key :c:`void autotest_` which corresponds to a specific test.
The script generates the header file ``autotest_include.h`` which
includes all the modules' test headers as well as several organizing
structures for keeping track of which tests have passed or failed.
The result is an executable file, ``xautotest``, which can be run to
validate the functional correctness of |liquid| on your target platform.

You can run all the tests and generate an output report simply with
``make check``.
As of mid 2026 there are over 1,300 test scripts and over 700,000 checks
which
`cover over 80% <https://codecov.io/gh/jgaeddert/liquid-dsp/tree/master/src>`_
of all of |liquid|.


Running the autotests
---------------------

The result is an executable file named `xautotest` which has several
options for running.
These options may be viewed with either the `-h` or `-u` flags (for
help/usage information).

.. todo:: update this help block

.. code-block::

    $ ./xautotest -h
    Usage: xautotest [OPTION]
    Execute autotest scripts for liquid-dsp library.
      -h            display this help and exit
      -t <id>       run specific test
      -H <id>       hammer on a specific test
      -c <count>    number of times to run hammer test
      -p <id>       run specific package
      -r            run all tests, random order
      -R <seed>     specify random seed value
      -L            lists all scripts
      -l            lists all packages
      -x            stop on fail
      -s <string>   run all tests matching search string
      -v            verbose
      -q            quiet
      -o <filename> output file (json)

Simply running the program without any arguments executes all the tests and
displays the results to the screen.
This is the default response of the target `make check` with the legacy build.


Random Seed
-----------

Tests are run by setting the random seed to `time(NULL)` by default.
This allows greater coverage for testing by effectively choosing a different
random seed each time the test suite is run;
however because repeatability is important for debugging, the random seed
can be specified with the `-R` flag.


Output Report
-------------

When the autotest script is run, it generates a
`JSON <https://www.json.org>`_
file which tabulates the entirety of the test results.
This file can be useful for reproducing the results (particularly failures)
in specific environments and machine architectures.

.. code-block:: json

    {
      "build-info" : {},
      "pass" : true,
      "num_failed" : 0,
      "num_checks" : 369656,
      "num_warnings" : 55,
      "command-line" : " ./xautotest -v -o autotest.json",
      "run-mode" : "RUN_ALL",
      "rseed" : 1660183820,
      "stop-on-fail" : false,
      "tests" : [
        {"id":  0, "pass":true, "num_checks":   1, "num_passed":   1, "name":"null"},
        {"id":  1, "pass":true, "num_checks":   2, "num_passed":   2, "name":"libliquid"},
        {"id":  2, "pass":true, "num_checks":   4, "num_passed":   4, "name":"agc_crcf_dc_gain_control"},
        {"id":  3, "pass":true, "num_checks":   3, "num_passed":   3, "name":"agc_crcf_scale"},
        {"id":  4, "pass":true, "num_checks":   1, "num_passed":   1, "name":"agc_crcf_ac_gain_control"},
        {"id":  5, "pass":true, "num_checks":   1, "num_passed":   1, "name":"agc_crcf_rssi_sinusoid"},
        {"..."},
        {"id":1091, "pass":true, "num_checks":   5, "num_passed":   5, "name":"lshift"},
        {"id":1092, "pass":true, "num_checks":   5, "num_passed":   5, "name":"rshift"},
        {"id":1093, "pass":true, "num_checks":   5, "num_passed":   5, "name":"lcircshift"},
        {"id":1094, "pass":true, "num_checks":   5, "num_passed":   5, "name":"rcircshift"}
      ]
    }

Autotest Examples
-----------------

Run all autotests matching the string "firhilb":

.. code-block:: bash

    $ ./xautotest -s firhilb
    # ...
    56: firhilb:
       420[    0.17 ms]   PASS   passed   32/  32 checks (100.0%) firhilbf_decim
       421[    0.15 ms]   PASS   passed   32/  32 checks (100.0%) firhilbf_interp
       422[    3.80 ms]   PASS   passed 6880/6880 checks (100.0%) firhilbf_psd
       423[    0.01 ms]   PASS   passed    2/   2 checks (100.0%) firhilbf_invalid_config
       424[    0.53 ms]   PASS   passed  160/ 160 checks (100.0%) firhilbf_copy_interp
       425[    0.47 ms]   PASS   passed   80/  80 checks (100.0%) firhilbf_copy_decim

    autotest seed: 1758750751
    ==================================
     PASSED ALL 7186 CHECKS
    ==================================

Run test `1056`:

.. code-block:: bash

    $ ./xautotest -t 1056
    demodsoft_apsk8:
      1056[    0.03 ms]   PASS   passed   16/  16 checks (100.0%) demodsoft_apsk8
    autotest seed: 1758750852
    ==================================
     PASSED ALL 16 CHECKS
    ==========================

Hammering Tests
---------------

As with stochastic digital signal processing, many tests rely on random numbers
to function.
While it would be pertinent to set the same seed each time the suite of tests
are run, it is also useful to find repeatable edge cases for which a test might
fail.
Running a single test repeatedly (hammering it to see if it breaks) can be
invoked with the ``-H`` option:

.. code-block:: bash

    $ ./xautotest -H 1056
    demodsoft_apsk8:
    trial 1/100, rseed=1758751075
    # ...
    trial 100/100, rseed=1758751174
    demodsoft_apsk8:
      1056[    0.00 ms]   PASS   passed   16/  16 checks (100.0%) demodsoft_apsk8
    autotest seed: 1758751075
    ==================================
     PASSED ALL 1600 CHECKS
    ==================================

.. toctree::
    :caption: Getting Started
    :maxdepth: 1

    macros

