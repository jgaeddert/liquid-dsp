#!/usr/bin/env python3
'''Parse autotest header files to generate registry'''
import argparse, functools, json, re, os, sys

def main(argv=None):
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('-path',   default='src',        type=str, help='input path to src')
    p.add_argument('-output', default='autotest_include.h', type=str, help='output file')
    args = p.parse_args()

    # get list of all potential source files by recursively parsing directories
    source_files = []
    for root, dirs, files in os.walk(args.path):
        #print("root: ", root)
        #print("dirs: ", dirs)
        #print("files:", files)

        # look only in 'tests' directory
        if os.path.split(root)[-1] == 'tests':
            # look only at source files (e.g. have '.c' extension)
            files = filter(lambda x: os.path.splitext(x)[-1]=='.c', files)
            # provide full path
            source_files.extend([root + '/' + f for f in files])

    #print(json.dumps(source_files,indent=2))

    for file in source_files:
        tests = parse_source_legacy(file)
        if len(tests) > 0:
            print('//', file)
            for test in tests:
                print('%s;' % (test,))

def get_source_files(path:str = '.'):
    '''get a list of autotest files with potential tests'''
    return ('src/filter/tests/rresamp_crcf_autotest.c')

def parse_source_legacy(path:str):
    '''parse source file and find test definitions (legacy method)'''
    # look for "void autotest_..."
    # void autotest_rresamp_crcf_baseline_P1_Q5() { test_rresamp_crcf("baseline", 1, 5, 15, -1, 60.0f); }
    autotests = []
    p = re.compile('void (autotest_[a-zA-Z0-9_]*)')
    with open(path,'r') as fid:
        for line in fid.readlines():
            m = p.search(line)
            if m is not None:
                autotests.append(m.group(1))
    return autotests

def parse_source(path:str):
    '''parse source file and find test definitions'''
    # look for "LIQUID_AUTOTEST(...)
    # LIQUID_AUTOTEST(firfilt_crcf_basic_2, "basic filter test", "a,b,c", 0.1)
    autotests = []
    search = re.search(leak + ': *([0-9,]*) bytes',line)
    with open(path,'r') as fid:
        for line in fid.readlines():
            pass
    return autotests

if __name__ == '__main__':
    sys.exit(main())

