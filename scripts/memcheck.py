#!/usr/bin/env python3
'''Find memory leaks by iterating through all autotest packages'''
import argparse, subprocess

def memcheck(package):
    '''run command-line'''
    cmd = 'valgrind --tool=memcheck ./xautotest -p %u' % (package)
    out = subprocess.check_output(cmd.split(),stderr=subprocess.STDOUT).decode('utf-8').split('\n')
    out = filter(lambda line: line.startswith('=='), out)
    out = filter(lambda line: 'ERROR SUMMARY' in line, out)
    out = tuple(out)
    return '?' if len(out)==0 else out[0]

for package in range(150):
    v = memcheck(package)
    print('%3u : %s' % (package,v))

