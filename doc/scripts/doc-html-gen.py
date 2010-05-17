#!/usr/bin/python
# Copyright (c) 2007, 2008, 2009, 2010 Joseph Gaeddert
# Copyright (c) 2007, 2008, 2009, 2010 Virginia Polytechnic
#                                      Institute & State University
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
# doc-html-gen.py
#
# generate html documentation from liquid.tex
#

'''Usage: %s [OPTIONS] <input file(s)>
Generate benchmark source file.

  -v, --version         Print version and exit
  -h, --help            Print this help file and exit
'''

import getopt
import string
import os.path
import sys
from datetime import date
#import glob

# global defaults
default_input_filename = 'liquid.tex'
default_html_filename = 'html/index.html'
default_eqmk_filename = 'html/equations.mk'

# global file identifiers
fid_tex = 0         # main .tex file
fid_eqmk = 0        # equations makefile
fid_eq = 0          # equation .tex file
fid_html = 0        # output html file

# global parameters
equations = []      # list of equations
parse_mode = 0      # parsing mode
equation_id = 0     # equation id

# LaTeX tokens
latex_env_tokens = ['\\begin', '\\end']
latex_eqn_tokens = ['\\[', '\\]']
latex_tokens = ['document', 'chapter', 'section', 'subsection',
                'subsubsection', 'equation', 'enumerate', 'itemize',
                'figure', 'tabular']

def main():
    '''The main program'''

def usage( problem = None ):
    '''Print usage info and exit'''
    if problem is None:
        print usageString()
        sys.exit(0)
    else:
        sys.stderr.write( usageString() )
        abort( str(problem) )

def usageString():
    '''Construct program usage string'''
    return __doc__ % sys.argv[0]

def abort( problem ):
    '''Print error message and exit'''
    sys.stderr.write( '\n' )
    sys.stderr.write( problem )
    sys.stderr.write( '\n\n' )
    sys.exit(2)

def parseLaTeXFile( fid_tex, fid_html ):
    '''Parse LaTeX file'''

    # write html header
    write_html_header(fid_html)

    token = "\\begin{document}"
    this_line_number = 0
    for line in fid_tex.readlines():
        this_line_number += 1
        # search for token
        base_index = string.find(line, token)
        if base_index >= 0:
            print "  token '" + token + "' found on line " + str(this_line_number) + " : " + line
            line_sub = line[len(token)+1:len(line)]

            # search for new line
            end_index = string.find(line_sub, "\n")

            if end_index < 0:
                # no "\n" found: assume we should read entire line
                end_index = len(line_sub)-1

    # write html footer
    write_html_footer(fid_html)


def write_html_header(fid_html):
    '''Write output html header'''

    fid_html.write("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n")
    fid_html.write("\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n");
    fid_html.write("<!-- auto-generated file, do not edit -->\n")
    fid_html.write("<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\">\n")
    fid_html.write("<head>\n")
    fid_html.write("<!-- <style type=\"text/css\" media=\"all\">@import url(http://computing.ece.vt.edu/~jgaeddert/web.css);</style> -->\n")
    fid_html.write("<title>jgaeddert</title>\n")
    fid_html.write("<meta name=\"description\" content=\"Gaeddert Virginia Tech\" />\n")
    fid_html.write("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />\n")
    fid_html.write("<!-- <link rel=\"Shortcut Icon\" type=\"image/png\" href=\"img/favicon.png\" /> -->\n")
    fid_html.write("</head>\n")
    fid_html.write("<body>\n")
    fid_html.write("    <h1>liquid documentation</h1>\n")

def write_html_footer(fid_html):
    '''Write output html footer'''

    fid_html.write("    <!--\n");
    fid_html.write("    <p>\n")
    fid_html.write("    Validate:\n")
    fid_html.write("    <a href=\"http://validator.w3.org/check?uri=https://ganymede.ece.vt.edu/\">XHTML 1.0</a>&nbsp;|\n")
    fid_html.write("    <a href=\"http://jigsaw.w3.org/css-validator/check/referer\">CSS</a>\n")
    fid_html.write("    </p>\n")
    fid_html.write("    -->\n");
    fid_html.write("    <p>Last updated: <em>" + 
                    date.today().strftime("%A, %d %B %Y %I:%M%p") +
                    "</em></p>\n")
    fid_html.write("</body>\n")
    fid_html.write("</html>\n")

def write_equations_makefile(fid_eqmk):
    '''Write output equations makefile'''
    fid_eqmk.write('# equations makefile : auto-generated (do not edit)')

    for eq in equations:
        fid_eqmk.write("%s : %s\n", 'html/eqn/a.png', 'html/eqn/a.tex')

if __name__ == '__main__':
    #if len(sys.argv) < 2:
    #    usage()
    #    sys.exit(1)

    # parse command-line arguments
    #ParseCommandLine()

    filename_tex  = default_input_filename
    filename_html = default_html_filename
    filename_eqmk = default_eqmk_filename

    # open input file
    if not os.path.exists( filename_tex ):
        abort("error: file does not exist: " + filename_tex)
        sys.exit(1)
    fid_tex = open( filename_tex, 'r')

    # open output html file
    if not os.path.exists( filename_html ):
        abort("error: file does not exist: " + filename_html)
        sys.exit(1)
    fid_html = open( filename_html, 'w')

    # run parser
    parseLaTeXFile( fid_tex, fid_html )

    # close files
    fid_tex.close()
    fid_html.close()

    # open output equations makefile
    if not os.path.exists( filename_eqmk ):
        abort("error: file does not exist: " + filename_eqmk)
        sys.exit(1)
    fid_eqmk = open( filename_eqmk, 'w')
    write_equations_makefile( fid_eqmk )
    fid_eqmk.close()

