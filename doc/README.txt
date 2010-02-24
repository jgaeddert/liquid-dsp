=============================================================================

  Liquid : Documentation

=============================================================================

This is the documentation directory...

To build the documentation, one must move to the root source directory, and
build/install liquid.  Then move to this directory and simply run 'make.'

Documentation dependencies:
    pdflatex    : texlive-base, texlive-latex-extra, (texlive-full)
    epstopdf    : (?)
    gnuplot     : v4.2 (?)
    pygments    : (sudo apt-get install python-pygments)
    liquid      : build and install the base DSP libraries

Documentation targets:
    liquid.pdf      :   main documentation file
    figures         :   automatically-generated figures
    listings        :   fancy code listings
