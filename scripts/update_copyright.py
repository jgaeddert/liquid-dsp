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

# new copyright
newcopy = []
newcopy.append('Copyright (c) 2007 - 2014 Joseph Gaeddert\n')
newcopy.append('You may use this for whatever you want\n')
newcopy.append('Hokies rule.\n')

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
    contents = []
    with open( filename, 'r' ) as f:
        # read lines from file (removing newline character at end)
        for line in f:
            #contents.append(line.strip('\n\r'))
            contents.append(line)

    # parse contents
    index_start = -1
    index_stop  = -1

    # search for copyright; starts at top of file
    for i in range(min(10, len(contents))):
        if re.search(r'Copyright .* Joseph Gaeddert',contents[i]):
            index_start = i
            break

    if index_start == -1:
        print '  >> no copyright found'
        return

    # look for end of copyright
    #for i in range(index_start+15,index_start+15+min(10, len(contents))):
    i = index_start + 15
    if re.search(r'along with liquid.  If not, see',contents[i]):
        index_stop = i
    else:
        print '    >> could not find end of copyright'
        return

    # check comment type
    m = re.search(r'^( \*|#+) +Copyright',contents[index_start])
    if m:
        comment = m.group(1)
    else:
        raise Exception('unexpected error')

    # delete items in list
    contents.__delslice__(index_start, index_stop+1)

    # insert new copyright
    for i in range(len(newcopy)):
        contents.insert(index_start+i, comment + ' ' + newcopy[i])

    # print...
    for line in contents:
        print line.rstrip()

    # open original file for writing
    with open( filename, 'w' ) as f:
        for line in contents:
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


