#!/usr/bin/python
# Copyright (c) 2007, 2009 Joseph Gaeddert
# Copyright (c) 2007, 2009 Virginia Polytechnic Institute & State University
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
# TODO list generator
#

'''Usage: %s [OPTIONS] <input file(s)>
Generate benchmark source file.

  -v, --version         Print version and exit
  -h, --help            Print this help file and exit
  -o, --output=NAME     Write output to file NAME
'''

import re
import sys
import getopt
import glob
import string
import os.path
from datetime import date

outputFileName = "todolist.txt"
included_files = []
listed_files = []
packages = []
version_number = "0.1.0"
repo = "github" # github [github.com/jgaeddert], ganymede [ganymede.ece.vt.edu]

def main():
    '''The main program'''
    files = parseCommandline()
    scanInputFiles( files )
    writeOutput()

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

def ParseCommandLine():
    '''Analyze command line arguments'''
    try:
        options, patterns = getopt.getopt( sys.argv[1:], 'vho:',
                                           ['version', 'help', 'output=',] )
    except getopt.error, problem:
        usage( problem )
    setOptions( options, patterns )

def setOptions( options, patterns ):
    '''Set options specified on command line'''
    global outputFileName, listed_files
    for o, a in options:
        if o in ('-v', '--version'):
            printVersion()
        if o in ('-h', '--help'):
            usage()
        elif o in ('-o', '--output'):
            outputFileName = a
        #elif o in ('i', '--include'):
            #if not re.match( r'^["<].*[>"]$', a ):
            #    a = ('"%s"' % a)
            #listed_files.append( a )

    for h in patterns:
        listed_files.append( h )

def printVersion():
    '''Print version and exit'''
    sys.stdout.write( "version " + version_number + "\n" )
    sys.exit(0)

class Package:
    def __init__(self, _file_name=""):
        self.file_name = _file_name

        self.tasks = []

    def Print(self):
        print self.file_name + ":"
        for t in self.tasks:
            t.Print()

class Task:
    def __init__(self, _file_name="", _line_number=0, _task=""):
        self.file_name = _file_name
        self.line_number = _line_number
        self.task = _task

    def Print(self):
        print "  " + str(self.line_number) + " : " + self.task

def parseFile( filename ):
    '''Parse file and searching for TODO'''
    # add header to list
    if not filename in included_files:
        p = Package(filename)
    else:
        return

    if not os.path.exists(filename):
        abort("error: file does not exist: " + filename)
        return

    #print "parsing file " + filename
    header_file = open(filename, 'r')
    function_key = "TODO"
    this_line_number = 0
    for line in header_file.readlines():
        this_line_number += 1
        # search for function_key
        base_index = string.find(line, function_key)
        if base_index >= 0:
            #print "  TODO found on line " + str(this_line_number) + " : " + line
            line_sub = line[len(function_key)+1:len(line)]

            # search for new line
            end_index = string.find(line_sub, "\n")

            if end_index < 0:
                # no "\n" found: assume we should read entire line
                end_index = len(line_sub)-1

            function_name = line[base_index:end_index+len(function_key)+1]
            t = Task(filename, this_line_number, function_name)
            p.tasks.append(t)

    if len(p.tasks) > 0:
        included_files.append(filename)
        packages.append(p)

    header_file.close()

def writeOutputFile( filename = outputFileName ):
    '''Parse file type, write appropriate file'''
    substr = filename.split('.')
    ext = substr[len(substr)-1]
    if ext.lower() == 'html':
        writeOutputHTMLFile(filename)
    else:
        writeOutputTextFile(filename)

def writeOutputHTMLFile( filename = outputFileName ):
    '''Write output HTML file'''
    f = open(outputFileName, "w")
    f.write("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n")
    f.write("\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n");
    f.write("<!-- auto-generated file, do not edit -->\n")
    f.write("<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\">\n")
    f.write("<head>\n")
    f.write("<!-- <style type=\"text/css\" media=\"all\">@import url(http://computing.ece.vt.edu/~jgaeddert/web.css);</style> -->\n")
    f.write("<title>liquid-dsp : todo list</title>\n")
    f.write("<meta name=\"description\" content=\"Gaeddert Virginia Tech\" />\n")
    f.write("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />\n")
    f.write("<!-- <link rel=\"Shortcut Icon\" type=\"image/png\" href=\"img/favicon.png\" /> -->\n")
    f.write("</head>\n")
    f.write("<body>\n")
    f.write("    <h1>liquid // todolist</h1>\n")

    if repo == "ganymede":
        # gitweb example : http://ganymede.ece.vt.edu/projects/liquid-dsp/.git/?p=liquid-dsp/.git;a=blob;f=src/fec/src/fec.c;hb=HEAD
        basepath = "http://ganymede.ece.vt.edu/projects/liquid-dsp/.git/?p=liquid-dsp/.git;a=blob;f="
    elif repo == "github":
        # github example : http://github.com/jgaeddert/liquid-dsp/blob/master/src/modem/src/modem_demodulate.c#L29
        basepath = "http://github.com/jgaeddert/liquid-dsp/blob/master/"
    else:
        sys.stderr.write( "error: unknown repo option : " + repo + "\n" )
        sys.exit(2)

    # write list
    for p in packages:
        if repo == "ganymede":
            long_filename = basepath + p.file_name + ";hb=HEAD"
        elif repo == "github":
            long_filename = basepath + p.file_name
        else:
            sys.stderr.write( "error: unknown repo option : " + repo + "\n" )
            sys.exit(2)

        f.write("    <p><a href=\"" + long_filename + "\">" + p.file_name + "</a></p>\n")
        f.write("    <ul>\n");
        for t in p.tasks:
            if repo == "ganymede":
                # ganymede.ece.vt.edu
                f.write("      <li>" + "[<a href=\"" + long_filename + "#l" + str(t.line_number) + "\">" +
                        str(t.line_number) + "</a>] : " + t.task + "</li>\n")
            elif repo == "github":
                # github.com
                f.write("      <li>" + "[<a href=\"" + long_filename + "#L" + str(t.line_number) + "\">" +
                        str(t.line_number) + "</a>] : " + t.task + "</li>\n")
            else:
                sys.stderr.write( "error: unknown repo option : " + repo + "\n" )
                sys.exit(2)
        # end the list
        f.write("    </ul>\n");

    # write footer
    f.write("    <p>\n")
    f.write("    Validate:\n")
    f.write("    <a href=\"http://validator.w3.org/check?uri=http://ganymede.ece.vt.edu/todolist.html\">XHTML 1.0</a>&nbsp;|\n")
    #f.write("    <a href=\"http://jigsaw.w3.org/css-validator/check/referer\">CSS</a>\n")
    f.write("    </p>\n")
    f.write("    <p>Last updated: <em>" + 
                    date.today().strftime("%A, %d %B %Y %I:%M%p") +
                    "</em></p>\n")
    f.write("</body>\n")
    f.write("</html>\n")
    f.close()

def writeOutputTextFile( filename = outputFileName ):
    '''Write output text file'''
    f = open(outputFileName, "w")

    # write header
    f.write("// auto-generated file, do not edit\n")
    f.write("\n")
    for p in packages:
        f.write("\n")
        f.write(p.file_name + "\n")
        for t in p.tasks:
            f.write("  " + str(t.line_number) + " : " + t.task + "\n")

    f.close()
    
if __name__ == '__main__':
    if len(sys.argv) < 2:
        usage()
        sys.exit(0)

    # parse command-line arguments
    ParseCommandLine()

    # initialize on single file
    for h in listed_files:
        parseFile( h )

    # write output file
    if len(packages) > 0:
        writeOutputFile(outputFileName)
    else:
        print "no tasks could be found"


