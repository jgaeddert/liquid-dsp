
liquid-dsp Table Generation ("gentab") Programs
===============================================

This directory contains programs for generating tables and constants used
in liquid-dsp. These programs are not built and run at compile time as
doing this complicates the build process substantially, but rather are
compiled and run offline with the resulting output committed to the main
source tree.

To build and run the table-generation scripts, simply run the following:

.. code-block:: bash

    make -f gentab/makefile


