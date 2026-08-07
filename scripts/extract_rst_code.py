#!/usr/bin/env python3
"""Extract named code blocks from RST files using docutils.

Parses RST files for '.. code-block::' directives with a ':name:' option
and writes the code content to the named output file.  Standard include-path
substitutions are applied automatically.

Requires: docutils (pip install docutils)

Usage:
  python3 scripts/extract_rst_code.py README.rst [-o output_dir]
"""

import sys
import os
import argparse
from docutils.core import publish_doctree
from docutils import nodes

# Standard substitutions applied to all extracted blocks
SUBSTITUTIONS = [
    ('<liquid/liquid.h>',  '"liquid.h"'),
    ('<liquid/liquid.hh>', '"liquid.hh"'),
]


def extract(filepath, output_dir='.'):
    """Parse RST file and write named code blocks to output_dir."""
    with open(filepath) as f:
        doctree = publish_doctree(f.read(), source_path=filepath,
                                  settings_overrides={'report_level': 4})

    for node in doctree.findall(nodes.literal_block):
        names = node.get('names', [])
        if not names:
            continue

        text = node.astext()

        # apply substitutions
        for old, new in SUBSTITUTIONS:
            text = text.replace(old, new)

        lines = text.splitlines()

        for name in names:
            path = os.path.join(output_dir, name)
            with open(path, 'w') as f:
                f.write('\n'.join(lines) + '\n')
            print(f"  extracted {name} ({len(lines)} lines)")


def main():
    parser = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    parser.add_argument('rst_file', help='Input RST file to parse')
    parser.add_argument('-o', '--output-dir', default='.',
                        help='Output directory (default: current directory)')
    
    args = parser.parse_args()
    
    print(f"extracting code blocks from {args.rst_file}...")
    extract(args.rst_file, args.output_dir)


if __name__ == '__main__':
    main()
