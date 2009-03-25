#!/bin/sh
#
# Useful shell script for profiling code.
#
# Example:
#   sudo ./oprof.sh ./benchmark -p0 -c1 -n1000000

if [ $# -lt 1 ]; then
    echo "need command"
    exit 1
fi

opcontrol --setup --no-vmlinux
opcontrol --reset
opcontrol --start

# run command, e.g: ./benchmark -p0 -c2.0e9 -n1000000
$@

opcontrol --shutdown
opannotate --source | vim -

