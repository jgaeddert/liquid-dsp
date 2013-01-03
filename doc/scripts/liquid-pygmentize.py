#!/usr/bin/python
# Copyright (c) 2013 Joseph Gaeddert
#
# This file is part of liquid.
#
# liquid is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# liquid is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with liquid.  If not, see <http://www.gnu.org/licenses/>.


# 
# liquid-pygmentize.py
#
# simple python pygmentize replacement when otherwise unavailable
#

# generate highlight.sty:
#   $ pygmentize -f latex -S default -P verboptions='fontsize=\small' -P commandprefix='PY' > highlight.sty
#
# generate basic code listing:
#   $ pygmentize -f latex -O linenos=True -P verboptions='fontsize=\small' -P commandprefix='PY' -o listings/nco.c.tex -l c listings/nco.c
#
#   \begin{Verbatim}[commandchars=\\\{\},numbers=left,firstnumber=1,stepnumber=1,fontsize=\small]
#   ...
#   ...
#   ...
#   \end{Verbatim}
#

import getopt, sys

def usage():
    '''print usage'''
    print 'usage:...'

def main(argv=None):
    if argv is None:
        argv = sys.argv

    try:
        opts, args = getopt.getopt(sys.argv[1:], "l:gF:f:O:P:o:S:a:L:N:H:hV", [])
    except getopt.GetoptError as err:
        print str(err)
        sys.exit(2)

    # operational mode, default is 'pygmentize' or
    # 'stylesheet' (specified with '-S')
    mode = 'pygmentize'

    # assuming mode is 'pygmentize, output file is
    # specified or standard ouput
    outputfile = None

    # assuming mode is 'pygmentize, input file is
    # last command-line argument
    inputfile = argv[-1]

    for o, a in opts:
        if o == '-S':
            mode = 'stylesheet'
        elif o in ("-o", "--output"):
            outputfile = a;
        else:
            # ignore other options
            pass

    if mode == 'stylesheet':
        PrintStylesheet()
    elif mode == 'pygmentize':
        Pygmentize(inputfile,outputfile)
    else:
        print >>sys.stderr, 'error: not a valid command-line syntax'
        return 1

    '''
    # parse all arguments and look for either '-S' or a filename
    try:
        filename = argv[1]
        brew = Brew(filename)
        brew.Print()
    except usage, err:
        print >>sys.stderr, err.msg
        return 1
    '''

def PrintStylesheet():
    '''Print stylesheet'''

    print '%'
    print '% auto-generated stylesheet from liquid-pygmentize.py'
    print '%'
    print '\\makeatletter'
    print '\\makeatother'

def Pygmentize(_inputfile='', _outputfile=None):
    '''Simple pygmentization of input filename'''

    # try to open file for reading
    try:
        f_in = open(_inputfile, 'r');
    except:
        print >>sys.stderr, 'error opening/parsing input file \'' + _inputfile + '\''
        sys.exit(2)

    # try to open output file for writing
    if _outputfile == None:
        f_out = sys.stdout
    else:
        try:
            f_out = open(_outputfile, 'w');
        except:
            print >>sys.stderr, 'error opening output file \'' + _outputfile + '\''
            sys.exit(2)

    # print header
    # commandchars=\\\\\\{\\}
    print >>f_out, '\\begin{Verbatim}[numbers=left,firstnumber=1,stepnumber=1,,fontsize=\\small]'

    # 'parse' file (simply read each line and print to screen)
    for line in f_in:
        # TODO: strip out tokens (e.g. 'int', 'float', '#inclue') and make
        # bold or something
        print >>f_out, '  ' + line,
    
    # print footer
    print >>f_out, '\\end{Verbatim}'
    
    # close input file
    f_in.close()

    # close output file (if not sys.stdout)
    if f_out != sys.stdout:
        f_out.close()

if __name__ == '__main__':
    sys.exit(main())

