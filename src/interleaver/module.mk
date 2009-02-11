# 
# Makefile for interleaver module
#

# local_dir: local library directory
# 
local_dir	:= src/interleaver
local_lib	:= libinterleaver.a

# local_s_files
#
# This is a list of local source files to compile into objects,
# referenced from the src/ subdirectory under $(local_dir)
#
local_s_files	:=			\
	interleaver.c			\
	interleaver_create.c		\
	interleaver_permute.c

# local_h_files
#
# This is a list of local header files upon which the objects
# depend.  These files are also located under the src/
# subdirectory within $(local_dir)
#
local_h_files	:= interleaver_internal.h

# local_t_files
#
# This is a list of local autotest scripts (header files) which
# are used to generate the autotest program with the 'check'
# target.  These files are located under the tests/ subdirectory
# within $(local_dir)
#
local_t_files	:=			\
	interleaver_autotest.h


# local_b_files
#
# This is a list of local benchmark scripts which are used to
# generate the benchmark program with the 'bench' target.
# These files are located under the bench/ subdirectory within
# $(local_dir)
#
local_b_files	:=			\
	interleaver_benchmark.h


# local_e_files
#
# This is a list of local example files to demonstrate
# functionality of the module.
#
local_e_files	:=			\
	interleaver_example.c

include common.mk

