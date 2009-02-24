# 
# Makefile for estimation module
#

# local_dir: local library directory
# 
local_dir	:= $(modulesdir)/estimation
local_lib	:= libestimation.a

# local_s_files
#
# This is a list of local source files to compile into objects,
# referenced from the $(modulesdir) subdirectory under $(local_dir)
#
local_s_files	:= frequency_offset.c

# local_h_files
#
# This is a list of local header files upon which the objects
# depend.  These files are also located under the $(modulesdir)
# subdirectory within $(local_dir)
#
local_h_files	:=

# local_t_files
#
# This is a list of local autotest scripts (header files) which
# are used to generate the autotest program with the 'check'
# target.  These files are located under the tests/ subdirectory
# within $(local_dir)
#
local_t_files	:= estimation_autotest.h


# local_b_files
#
# This is a list of local benchmark scripts which are used to
# generate the benchmark program with the 'bench' target.
# These files are located under the bench/ subdirectory within
# $(local_dir)
#
local_b_files	:= #frequency_offset_benchmark.h


include common.mk

