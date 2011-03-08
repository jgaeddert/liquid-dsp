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
    pdflatex    : texlive-base, texlive-latex-extra, (texlive-full)
    epstopdf    : (?)
    gnuplot     : v4.2 (?)
    pygments    : (sudo apt-get install python-pygments)
    pgf 2.0     : (sudo apt-get install pgf)
    liquid      : build and install the base DSP libraries

    (experimental html dependencies)
    dvipng      : dvi to png converter (sudo apt-get install dvipng)
    convert     : ImageMagick's command-line image conversion (html)

Documentation targets:
    liquid.pdf      :   main documentation file
    figures         :   automatically-generated figures
    listings        :   fancy code listings
    clean-figures   :   clean all auto-generated figures
    clean-listings  :   clean all auto-generated listings
    html            :   html documentation (under construction)

Directory listings and their contents
    figures.gen/    :   automatically-generated figures (gnuplot)
    figures.pgf/    :   automatically-generated figures (PGF)
    include/        :   documentation library include files
    lib/            :   documentation library source files
    listings/       :   fancy code listings
    sandbox/        :   playground for testing
    sections/       :   documentation .tex files
    src/            :   auto-generated figures source files

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

Installing on Mac OS X
    $ sudo port install texlive

Installing on Ubuntu Linux
    $ sudo apt-get install texlive-full

Additional notes:
    Splatter graphics were created using GIMP with two splatter brush
    plug-ins.
      * http://www.gimp.org/
      * http://corelila.deviantart.com/art/Splatter-Brushes-60718934
      * http://hawksmont.com/blog/gimp-brushes-splatters/




