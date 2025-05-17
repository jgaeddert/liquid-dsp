#!/usr/bin/env python3
'''Run valgrind on autotests and catalog memory errors'''
import argparse, functools, json, re, os, sys, subprocess

def generate_json(path: str='valgrind', overwrite=False):
    '''load autotest .json summary file, create one if needed'''
    filename = path + '/init.json'
    if overwrite or not os.path.exists(filename):
        os.makedirs(path, exist_ok=True)
        os.system('./xautotest -t0 -o %s' % (filename,))
    return json.load(open(filename,'r'))

def get_test_filename(test: dict, path: str='valgrind'):
    '''get a consistent filename for a specific test'''
    return '%s/memcheck.%.4u.%s.log' % (path, test['id'], test['name'])

def run_test(test: dict, opts: str='--tool=memcheck --leak-check=full --track-origins=yes',
        path: str='valgrind', seed: int=1, debug: bool=True, dry_run: bool=False):
    '''run a specific test'''
    filename = get_test_filename(test, path)
    cmd = 'valgrind %s --log-file=%s ./xautotest -t %u -R %u' % (opts, filename, test['id'], seed)
    if debug or dry_run:
        print(cmd)
    if not dry_run:
        os.system(cmd)

def parse_results(test: dict, path: str='valgrind'):
    '''parse results of single test and return dictionary of error values'''
    filename = get_test_filename(test, path)
    leaks = ('definitely lost', 'indirectly lost', 'possibly lost', 'still reachable', 'suppressed')
    r = {leak:0 for leak in leaks}
    r['errors'] = 0
    try:
        fid = open(filename,'r')
    except FileNotFoundError:
        return # test was probably not run
    for line in fid:
        for leak in leaks:
            m = re.search(leak + ': *([0-9,]*) bytes',line)
            if m is not None:
                r[leak] = int(m.group(1).replace(',',''))
        m = re.search('ERROR SUMMARY: ([0-9]) errors',line)
        if m is not None:
            r['errors'] = int(m.group(1).replace(',',''))
    return r

def summary(tests: list, path: str='valgrind'):
    '''parse results of all tests and print summary'''
    total_errors, total_tests = 0, 0
    tests_with_errors = []
    for test in tests:
        summary = parse_results(test, path)
        if summary is not None:
            print(test, summary)
            total_tests += 1
            error_count = functools.reduce(lambda a,b: a+b, summary.values())
            total_errors += error_count
            if error_count > 0:
                tests_with_errors.append(test['name'])

    print(f'\nfound %s total errors in %u test%s across %u total test%s%s' % (
        total_errors,
        len(tests_with_errors),
        "" if len(tests_with_errors)==1 else "s",
        total_tests,
        "" if total_tests==1 else "s",
        ":" if len(tests_with_errors)>0 else ""));
    for test in tests_with_errors:
        print('  ', test)
    return total_errors

def main(argv=None):
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('-output', default='valgrind', type=str, help='output directory')
    p.add_argument('-summary',action='store_true',          help='rather than run tests, parse output logs and print summary of results')
    p.add_argument('-search', default=None,       type=str, help='run tests whose name matches search string')
    p.add_argument('-test',   default=None,       type=int, help='run a specific test at index')
    p.add_argument('-exclude',default=[], action='append',  help='exclude tests that match a particular search string')
    #p.add_argument('-skip',   default=None,       type=str, help='skip tests, e.g. "12,17,44-48,123"')
    #p.add_argument('-jobs',   default=1,          type=int, help='number of processes to run concurrently')
    p.add_argument('-dry-run',action='store_true',          help='print tests to run without actually running them')
    args = p.parse_args()

    # generate and load .json object
    v = generate_json(args.output)

    # just print a summary
    if args.summary:
        total_errors = summary(v['tests'], args.output)
        sys.exit(total_errors)

    # set additional configuration options
    opts = '--tool=memcheck --leak-check=full --track-origins=yes'

    # build up a list of tests to run from the command-line specification
    if args.test is not None:
        tests = [v['tests'][arg.test],]
    elif args.search is not None:
        # run all tests matching search string
        tests = list(filter(lambda t: t['name'].lower().__contains__(args.search.lower()), v['tests']))
    else:
        # iterate over all tests and execute
        tests = v['tests']

    # exclude tests that match strings
    for exclude in args.exclude:
        tests = list(filter(lambda t: not t['name'].lower().__contains__(exclude.lower()), tests))

    # run all tests specified
    # TODO: use popen and run concurrent jobs
    print("running %u tests...\n" % (len(tests)))
    for test in tests:
        run_test(test, opts, path=args.output, seed=v['rseed'], dry_run=args.dry_run)

if __name__ == '__main__':
    sys.exit(main())

