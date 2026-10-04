#!/usr/bin/env python3
'''Parse liquid-dsp source files to generate a registry header file.

Works for either autotests or benchmarks, selected via -type. Scans source
files for LIQUID_AUTOTEST(...) or LIQUID_BENCHMARK(...) macro invocations and
emits a header with one extern declaration per discovered symbol plus a
NULL-terminated array of pointers to the companion structs.
'''
import argparse, json, re, os, sys

# per-type configuration: macro to scan for, emitted C type/struct/array names,
# include header, default output path, directories to ignore during traversal,
# and the noun used in the summary line
CONFIG = {
    'autotest': {
        'macro':   'LIQUID_AUTOTEST',
        'include': 'liquid.autotest.h',
        'struct':  'liquid_autotest_s',
        'ctype':   'liquid_autotest',
        'array':   'liquid_autotest_registry',
        'guard':   '__LIQUID_AUTOTEST_REGISTRY_H__',
        'output':  'autotest/liquid_autotest_registry.h',
        'ignore':  ('examples', 'sandbox'),
        'noun':    'tests',
    },
    'benchmark': {
        'macro':   'LIQUID_BENCHMARK',
        'include': 'liquid.benchmark.h',
        'struct':  'liquid_benchmark_s',
        'ctype':   'liquid_benchmark',
        'array':   'liquid_benchmarks',
        'guard':   '__LIQUID_BENCHMARK_REGISTRY_H__',
        'output':  'benchmark/liquid_benchmark_registry.h',
        'ignore':  ('examples', 'sandbox', 'tests'),
        'noun':    'benchmarks',
    },
}

def main(argv=None):
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('-type',   required=True, choices=sorted(CONFIG.keys()),
                   help='registry type to generate (autotest or benchmark)')
    p.add_argument('-path',   default='.', type=str, help='input path to src')
    p.add_argument('-output', default=None, type=str, help='output file (defaults per type)')
    args = p.parse_args()

    cfg = CONFIG[args.type]
    if args.output is None:
        args.output = cfg['output']

    # get list of all potential source files by recursively parsing directories
    source_files = get_source_files(args.path, cfg['ignore'])
    #print(json.dumps(source_files,indent=2))

    # open output and print header
    fid = open(args.output,'w')
    fid.write('#ifndef %s\n' % (cfg['guard'],))
    fid.write('#define %s\n' % (cfg['guard'],))
    fid.write('\n')
    fid.write('#include "%s"\n' % (cfg['include'],))
    fid.write('\n')

    # parse files
    all_sources = []
    all_items = []
    for file in source_files:
        items = parse_source(file, cfg['macro'])
        all_items.extend(items)
        if len(items) > 0:
            all_sources.append(file)
            fid.write('// %s\n' % (file,))
            for item in items:
                fid.write('extern struct %s %s_s;\n' % (cfg['struct'], item,))

    fid.write('\n')
    fid.write('// compile %s registry\n' % (args.type,))
    fid.write('%s %s[] =\n' % (cfg['ctype'], cfg['array'],))
    fid.write('{\n')
    for item in all_items:
        fid.write('    &%s_s,\n' % (item,))
    fid.write('    NULL\n')
    fid.write('};\n')

    # finish output file
    fid.write('\n')
    fid.write('#endif // %s\n' % (cfg['guard'],))
    fid.write('\n')

    print('found %u %s across %u source files' % (len(all_items), cfg['noun'], len(all_sources)))

def _is_git_worktree(path):
    '''Check if a directory is a git worktree by looking for a .git file (not directory)'''
    git_path = os.path.join(path, '.git')
    if os.path.isfile(git_path):
        with open(git_path, 'r') as f:
            return f.read().startswith('gitdir:')
    return False

def get_source_files(path:str = '.', ignore:tuple = ()):
    '''get a list of source files with potential registry entries'''
    source_files = []
    for root, dirs, files in os.walk(path):
        #print("root: ", root)
        #print("dirs: ", dirs)
        #print("files:", files)

        # prune git worktrees from traversal
        dirs[:] = [d for d in dirs if not _is_git_worktree(os.path.join(root, d))]

        # ignore certain directories
        if os.path.split(root)[-1] in ignore:
            pass
        else:
            # look only at source files (e.g. have '.c' extension)
            files = filter(lambda x: os.path.splitext(x)[-1]=='.c', files)
            # provide full path
            source_files.extend([root + '/' + f for f in files])
    return sorted(source_files)

def parse_source(path:str, macro:str):
    '''parse source file and find registry definitions for the given macro'''
    # e.g. LIQUID_AUTOTEST(firfilt_crcf_basic_2, "basic filter test", "a,b,c", 0.1)
    # e.g. LIQUID_BENCHMARK(firfilt_crcf_4, "firfilt_crcf execute, n=4", "FIR,filter")
    items = []
    p = re.compile('%s *\\( *([a-zA-Z0-9_]*)' % (macro,))
    with open(path,'r') as fid:
        for line in fid.readlines():
            m = p.search(line)
            if m is not None:
                items.append(m.group(1))
    return items

if __name__ == '__main__':
    sys.exit(main())
