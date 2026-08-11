#!/bin/sh

# Test building source distribution for python

set -e

# deactivate
rm -rf env dist .githash

./scripts/githash.sh

virtualenv env
source env/bin/activate
pip install build
python -m build --sdist
pip install dist/liquid_dsp*.tar.gz --no-binary ':all:'

python3 -c "import json, liquid as dsp; print(json.dumps(dsp.build_info,indent=2))"
# assert dsp.build_info['githash'] != 'unknown'

# if successful, upload to twine with
#   pip install twine
#   python3 -m twine check --strict dist/*
#   python3 -m twine upload dist/*.tar.gz


# clean things up
#   deactivate
#   rm -rf dist env

