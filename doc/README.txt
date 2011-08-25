=============================================================================

  liquid documentation

=============================================================================

This is the documentation directory...

All documentation is in LaTeX [ctan:web].  Figures are auto-generated using
gnuplot [gnuplot:web] and pgf [pgf:web].  Code snippets are made pretty using
pygments [pygments:web].

To build the documentation, one must first move to the root source
directory and build the liquid DSP library.  Then move to this directory
and simply run 'make.'  The documentation is built daily for your
convenience at http://ganymede.ece.vt.edu/.


Documentation dependencies:
    pdflatex    : LaTeX distribution
    epstopdf    : postscript to .pdf converter
    gnuplot     : plotting package (version 4.2 or greater)
    pygments    : syntax highlighting
    pgf         : LaTeX graphics package (version 2.0 or greater)
    liquid      : the base DSP libraries

    (experimental html dependencies)
    dvipng      : dvi to png converter (sudo apt-get install dvipng)
    convert     : ImageMagick's command-line image conversion (html)

Documentation targets:
    liquid.pdf      :   main documentation file
    figures         :   automatically-generated figures
    listings        :   fancy code listings
    listings-check  :   validate that code listings compile properly
    clean-figures   :   clean all auto-generated figures
    clean-listings  :   clean all auto-generated listings
    mostlyclean     :   full clean of all documentation (figures, listintgs, etc.)
    distclean       :   removing everything but the originally distributed files
    html            :   html documentation [EXPERIMENTAL]

Directory listings and their contents
    figures.gen/    :   automatically-generated figures (gnuplot)
    figures.pgf/    :   automatically-generated figures (PGF)
    include/        :   documentation library include files
    lib/            :   documentation library source files
    listings/       :   fancy code listings
    sandbox/        :   playground for testing
    sections/       :   documentation .tex files
    src/            :   auto-generated figures source files

Installing on Mac OS X
    $ sudo port install texlive-basic texlive-bin texlive-bin-extra texlive-common \
        texlive-fontutils texlive-latex-extra texlive-latex-recommended texlive-science \
        pgf py-pygments

Installing on Ubuntu Linux
    $ sudo apt-get install texlive-full pgf python-pygments

Additional notes:
    Splatter graphics were created using GIMP with two splatter brush
    plug-ins.
      * http://www.gimp.org/
      * http://corelila.deviantart.com/art/Splatter-Brushes-60718934
      * http://hawksmont.com/blog/gimp-brushes-splatters/

References
    [ctan:web] The Comprehensive TeX Archive Network, Online:
        http://www.ctan.org/
    [pygments:web] Python Syntax Highlighter, Online: http://pygments.org/
    [gnuplot:web] Gnuplot homepage, Online: http://www.gnuplot.info/
    [pgf:web] PGF and TikZ, Graphics systems for TeX, Online:
        http://sourceforge.net/projects/pgf/
    [dvipng:web] dvipng: A DVI-to-PNG converter, Online:
        http://sourceforge.net/projects/dvipng/
    [imagemagick:web] ImageMagick command-line image converter, Online:
        http://www.imagemagick.org/script/convert.php
    [macports:web] MacPorts homepage: open-source package manager, Online:
        http://www.macports.org/




