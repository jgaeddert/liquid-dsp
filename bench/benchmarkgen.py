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
# Benchmark generator script
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

outputFileName = "benchmark_include.h"
included_headers = []
listed_headers = []
packages = []
benchmark_functions = []
version_number = "0.2.0"

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
    global outputFileName, listed_headers
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
            #listed_headers.append( a )

    for h in patterns:
        listed_headers.append( h )

def printVersion():
    '''Print version and exit'''
    sys.stdout.write( "This is benchtest version " + version_number + "\n" )
    sys.exit(0)

class Package:
    def __init__(self, _header_file=""):
        self.header_file = _header_file

        # split base file name from path, remove extension
        self.name = os.path.splitext(os.path.basename(self.header_file))[0]
        self.benchmark_index = 0
        self.num_benchmarks = 0

    def Print(self):
        for i in range(0,self.num_benchmarks):
            benchmark_functions[self.benchmark_index+i].Print()

class BenchmarkFunction:
    def __init__(self, _function_name="", _short_name="", _file_name="", _line_number=0):
        self.function_name = _function_name
        self.short_name = _short_name
        self.file_name = _file_name
        self.line_number = _line_number

    def Print(self):
        print "  " + self.short_name + "(), " + self.file_name + ":" + str(self.line_number)

def parseHeader( filename ):
    '''Parse header file and searching for void benchmark_'''
    # add header to list
    if not filename in included_headers:
        p = Package(filename)
        p.benchmark_index = len(benchmark_functions)
    else:
        return

    if not os.path.exists(filename):
        abort("error: file does not exist: " + filename)
        return

    header_file = open(filename, 'r')
    function_key = "void benchmark_"
    this_line_number = 0
    for line in header_file.readlines():
        this_line_number += 1
        # search for "void benchmark_"
        base_index = string.find(line, function_key)
        if base_index >= 0:
            line_sub = line[len(function_key)+1:len(line)]

            # search for space or "(" or new line
            # TODO also search for tab
            end_index_newline = len(line_sub)-1
            end_index_space   = string.find(line_sub, " ")
            end_index_paren   = string.find(line_sub, "(")

            # use smallest end index greater than zero
            end_index = end_index_newline

            if end_index_space > 0:
                end_index = min(end_index,end_index_space)

            if end_index_paren > 0:
                end_index = min(end_index,end_index_paren)

            function_name = line[base_index+5:end_index+len(function_key)+1]
            short_name = line[base_index+len(function_key):end_index+len(function_key)+1]
            f = BenchmarkFunction(function_name, short_name, filename, this_line_number)
            f.Print()
            benchmark_functions.append(f)
            p.num_benchmarks += 1

    if p.num_benchmarks > 0:
        included_headers.append(filename)
        packages.append(p)

    header_file.close()

def writeOutputFile( filename = outputFileName ):
    '''Write output file'''
    f = open(outputFileName, "w")

    # write header
    f.write("// auto-generated file, do not edit\n")
    f.write("//\n")
    f.write("// created with benchtest v" + version_number + "\n\n");

    # include guard
    f.write("#ifndef __BENCHINCLUDE_H__\n");
    f.write("#define __BENCHINCLUDE_H__\n\n");
    f.write("#define BENCHMARK_VERSION " + "\"" + version_number + "\"" + "\n\n");

    #
    # benchmarks declarations
    #
    f.write("// benchmark arguments pre-processor directive\n");
    f.write("#define BENCHMARK_ARGS                 \\\n");
    f.write("   struct rusage * _start,             \\\n");
    f.write("   struct rusage * _finish,            \\\n");
    f.write("   unsigned long int * _num_iterations\n");
    f.write("\n");

    f.write("// function declarations\n");
    for i in range(len(benchmark_functions)):
        bf = benchmark_functions[i]
        # append benchmark function to array
        f.write("void " + bf.function_name + "(BENCHMARK_ARGS);\n");
    f.write("\n")

    f.write("// number of benchmarks\n");
    f.write("#define NUM_BENCHMARKS " + str(len(benchmark_functions)) + "\n\n");

    #
    # benchmarks array
    #
    f.write("// array of benchmarks\n");
    f.write("bench_t benchmarks[NUM_BENCHMARKS] = {\n")
    for i in range(len(benchmark_functions)):
        bf = benchmark_functions[i]
        # append benchmark function to array
        f.write(" "*4 + "{" + str(i) + ",&" + bf.function_name + ",\"" + bf.short_name + "\",0,0.0f,0.0f}")
        if not i == len(benchmark_functions)-1:
            f.write(",\n")
    f.write("\n};\n")

    f.write("\n")
    f.write("// number of packages\n");
    f.write("#define NUM_PACKAGES " + str(len(packages)) + "\n\n");

    f.write("// array of packages\n");
    f.write("package_t packages[NUM_PACKAGES] = {\n")
    for i in range(len(packages)):
        p = packages[i];
        # append packages to array
        f.write(" "*4 + "{" + str(i) + "," + str(p.benchmark_index) + "," + str(p.num_benchmarks) + ",\"" + p.name + "\"}")
        if not i == len(packages)-1:
            f.write(",\n")
    f.write("\n};\n")


    # include guard
    f.write("#endif // __BENCHINCLUDE_H__\n");

    f.close()
    
if __name__ == '__main__':
    if len(sys.argv) < 2:
        usage()
        sys.exit(0)

    # parse command-line arguments
    ParseCommandLine()

    # add null package/benchmark
    #p = Package("bench.c");
    p = Package("null_benchmark.h");
    p.benchmark_index = len(benchmark_functions)
    f = BenchmarkFunction("benchmark_nullbench", "nullbench", "bench.c", 0);
    f.Print();
    p.num_benchmarks += 1
    benchmark_functions.append(f);
    included_headers.append("bench.c")
    packages.append(p)

    # initialize on single file
    for h in listed_headers:
        print "parsing " + h + "..."
        parseHeader( h )

    # write output file
    if len(packages) > 0:
        writeOutputFile(outputFileName)
    else:
        print "no benchmarks could be found"


