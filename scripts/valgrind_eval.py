#!/usr/bin/env python3
'''Run valgrind on each autotest and catalog memory errors'''
import argparse, json, os, sys, subprocess

def generate_json(path):
    filename = path + '/init.json'
    os.system('./xautotest -t0 -o %s' % (filename,))
    return json.load(open(filename,'r'))

def main(argv=None):
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('-output', default='valgrind', type=str, help='output directory')
    args = p.parse_args()

    v = generate_json(args.output)
    print(v)

    # iterate over all tests and execute
    opts = '--tool=memcheck --leak-check=full --track-origins=yes'
    for i,test in enumerate(v['tests']):
        filename = '%s/memcheck.%.4u.%s.log' % (args.output, test['id'], test['name'])
        cmd = 'valgrind %s --log-file=%s ./xautotest -t %u' % (opts, filename, test['id'],)
        print(cmd)
        os.system(cmd)

        # TODO parse result

if __name__ == '__main__':
    sys.exit(main())

