#!/usr/bin/env python3
'''Find memory leaks by iterating through all autotest packages'''
import argparse, re, subprocess

def parsecheck(lines):
    '''parse list of valgrind output lines and aggregate results into dictionary'''
    leaks = ('definitely lost', 'indirectly lost', 'possibly lost', 'still reachable', 'suppressed')
    r = {leak:0 for leak in leaks}
    r['errors'] = 0
    for line in lines:
        for leak in leaks:
            m = re.search(leak + ': *([0-9,]*) bytes',line)
            if m is not None:
                r[leak] = int(m.group(1).replace(',',''))
        m = re.search('ERROR SUMMARY: ([0-9]) errors',line)
        if m is not None:
            r['errors'] = int(m.group(1).replace(',',''))
    return r

def memcheck(package):
    '''run valgrind from command-line and check results'''
    cmd = 'valgrind --tool=memcheck ./xautotest -p %u' % (package)
    out = subprocess.check_output(cmd.split(),stderr=subprocess.STDOUT).decode('utf-8').split('\n')
    out = filter(lambda line: line.startswith('=='), out)
    return parsecheck(out)

def checkresults(results):
    '''check results from dictionary and export string summary'''
    if sum(results.values()) == 0:
        return 'pass'
    return str(results)

#for package in range(68,76):
for package in range(150):
    r = memcheck(package)
    print('%3u : %s' % (package,checkresults(r)))

