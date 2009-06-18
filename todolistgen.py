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

outputFileName = "todo_list.txt"
included_files = []
listed_files = []
packages = []
version_number = "0.1.0"

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
    '''Write output file'''
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


