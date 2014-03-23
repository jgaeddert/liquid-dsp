#!/usr/bin/python

# this script updates the copyright license lines in all directories
# to do two things:
#   1. Removes lines like 'Copyright (c) ... Virginia Polytechnic...' and
#      ' ... Institute & State University', noting that sometimes this is
#      split across several lines
#   2. replaces lines like "Copyright 2007, ... 2012, 2013 Joseph Gaeddert"
#      with an updated date
# 
# usage:
#   $ python scripts/update_copyright.py <dir>
#

import os
import sys
import re

rootdir = sys.argv[1]

# specific files to ignore
ignore_files = ['update_copyright.py',]

# directories to ignore
ignore_directories = ['.git',]

# only look at these extensions
include_extensions = ['.h', '.c', '.md', '.tex', '.ac', '.in', '.m',]

# new copyright string
copyright_new = "Copyright (c) 2007 - 2014 Joseph Gaeddert"

#
def update_copyright(filename=""):
    """
    Update copyright on file.
    """
    # try to open file
    contents_old = []
    with open( filename, 'r' ) as f:
        # read lines from file (removing newline character at end)
        for line in f:
            #contents_old.append(line.strip('\n\r'))
            contents_old.append(line)

    # parse contents
    contents_new = []
    for line in contents_old:
        if re.search(r'Copyright.*Virginia Poly',line):
            # delete line
            pass
        elif re.search(r'Institute & State University',line):
            # delete line
            pass
        elif re.search( r'Copyright \(c\)', line ):
            # replace with new copyright
            contents_new.append( re.sub(r'Copyright \(c\).*', copyright_new, line) )
        else:
            # regular line
            contents_new.append(line)

    # open original file for writing
    with open( filename, 'w' ) as f:
        for line in contents_new:
            f.write(line)

#
for root, subFolders, files in os.walk(rootdir):

    # strip off leading './' if it exists
    if root.startswith('./'):
        root = root[2:]

    # print root directory
    print root
    
    # ignore certain directories
    if root in ignore_directories:
        print "(skipping directory)"
        continue

    # print subfolders
    #for folder in subFolders:
    #    print "%s has subdirectory %s" % (root, folder)

    # parse each filename in directory
    for filename in files:
        filePath = os.path.join(root, filename)

        # check filename
        if filePath in ignore_files:
            print "  " + filePath + " (ignoring this specific file)"
            continue;

        # check filename extension
        baseName, extension = os.path.splitext(filename)
        if extension not in include_extensions:
            print "  " + filePath + " (improper extension; ignoring file)"
            continue;

        # continue on with this file
        print "  " + filePath
        update_copyright(filePath)


