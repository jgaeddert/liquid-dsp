#!/usr/bin/env python3
'''Parse autotest header files to generate registry'''
import argparse, functools, json, re, os, sys

def main(argv=None):
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('-path',   default='.', type=str, help='input path to src')
    p.add_argument('-output', default='autotest/liquid_autotest_registry.h', type=str, help='output file')
    args = p.parse_args()

    # get list of all potential source files by recursively parsing directories
    source_files = get_source_files(args.path)
    #print(json.dumps(source_files,indent=2))

    # open output and print header
    fid = open(args.output,'w')
    fid.write('#ifndef __LIQUID_AUTOTEST_REGISTRY_H__\n')
    fid.write('#define __LIQUID_AUTOTEST_REGISTRY_H__\n')
    fid.write('\n')
    fid.write('#include "liquid.autotest.h"\n')
    fid.write('\n')

    # parse files
    all_sources = []
    all_tests = []
    for file in source_files:
        tests = parse_source(file)
        all_tests.extend(tests)
        if len(tests) > 0:
            all_sources.append(file)
            fid.write('// %s\n' % (file,))
            for test in tests:
                fid.write('extern struct liquid_autotest_s %s_s;\n' % (test,))

    fid.write('\n')
    fid.write('// compile test registry\n')
    fid.write('liquid_autotest liquid_autotest_registry[] =\n')
    fid.write('{\n')
    for test in all_tests:
        fid.write('    &%s_s,\n' % (test,))
    fid.write('    NULL\n')
    fid.write('};\n')

    # finish output file
    fid.write('\n')
    fid.write('#endif // __LIQUID_AUTOTEST_REGISTRY_H__\n')
    fid.write('\n')

    print('found %u tests across %u source files' % (len(all_tests),len(all_sources)))

def get_source_files(path:str = '.'):
    '''get a list of autotest files with potential tests'''
    source_files = []
    for root, dirs, files in os.walk(path):
        #print("root: ", root)
        #print("dirs: ", dirs)
        #print("files:", files)

        # look only in 'tests' directory
        if os.path.split(root)[-1] in ('sandbox',):
            pass
        else:
            # look only at source files (e.g. have '.c' extension)
            files = filter(lambda x: os.path.splitext(x)[-1]=='.c', files)
            # provide full path
            source_files.extend([root + '/' + f for f in files])
    return source_files

def parse_source(path:str):
    '''parse source file and find test definitions'''
    # look for "LIQUID_AUTOTEST(...)
    # LIQUID_AUTOTEST(firfilt_crcf_basic_2, "basic filter test", "a,b,c", 0.1)
    autotests = []
    p = re.compile('LIQUID_AUTOTEST *\\( *([a-zA-Z0-9_]*)')
    with open(path,'r') as fid:
        for line in fid.readlines():
            m = p.search(line)
            if m is not None:
                autotests.append(m.group(1))
    return autotests

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

if __name__ == '__main__':
    sys.exit(main())

